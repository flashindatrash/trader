#include "argparser/ArgumentParser.hpp"
#include "Logger.hpp"
#include "app/EventManagerApp.hpp"

int main(int argc, char** argv) {
    std::string cfg_file;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));

        auto parsed = args.parse(argc, argv);
        cfg_file = parsed.getValue("--config");
    } catch(...) {
        args.showHelp("event_manager -c ./config/default.cfg");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(cfg_file.c_str())) {
        Logger::info(util::format("Can't init config %s", cfg_file.c_str()));
        return EXIT_FAILURE;
    }

    return EventManagerApp::create(cfg)->run();
}
