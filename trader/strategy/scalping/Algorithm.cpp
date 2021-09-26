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
    _positions = Positions::create(_settings.uniqId() + ":positions", not _settings.test);
    _statistics = Statistics::create(_settings.uniqId() + ":stats", not _settings.test);

    // промигрируем данные
    if (not Migrator::migrate(_positions, _statistics, _settings.symbol, _settings.test))
        return false;

    return true;
}

void Algorithm::execute(const Context& context) {
    bool status = tryTakeProfit(context) || tryStopLoss(context) || tryOpen(context);
    Terminal::update(*_positions, _settings, context);
}

bool Algorithm::tryTakeProfit(const Context& context) {
    // самая выгодная позиция
    const auto position = _positions->compare_if(Predicates::closable, Compares::profitable);
    if (position == _positions->cend())
        return false;

    // интересует выход за TAKE PROFIT
    if (position->distance() < position->price() * _settings.take_profit)
        return false;

    return tryClose(*position);
}

bool Algorithm::tryStopLoss(const Context& context) {
    // самая проигранная позиция
    const auto position = _positions->compare_if(Predicates::closable, Compares::losable);
    if (position == _positions->cend())
        return false;

    // интересует выход за STOP LOSS
    if (position->distance() > position->price() * _settings.stop_loss)
        return false;

    return tryClose(*position);
}

bool Algorithm::tryClose(const Position& closable) {
    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.side = closable.revert();
    request.quantity = closable.baseQuantity();

    // создадим заказ
    Position position;
    if (not createOrder(request, position))
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

    // выбираем куда идем в шорт или лонг
    std::pair<OrderSide, double> analyze = risk();
    if (analyze.first == OrderSide::Invalid)
        return false;

    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.side = analyze.first;
    request.quantity = Exchanger().minQuantity(request.symbol);

    // чем меньше риск, тем выше ставка
    double lot_k = (_settings.risk - analyze.second) / _settings.risk;
    double lot_diff = _settings.lot_max - _settings.lot_min;
    request.quantity *= _settings.lot_min + lot_diff * lot_k;

    // создание заказа
    Position position;
    if (not createOrder(request, position))
        return false;

    if (not _positions->push(position))
        return false;

    Terminal::printOrder(position, ">");
    return true;
}

bool Algorithm::createOrder(OrderRequest& request, Position& result) const {
    if (not request.isEnough())
        return false;

    if (_settings.test) {
        static int sTestId = 1;

        PriceWrapper* price = Exchanger().price(request.symbol.id());
        if (price == nullptr)
            return false;

        result = Position("test" + std::to_string(++sTestId));
        result.setBaseQuantity(request.quantity);
        result.setQuoteQuantity(request.quantity * price->get(request.side));
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

std::pair<OrderSide, double> Algorithm::risk() const {
    double buy = risk(OrderSide::Buy);
    double sell = risk(OrderSide::Sell);

    if (buy > _settings.risk && sell > _settings.risk)
        return std::make_pair(OrderSide::Invalid, std::numeric_limits<double>::infinity());

    if (sell < buy)
        return std::make_pair(OrderSide::Sell, sell);
    else
        return std::make_pair(OrderSide::Buy, buy);
}

double Algorithm::risk(OrderSide side) const {
    const ChartWrapper* chart = Exchanger().chart(_settings.symbol);
    if (chart == nullptr)
        return std::numeric_limits<double>::infinity();

    const PriceWrapper* price = Exchanger().price(_settings.symbol);
    if (price == nullptr)
        return std::numeric_limits<double>::infinity();

    Price current = price->get(side);
    Price change;

    switch (side) {
        case OrderSide::Buy: change = current + current * _settings.take_profit; break;
        case OrderSide::Sell: change = current - current * _settings.take_profit; break;
        case OrderSide::Invalid: return std::numeric_limits<double>::infinity();
    }

    const ChartWrapper::Range range = chart->last(current, change);
    if (not range.isValid())
        return std::numeric_limits<double>::infinity();

    time_t now = Time().ms();
    time_t diffs = (now - range.end->timeClose()) + (now - range.begin->timeClose());

    // эталонное время 1 час
    return (double)diffs / (double)Timer::sHour;
}