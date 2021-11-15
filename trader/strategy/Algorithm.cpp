#include "Algorithm.hpp"

#include <utility>
#include "Context.hpp"
#include "Position.hpp"
#include "Report.hpp"
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
    delete _position;
    _position = nullptr;
}

bool Algorithm::init() {
    _position = Position::create(_settings.storage("position"));
    return true;
}

void Algorithm::stop() {
    // вернем средва по последней не выполненной позиции
    if (_position->has() && _settings.isBackTest()) {
        _position->setSide(_position->revert());
        _position->operate();
    }

    onStop.emmit(nullptr);
}

const Position& Algorithm::execute(const Context& context) {
    // закрытие сделок: получить профит || усреднение цены || остановить убыток
    bool open = tryTakeProfit(context) || tryAverage(context) || tryStopLoss(context);
    // открытие сделок
    bool close = tryOpen(context);

    return *_position;
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

    onAverage.emmit(avg);
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

    // создадим отчет
    Report report(*_position, close);

    // удалим из базы, результат удаления не важен
    _position->remove();

    onClose.emmit(report);
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

    onOpen.emmit(position);
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