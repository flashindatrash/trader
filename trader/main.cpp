#include <argparser/ArgumentParser.hpp>

#include "TraderApp.hpp"

int main(int argc, char** argv) {
    srand(time(NULL));

	std::string cfg_file;
	cppargparser::ArgumentParser args;

    TraderApp::create();
	return EXIT_SUCCESS;
}
