#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

#include "argparser/ArgumentParser.hpp"
#include "Logger.hpp"
#include "app/TraderApp.hpp"
#include "util/StringUtil.hpp"

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

    std::string cfg_file;
    std::vector<std::string> symbol;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));
        args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 2, false));

        auto parsed = args.parse(argc, argv);
        cfg_file = parsed.getValue("--config");
        if (parsed.hasArgument("--symbol"))
            symbol = parsed.getValues("--symbol");
    } catch(...) {
        args.showHelp("trader -c ./config/default.cfg -s btc usdt");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(cfg_file.c_str())) {
        Logger::info(util::format("Can't init config %s", cfg_file.c_str()));
        return EXIT_FAILURE;
    }

    if (symbol.size() >= 2)
        cfg.set("SYMBOL", symbol.at(0) + symbol.at(1));

    return TraderApp::create(cfg)->run();
}
