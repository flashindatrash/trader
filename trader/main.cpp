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

    std::string config_file;
    std::string script_file;
    std::string symbol;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));
        args.addArgument(cppargparser::Argument("-s", "--script", "script", 1, true));
        args.addArgument(cppargparser::Argument("-p", "--pair", "pair", 2, true));

        auto parsed = args.parse(argc, argv);
        config_file = parsed.getValue("--config");
        script_file = parsed.getValue("--script");
        symbol = parsed.getValues("--pair").at(0) + parsed.getValues("--pair").at(1);
    } catch(...) {
        args.showHelp("trader -c default.cfg -s script.lua -p btc usdt");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(config_file.c_str())) {
        Logger::info(util::format("Can't load config %s", config_file.c_str()));
        return EXIT_FAILURE;
    }

    cfg.set("SCRIPT", script_file);
    cfg.set("SYMBOL", symbol);

    return TraderApp::create(cfg)->run();
}
