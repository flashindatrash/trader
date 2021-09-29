#include "Algorithm.hpp"

#include <utility>
#include "Time.hpp"
#include "Logger.hpp"
#include "Context.hpp"
#include "Position.hpp"
#include "Statistics.hpp"
#include "Terminal.hpp"
#include "Migrator.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/Exchanger.hpp"

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
    Logger::setLogfile("/tmp/" + _settings.uniqId() + ".log");

    // создадим структуры для хранения данных
    _position = Position::create(_settings.uniqId() + ":position");
    _statistics = Statistics::create(_settings.uniqId() + ":stats");


    // промигрируем данные
    if (not Migrator::migrate(_position, _statistics, _settings.symbol))
        return false;

    return true;
}

void Algorithm::execute(const Context& context) {
    // закрытие сделок: получить профит || усреднение цены || остановить убыток
    bool open = tryTakeProfit(context) || tryAverage(context) || tryStopLoss(context);
    // открытие сделок
    bool close = tryOpen(context);

    // обновляем строку терминала
    Terminal::update(*_position, _settings, context);
}

bool Algorithm::tryTakeProfit(const Context& context) {
    if (not _position->has())
        return false;

    // интересует выход за TAKE PROFIT
    if (_position->distance(context.price(_position->revert())) < _position->price() * _settings.take_profit)
        return false;

    // ждем сигнал на закрытие
    if (_position->revert() != getSignal(context))
        return false;

    return tryClose(context);
}

bool Algorithm::tryStopLoss(const Context& context) {
    if (not _position->has() || _settings.stop_loss >= 0.0)
        return false;

    // интересует выход за STOP LOSS
    if (_position->distance(context.price(_position->revert())) > _position->price() * _settings.stop_loss)
        return false;

    return tryClose(context);
}

bool Algorithm::tryAverage(const Context& context) {
    if (not _position->has() || _settings.averaging >= 0.0)
        return false;

    // интересует выход за усреднение
    if (_position->distance(context.price(_position->revert())) > _position->price() * _settings.averaging)
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
    // создадим реквест
    OrderRequest request;
    request.symbol = _position->symbol();
    request.side = _position->revert();
    request.quantity = _position->baseQuantity();

    // создадим заказ
    Position position;
    if (not createOrder(context, request, position))
        return false;

    Quantity profit = _position->profit(position.price()) - position.fee();

    // распечатаем созданную позицию с id закрытой
    Terminal::printOrder(position, "<");

    // удалим из базы, результат удаления не важен
    _position->remove();

    // сохраняем профит
    auto profits = _statistics->addProfit(profit);
    if (_settings.isRelease())
        _statistics->save();

    // показываем профит
    Terminal::printProfit(profit, profits);
    return true;
}

bool Algorithm::tryOpen(const Context& context) {
    if (_position->has())
        return false;

    static OrderSide previous_signal = OrderSide::Invalid;

    OrderSide side = getSignal(context);
    if (side == OrderSide::Invalid || previous_signal == side)
        return false;

    // интересует только пересечение сигналов
    if (previous_signal == OrderSide::Invalid) {
        previous_signal = side;
        return false;
    }

    previous_signal = side;

    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.side = side;
    request.quantity = Exchanger().minQuantity(request.symbol) * _settings.lot_size;

    // создание заказа
    Position position;
    if (not createOrder(context, request, position))
        return false;

    _position->copy(position);
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
        result.setBaseQuantity(request.quantity);
        result.setQuoteQuantity(request.quantity * context.price(request.side));

        BalanceWrapper* baseBalance = Exchanger().balance(request.symbol.baseAsset());
        BalanceWrapper* quoteBalance = Exchanger().balance(request.symbol.quoteAsset());

        if (baseBalance && quoteBalance) {
            switch (request.side) {
                case OrderSide::Buy: {
                    baseBalance->gain(result.baseQuantity());
                    quoteBalance->spend(result.quoteQuantity());
                    break;
                }
                case OrderSide::Sell: {
                    baseBalance->spend(result.baseQuantity());
                    quoteBalance->gain(result.quoteQuantity());
                    break;
                }
                case OrderSide::Invalid:
                    break;
            }
        }
        return true;
    }

    // создание заказа
    const OrderWrapper* order = Exchanger().createOrder(request);
    if (order == nullptr)
        return false;

    result.copy(*order);
    return true;
}

OrderSide Algorithm::getSignal(const Context& context) const {
    Price ema_long = context.ema(30);
    Price ema_short = context.ema(20);
    if (ema_long == 0.0 || ema_short == 0.0)
        return OrderSide::Invalid;

    return ema_short > ema_long ? OrderSide::Buy : OrderSide::Sell;
}
