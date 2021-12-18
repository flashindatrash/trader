#include "Algorithm.hpp"

#include "Position.hpp"
#include "Script.hpp"
#include "Context.hpp"
#include "Report.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/Exchanger.hpp"

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
    _script = Script::create(_settings.script);
    return _position != nullptr && _script != nullptr && _script->main(_settings);
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
    if (Context::current == nullptr)
        return false;

    bool close = tryClose() || tryAverage();
    bool open = tryOpen();

    onTick.emmit(*_position);
    return close || open;
}

const Position& Algorithm::position() const {
    return *_position;
}

bool Algorithm::tryClose() {
    if (not _position->has() || not _script->close(*_position))
        return false;

    return close();
}

bool Algorithm::tryAverage() {
    if (not _position->has() || not _script->average(*_position))
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

bool Algorithm::close() {
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
    if (_position->has())
        return false;

    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.quantity = Exchanger().roundQuantity(0.0, request.symbol);

    if (not _script->open(request))
        return false;

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