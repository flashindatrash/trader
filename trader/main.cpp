#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <argparser/ArgumentParser.hpp>
#include "Logger.hpp"
#include "Config.hpp"
#include "TraderApp.hpp"
#include "exchanger/wrapper/Symbol.hpp"

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

    Symbol symbol;
    std::string cfg_file;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));
        args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 2, true));

        auto parsed = args.parse(argc, argv);

        cfg_file = parsed.getValue("--config");
        std::vector<std::string> assets = parsed.getValues("--symbol");
        symbol = Symbol(assets[0], assets[1]);
    } catch(...) {
        args.showHelp("trader -c ./config/default.cfg -s btc usdt");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(cfg_file.c_str())) {
        trace("Can't init config %s", cfg_file.c_str());
        return EXIT_FAILURE;
    }

    TraderApp::create(cfg)->run(symbol);
	return EXIT_SUCCESS;
}
