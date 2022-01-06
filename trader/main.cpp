#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

#include "argparser/ArgumentParser.hpp"
#include "Logger.hpp"
#include "app/TraderApp.hpp"
#include "app/ListingApp.hpp"

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

    std::string config_file;
    std::string script_file;
    std::string symbol;
    bool listing = false;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));
        args.addArgument(cppargparser::Argument("-s", "--script", "script", 1, false));
        args.addArgument(cppargparser::Argument("-p", "--pair", "pair", 2, false));
        args.addArgument(cppargparser::Argument("-l", "--listing", "listing", 0, false));

        auto parsed = args.parse(argc, argv);
        config_file = parsed.getValue("--config");

        if (parsed.hasArgument("--script"))
            script_file = parsed.getValue("--script");

        if (parsed.hasArgument("--pair"))
            symbol = parsed.getValues("--pair").at(0) + parsed.getValues("--pair").at(1);

        listing = parsed.hasArgument("--listing");

    } catch(...) {
        args.showHelp("trader -c default.cfg -s script.lua -p btc usdt | trader -c default.cfg --listing");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(config_file.c_str())) {
        Logger::info(util::format("Can't load config %s", config_file.c_str()));
        return EXIT_FAILURE;
    }

    if (listing) {
        return ListingApp::create(cfg)->run();
    }

    cfg.set("SCRIPT", script_file);
    cfg.set("SYMBOL", symbol);
    return TraderApp::create(cfg)->run();
}
