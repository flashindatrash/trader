#include "Algorithm.hpp"

#include "Position.hpp"
#include "Context.hpp"
#include "Report.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/indicator/DEMA.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    auto* algorithm = new Algorithm(settings);
    return algorithm;
}

Algorithm::Algorithm(const Settings& settings)
    : _settings(settings)
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

void Algorithm::start() {
    onStart.emmit(nullptr);
}

void Algorithm::stop() {
    // вернем средства по последней не выполненной позиции
    if (_position->has() && _settings.isBackTest()) {
        _position->setSide(_position->revert());
        _position->operate();
    }

    onStop.emmit(nullptr);
}

bool Algorithm::execute() {
    // закрытие сделок: получить профит || усреднение цены || остановить убыток
    bool close = tryTakeProfit() || tryAverage() || tryStopLoss();
    // открытие сделок
    bool open = tryOpen();
    // уведомление о выполнении тика
    onTick.emmit(*_position);
    // статус позции либо открыли, либо закрыли
    return close || open;
}

const Position& Algorithm::position() const {
    return *_position;
}

bool Algorithm::tryTakeProfit() {
    if (not _position->has() || Context::current == nullptr)
        return false;

    // интересует выход за TAKE PROFIT
    if (_position->change(Context::current->price(_position->revert())) < _settings.take_profit)
        return false;

    OrderSide trend, signal;
    indicator(trend, signal);
    if (trend != _position->revert())
        return false;

    return tryClose();
}

bool Algorithm::tryStopLoss() {
    if (not _position->has() || Context::current == nullptr || _settings.stop_loss >= 0.0)
        return false;

    // не режем лося, если доступно усреднения
    if (_settings.averaging < 0.0 && _position->averages() > 0)
        return false;

    // интересует выход за STOP LOSS
    if (_position->change(Context::current->price(_position->revert())) > _settings.stop_loss)
        return false;

    return tryClose();
}

bool Algorithm::tryAverage() {
    if (not _position->has() || Context::current == nullptr || _settings.averaging >= 0.0)
        return false;

    // доступно ли хотябы одно усреднение
    if (_position->averages() < 1)
        return false;

    // интересует выход за усреднение
    if (_position->change(Context::current->price(_position->revert())) > _position->averagePercent(_settings.averaging))
        return false;

    // создадим реквест
    OrderRequest request;
    request.symbol = _position->symbol();
    request.side = _position->side();
    request.quantity = _position->baseQuantity();

    // создание заказа
    Position avg;
    if (not createOrder(request, avg))
        return false;

    _position->merge(avg);
    if (_settings.isRelease())
        _position->save();

    onAverage.emmit(avg);
    return true;
}

bool Algorithm::tryClose() {
    if (Context::current == nullptr)
        return false;

    // определим размер лота, для закрытия позиции
    // он может быть тот же, или отличаться на размер профита
    Price price = Context::current->price(_position->revert());
    Quantity profit_base = _position->profit(price) / price * _settings.profit_ratio;

    Quantity additional = 0.0;
    double (*round)(double) = std::round;
    if (_position->side() == Buy) {
        // если это лонг, то продаем чуть меньше с округлением вверх
        additional = -profit_base;
        round = std::ceil;
    } else if (_position->side() == Sell) {
        // если это шорт, то покупаем чуть больше с округлением вниз
        additional = profit_base;
        round = std::floor;
    }

    // создадим реквест
    OrderRequest request;
    request.symbol = _position->symbol();
    request.side = _position->revert();
    request.quantity = Exchanger().roundQuantity(_position->baseQuantity() + additional, _position->symbol(), round);

    // создадим заказ
    Position close;
    if (not createOrder(request, close))
        return false;

    // создадим отчет
    Report report(*_position, close);

    // удалим из базы, результат удаления не важен
    _position->remove();

    onClose.emmit(close);
    onReport.emmit(report);
    return true;
}

bool Algorithm::tryOpen() {
    if (_position->has() || Context::current == nullptr)
        return false;

    OrderSide trend, signal;
    indicator(trend, signal);

    // проверяем на фильтр открываемых позиций
    if (_settings.open_filter != -1 && (_settings.open_filter == OrderSide::Invalid || _settings.open_filter != (int)signal))
        return false;

    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.side = signal;
    request.quantity = Exchanger().roundQuantity(0.0, request.symbol) * _settings.lot_size;

    // создание заказа
    Position position;
    if (not createOrder(request, position))
        return false;

    _position->copy(position);
    _position->setTime(Context::current->time());

    if (_settings.isRelease())
        _position->save();

    onOpen.emmit(position);
    return true;
}

bool Algorithm::createOrder(OrderRequest& request, Position& result) const {
    if (Context::current == nullptr)
        return false;

    if (request.side == OrderSide::Invalid || not request.isEnough())
        return false;

    if (not _settings.isRelease()) {
        result.setSymbol(request.symbol);
        result.setSide(request.side);
        result.setBaseQuantity(Exchanger().roundQuantity(request.quantity, request.symbol));
        result.setQuoteQuantity(result.baseQuantity() * Context::current->price(request.side));
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

void Algorithm::indicator(OrderSide& trend, OrderSide& signal) const {
    trend = signal = Invalid;

    if (Context::current == nullptr)
        return;

    DEMA dema = DEMA(20, 30);
    if (not Context::current->load(dema))
        return;

    trend = dema.trend();
    signal = dema.signal();
}