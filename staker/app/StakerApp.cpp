//
// Created by Вадим Проскурин on 04.01.2022.
//

#include "StakerApp.hpp"
#include "Logger.hpp"
#include "binacpp.h"
#include "binacpp_logger.h"

StakerApp* StakerApp::create(const core::Config& config) {
    auto* app = new StakerApp(config);
    return app;
}

StakerApp::StakerApp(const core::Config& config)
    : core::App(config, core::Version(1, 0, 0))
{
}

int StakerApp::run() {
    Logger::info(util::format("StakerBot %s", _version.toString().c_str()));

    // init binance logger
    BinaCPP_logger::set_debug_level(0);
    BinaCPP_logger::enable_logfile(0);

    // init binance config
    static string api_key       = _config.asString("BINANCE_API_KEY");
    static string secret_key    = _config.asString("BINANCE_SECRET_KEY");
    if (api_key.empty() || secret_key.empty())
        return EXIT_FAILURE;

    // init binance api
    if (not BinaCPP::init(api_key, secret_key))
        return EXIT_FAILURE;

    Json::Value response;
    BinaCPP::get_fixedProjects("", "CUSTOMIZED_FIXED", "ALL", "true", "START_TIME", 1, 10, _config.asInt("BINANCE_RECV_WINDOW"), response);

    if (not response.isArray())
        return EXIT_FAILURE;

    for (const auto& project : response) {
        const char* project_id = project["projectId"].asCString();
        const char* project_name = project["projectName"].asCString();
        const char* asset = project["asset"].asCString();
        const char* status = project["status"].asCString();
        int duration = project["duration"].asInt();
        double interest_rate = atof(project["interestRate"].asString().c_str());
        double interest_per_lot = atof(project["interestPerLot"].asString().c_str());
        double lot_size = atof(project["lotSize"].asString().c_str());
        double lots_low_limit = atof(project["lotsLowLimit"].asString().c_str());
        double lots_up_limit = atof(project["lotsUpLimit"].asString().c_str());
        double max_lots_per_user = atof(project["maxLotsPerUser"].asString().c_str());
        bool need_kys = project["needKyc"].asBool();
        Logger::info(util::format("project: %s\n\tstatus: %s\n\tduration: %d\n\tinterest_rate: %f\n\tinterest_per_lot: %f\n", project_name, status, duration, interest_rate, interest_per_lot));
    }

    return EXIT_SUCCESS;
}