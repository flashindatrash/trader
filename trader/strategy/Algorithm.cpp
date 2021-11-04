#include "Algorithm.hpp"

#include <utility>
#include "Time.hpp"
#include "Logger.hpp"
#include "Context.hpp"
#include "Position.hpp"
#include "Statistics.hpp"
#include "Terminal.hpp"
#include "Migrator.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/indicator/DEMA.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    auto* algorithm = new Algorithm(settings);
    return algorithm;
}

Algorithm::Algorithm(Settings settings)
    : _settings(std::move(settings))
{
}

Algorithm::~Algorithm() {
    if (_position != nullptr) {
        delete _position;
        _position = nullptr;
    }

    if (_statistics != nullptr) {
        delete _statistics;
        _statistics = nullptr;
    }
}

bool Algorithm::init() {
    Terminal::setTitle(_settings.symbol);

    // создадим структуры для хранения данных
    _position = Position::create(_settings.uniqId() + ":position");
    _statistics = Statistics::create(_settings.uniqId() + ":stats");

    // промигрируем данные
    if (not Migrator::migrate(_position, _statistics, _settings.symbol))
        return false;

    if (_settings.isRelease() || true)
        DB().rpush(_settings.username + ":events", "start " + (std::string)_settings.symbol + " pair");
        //Events().send("start %s pair", _settings.symbol.c_str());

    return true;
}

void Algorithm::execute(const Context& context) {
    // закрытие сделок: получить профит || усреднение цены || остановить убыток
    bool open = tryTakeProfit(context) || tryAverage(context) || tryStopLoss(context);
    // открытие сделок
    bool close = tryOpen(context);

    // обновляем строку терминала
    if (_position->has() && not _settings.isBackTest())
        Terminal::update(*_position, _settings.symbol, context);
}

bool Algorithm::tryTakeProfit(const Context& context) {
    if (not _position->has())
        return false;

    // интересует выход за TAKE PROFIT
    if (_position->change(context.price(_position->revert())) < _settings.take_profit)
        return false;

    OrderSide trend, signal;
    indicator(context, trend, signal);
    if (trend != _position->revert())
        return false;

    return tryClose(context);
}

bool Algorithm::tryStopLoss(const Context& context) {
    if (not _position->has() || _settings.stop_loss >= 0.0)
        return false;

    // не режем лося, если доступно усреднения
    if (availableAverage() > 0)
        return false;

    // интересует выход за STOP LOSS
    if (_position->change(context.price(_position->revert())) > _settings.stop_loss)
        return false;

    return tryClose(context);
}

bool Algorithm::tryAverage(const Context& context) {
    // посчитаем сколько раз сможем усреднить
    int available = availableAverage();
    if (available == 0)
        return false;

    // посчитаем процент усреднения
    Change average_percent = _settings.averaging;
    for (int i = 0; i < available - 1; ++i)
        average_percent /= 2.0;

    // интересует выход за усреднение
    if (_position->change(context.price(_position->revert())) > average_percent)
        return false;

    // создадим реквест
    OrderRequest request;
    request.symbol = _position->symbol();
    request.side = _position->side();
    request.quantity = _position->baseQuantity();

    // создание заказа
    Position avg;
    if (not createOrder(context, request, avg))
        return false;

    _position->merge(avg);
    if (_settings.isRelease())
        _position->save();

    Terminal::printOrder(avg, ">");
    return true;
}

bool Algorithm::tryClose(const Context& context) {
    // определим размер лота, для закрытия позиции
    // он может быть тот же, или отличаться на размер профита
    Price price = context.price(_position->revert());
    Quantity profit_base = _position->profit(price) / price;
    Quantity quantity_add = OrderUtil::usedQuantity(_position->side(), profit_base, -profit_base) * _settings.profit_ratio;
    Quantity quantity = _position->baseQuantity() + quantity_add;

    // создадим реквест
    OrderRequest request;
    request.symbol = _position->symbol();
    request.side = _position->revert();
    request.quantity = quantity;

    // создадим заказ
    Position close;
    if (not createOrder(context, request, close))
        return false;

    // сохраняем отчет
    Report report(*_position, close);
    _report.add(report);

    // сохраняем статистику закрытия сделки
    _statistics->report(report);
    if (_settings.isRelease())
        _statistics->save();

    // распечатаем созданную позицию с id закрытой
    Terminal::printOrder(close, "<");
    // показываем профит
    Terminal::printProfit(report, _settings.symbol.quoteAsset());

    // удалим из базы, результат удаления не важен
    _position->remove();
    return true;
}

bool Algorithm::tryOpen(const Context& context) {
    if (_position->has())
        return false;

    OrderSide trend, signal;
    indicator(context, trend, signal);

    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.side = signal;
    request.quantity = Exchanger().roundQuantity(0.0, request.symbol) * _settings.lot_size;

    // создание заказа
    Position position;
    if (not createOrder(context, request, position))
        return false;

    _position->copy(position);
    _position->setTime(context.time());

    if (_settings.isRelease())
        _position->save();

    Terminal::printOrder(position, ">");
    return true;
}

bool Algorithm::createOrder(const Context& context, OrderRequest& request, Position& result) const {
    if (request.side == OrderSide::Invalid || not request.isEnough())
        return false;

    if (not _settings.isRelease()) {
        result.setSymbol(request.symbol);
        result.setSide(request.side);
        result.setBaseQuantity(Exchanger().roundQuantity(request.quantity, request.symbol));
        result.setQuoteQuantity(result.baseQuantity() * context.price(request.side));
        result.operate();
        return true;
    }

    // создание заказа
    const OrderWrapper* order = Exchanger().createOrder(request);
    if (order == nullptr)
        return false;

    DB().rpush(_settings.username + ":events", (order->side() == Buy ? "buy " : "sell ") + std::to_string(order->baseQuantity()) + " " + (std::string)order->symbol().baseAsset());
    //Events().send("%s %f %s", order->side() == Buy ? "buy" : "sell", order->baseQuantity(), order->symbol().baseAsset().c_str());

    result.copy(*order);
    return true;
}

void Algorithm::indicator(const Context& context, OrderSide& trend, OrderSide& signal) const {
    DEMA dema = DEMA(20, 30);
    if (not context.load(dema))
        return;

    trend = dema.trend();
    signal = dema.signal();
}

int Algorithm::availableAverage() const {
    if (not _position->has() || _settings.averaging >= 0.0)
        return 0;

    Quantity balance = OrderUtil::usedQuantity(_position->side(), _position->symbol().baseAsset().balance(), _position->symbol().quoteAsset().balance());
    Quantity quantity = OrderUtil::usedQuantity(_position->side(), _position->baseQuantity(), _position->quoteQuantity());
    int result = 0;
    while (quantity <= balance) {
        ++result;
        quantity *= 2.0;
    }

    return result;
}

void Algorithm::report() const {
    // вернем средва по последней не выполненной позиции
    if (_position->has() && _settings.isBackTest()) {
        _position->setSide(_position->revert());
        _position->operate();
    }

    Terminal::printReport(_report, _settings.symbol);
}
