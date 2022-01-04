//
// Created by Вадим Проскурин on 04.01.2022.
//

#include "argparser/ArgumentParser.hpp"
#include "Logger.hpp"
#include "app/StakerApp.hpp"

int main(int argc, char** argv) {
    std::string config_file;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));

        auto parsed = args.parse(argc, argv);
        config_file = parsed.getValue("--config");
    } catch (...) {
        args.showHelp("staker -c default.cfg");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(config_file.c_str())) {
        Logger::info(util::format("Can't load config %s", config_file.c_str()));
        return EXIT_FAILURE;
    }

    return StakerApp::create(cfg)->run();
}