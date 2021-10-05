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
    if (_position->change(context.price(_position->revert())) < _settings.take_profit)
        return false;

    EMACross indicator = ema(context);
    if (indicator.signal() != _position->revert())
        return false;

    return tryClose(context);
}

bool Algorithm::tryStopLoss(const Context& context) {
    if (not _position->has() || _settings.stop_loss >= 0.0)
        return false;

    // интересует выход за STOP LOSS
    if (_position->change(context.price(_position->revert())) > _settings.stop_loss)
        return false;

    return tryClose(context);
}

bool Algorithm::tryAverage(const Context& context) {
    if (not _position->has() || _settings.averaging >= 0.0)
        return false;

    // интересует выход за усреднение
    if (_position->change(context.price(_position->revert())) > _settings.averaging)
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
    // определим количество средств для закрытия
    // для того чтобы уметь зарабатывать в quote и base монете
    const Price& price = context.price(_position->revert());
    Quantity quote_profit = _position->profit(price);
    Quantity base_profit = quote_profit / price;
    Quantity quantity = _position->baseQuantity() - base_profit * _settings.profit_ratio;

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

    EMACross indicator = ema(context);
    if (indicator.empty() || not indicator.crossed())
        return false;

    // создадим реквест
    OrderRequest request;
    request.symbol = _settings.symbol;
    request.side = indicator.signal();
    request.quantity = Exchanger().minQuantity(request.symbol) * _settings.lot_size;

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

EMACross Algorithm::ema(const Context& context) const {
    return context.ema(30, 20);
}

void Algorithm::report() const {
    Terminal::printReport(_report, _settings.symbol);
}
