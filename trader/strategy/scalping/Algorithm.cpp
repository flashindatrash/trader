#include "Algorithm.hpp"

#include <utility>
#include "Time.hpp"
#include "Logger.hpp"
#include "Context.hpp"
#include "Positions.hpp"
#include "Statistics.hpp"
#include "Terminal.hpp"
#include "Migrator.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
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
    if (_positions != nullptr) {
        delete _positions;
        _positions = nullptr;
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
    _positions = Positions::create(_settings.uniqId() + ":positions", _settings.isRelease());
    _statistics = Statistics::create(_settings.uniqId() + ":stats", _settings.isRelease());

    // промигрируем данные
    if (not Migrator::migrate(_positions, _statistics, _settings.symbol, _settings.isRelease()))
        return false;

    return true;
}

void Algorithm::execute(const Context& context) {
    bool status = false;
    // закрытие сделок: получить профит || остановить убыток || усреднение цены
    status |= tryTakeProfit(context) || tryStopLoss(context) || tryAverage(context);
    // открытие сделок
    status |= tryOpen(context);
    // обновляем строку терминала
    Terminal::update(*_positions, _settings, context);
}

bool Algorithm::tryTakeProfit(const Context& context) {
    // самая выгодная позиция
    const auto position = _positions->compare_if(Predicates::closable, Compares::profitable(context.price()));
    if (position == _positions->cend())
        return false;

    // интересует выход за TAKE PROFIT
    if (position->distance(context.price(position->revert())) < position->price() * _settings.take_profit)
        return false;

    // ждем сигнал на закрытие
    if (position->revert() != getSignal(context))
        return false;

    return tryClose(context, *position);
}

bool Algorithm::tryStopLoss(const Context& context) {
    if (_settings.stop_loss >= 0.0)
        return false;

    // самая проигранная позиция
    const auto position = _positions->compare_if(Predicates::closable, Compares::losable(context.price()));
    if (position == _positions->cend())
        return false;

    // интересует выход за STOP LOSS
    if (position->distance(context.price(position->revert())) > position->price() * _settings.stop_loss)
        return false;

    return tryClose(context, *position);
}

bool Algorithm::tryAverage(const Context& context) {
    if (_settings.averaging >= 0.0 || _positions->size() == 0)
        return false;

    // самая проигранная позиция
    auto& position = _positions->front();

    // интересует выход за усреднение
    if (position.distance(context.price(position.revert())) > position.price() * _settings.averaging)
        return false;

    // создадим реквест
    OrderRequest request;
    request.symbol = position.symbol();
    request.side = position.side();
    request.quantity = position.baseQuantity();

    // создание заказа
    Position avg;
    if (not createOrder(context, request, avg))
        return false;

    Price t1 = position.price();

    // объединение
    position.setBaseQuantity(position.baseQuantity() + avg.baseQuantity());
    position.setQuoteQuantity(position.quoteQuantity() + avg.quoteQuantity());

    // todo: возможно стоит выпилить массив
    // todo: вторая покупка, по ней не учтется комиссия
    if (_settings.isRelease())
        position.save();

    Price t2 = position.price();

    Terminal::printOrder(avg, ">");
    return true;
}

bool Algorithm::tryClose(const Context& context, const Position& closable) {
    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.side = closable.revert();
    request.quantity = closable.baseQuantity();

    // создадим заказ
    Position position;
    if (not createOrder(context, request, position))
        return false;

    Quantity profit = closable.profit(position.price()) - position.fee();

    // распечатаем созданную позицию с id закрытой
    Terminal::printOrder(position, "<");

    // удалим из базы, результат удаления не важен
    _positions->remove(closable);

    // сохраняем профит, высчитываем и показываем PNL
    auto profits = _statistics->addProfit(profit);
    Terminal::printProfit(profit, profits);
    return true;
}

bool Algorithm::tryOpen(const Context& context) {
    if (_positions->size() > 0)
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

    if (not _positions->push(position))
        return false;

    Terminal::printOrder(position, ">");
    return true;
}

bool Algorithm::createOrder(const Context& context, OrderRequest& request, Position& result) const {
    if (request.side == OrderSide::Invalid || not request.isEnough())
        return false;

    if (not _settings.isRelease()) {
        static int sTestId = 1;

        result = Position("test" + std::to_string(sTestId++));
        result.setBaseQuantity(request.quantity);
        result.setQuoteQuantity(request.quantity * context.price(request.side));
        result.setSide(request.side);
        result.setSymbol(request.symbol);

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

    result = Position(*order);
    return true;
}

OrderSide Algorithm::getSignal(const Context& context) const {
    Price ema_long = context.ema(30);
    Price ema_short = context.ema(20);
    if (ema_long == 0.0 || ema_short == 0.0)
        return OrderSide::Invalid;

    return ema_short > ema_long ? OrderSide::Buy : OrderSide::Sell;
}
