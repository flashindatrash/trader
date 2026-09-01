#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

#include "base/Settings.hpp"
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

    Settings settings;
    if (not settings.parse(argc, argv))
        return EXIT_FAILURE;

    if (TraderApp* app = TraderApp::create(settings)) {
        return app->run();
    }

    return EXIT_FAILURE;
}
