#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

#include "argparser/ArgumentParser.hpp"
#include "Logger.hpp"
#include "app/TraderApp.hpp"

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(int argc, char** argv) {
    signal(SIGSEGV, handler);
    srand(time(NULL));

    std::string type;
    std::string config_file;
    std::string lua_file;
    std::string symbol;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config file", 1, true));
        args.addArgument(cppargparser::Argument("-t", "--type", "type", 1, true));
        args.addArgument(cppargparser::Argument("-l", "--lua", "lua file", 1, false));
        args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 2, false));
        args.addArgument(cppargparser::Argument("-l", "--listing", "listing", 0, false));

        auto parsed = args.parse(argc, argv);

        type = parsed.getValue("--type");
        config_file = parsed.getValue("--config");

        if (parsed.hasArgument("--lua"))
            lua_file = parsed.getValue("--lua");

        if (parsed.hasArgument("--symbol"))
            symbol = parsed.getValues("--symbol").at(0) + parsed.getValues("--symbol").at(1);

    } catch(...) {
        args.showHelp("trader -t pair -c default.cfg -l script.lua -s btc usdt | trader -t listing -c default.cfg");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(config_file.c_str())) {
        Logger::info(util::format("Can't load config %s", config_file.c_str()));
        return EXIT_FAILURE;
    }

    // todo
    cfg.set("SCRIPT", lua_file);
    cfg.set("SYMBOL", symbol);

    if (TraderApp* app = TraderApp::create(type, cfg)) {
        return app->run();
    }

    return EXIT_FAILURE;
}
