// Copyright 2012, Fredy Wijaya
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the Lesser GNU General Public License as published by
// the Free Software Foundation, either version 3.0 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Lesser GNU General Public License for more details.
//
// You should have received a copy of the Lesser GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>

#ifndef ARGUMENTPARSER_H_
#define ARGUMENTPARSER_H_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "Argument.hpp"
#include "ParsedArgument.hpp"
#include "ArgumentParserUtils.hpp"

namespace cppargparser {

class ArgumentParser {
public:
    /**
     * Adds an argument.
     * @param arg the argument
     */
	void addArgument(const Argument& arg) {
		vargs.push_back(arg);
		if (arg.getShortArg().size() > 0) {
			args.insert(std::pair<std::string, Argument>(arg.getShortArg(), arg));
		}

		if (arg.getLongArg().size() > 0) {
			args.insert(std::pair<std::string, Argument>(arg.getLongArg(), arg));
		}
	}

    /**
     * Parses the arguments.
     * @param argc the number of argument, the number of argument should
     *             include the program name, in other words just pass
     *             whatever you get from the main(argc, argv)
     *             if the number of arguments isn't correct, expect a segmentation fault :)
     * @param argv the arguments
     * @return the parsed argument
     */
	ParsedArgument parse(int argc, char** argv) {
		// put all the arguments into vector for easy manipulation
		std::vector<std::string> v;
		for (int i = 1; i < argc; i++) {
			v.push_back(std::string(argv[i]));
		}
		ParsedArgument pa;
		// ignore the first argument since the first argument is a program name
		for (size_t i = 0; i < v.size(); ++i) {
			std::string s = v[i];
			bool shortArg = cppargparser::isShortArg(s);
			bool longArg = cppargparser::isLongArg(s);
			if (!shortArg && !longArg) {
				throw InvalidArgumentException(s + " is an invalid argument");
			}
			std::string arg = s;
			if (longArg) {
				// if the argument has =, e.g. ---ccc=123 split it by = and then
				// modify the vector by modifying --ccc=123 with --ccc and adding
				// a new element 123
				size_t index = arg.find("=");
				if (index != std::string::npos) {
					std::string key = arg.substr(0, index);
					std::string value = arg.substr(index+1);
					arg = key;
					v[i] = key;
					v.insert(v.begin()+i+1, value);
				}
			}
			std::map<std::string, Argument>::iterator it = args.find(arg);
			if (it == args.end()) {
				throw InvalidArgumentException(arg + " is an invalid argument");
			}
			Argument argument = it->second;
            if (argument.getNumArgs() == -1) {
				std::string value = "";
				do {
					value = v[++i];
					if (!cppargparser::isShortArg(value)) {
						pa.putArgument(argument.getShortArg(), value);
					}
					if (!cppargparser::isLongArg(value)) {
						pa.putArgument(argument.getLongArg(), value);
					}
				} while (!isShortArg(value) && !isLongArg(value));
			} else {
				i = i + 1;
				size_t n = i + argument.getNumArgs();
				// this condition means there's the argument doesn't need any value,
				// i.e. the numArgs is 0, thus there's no need to iterate each
				// argument value
				if (i == n) {
					pa.putArgument(argument.getShortArg(), "");
					pa.putArgument(argument.getLongArg(), "");
				}
				else {
					for (; i < n; ++i) {
						if (i >= v.size()) {
							throw InvalidArgumentException(
								argument.getDescription() + " requires " +
								cppargparser::toString(argument.getNumArgs()) +
								" argument(s)");
						}
						std::string value = v[i];
						pa.putArgument(argument.getShortArg(), value);
						pa.putArgument(argument.getLongArg(), value);
					}
				}
			}
			Validator* validator = argument.getValidator();
			if (validator != NULL) {
				if (argument.isShortArg()) {
					std::vector<std::string> values = pa.getValues(argument.getShortArg());
					if (!validator->validate(values)) {
						throw InvalidArgumentException(cppargparser::toString(values) +
							" is an invalid argument value");
					}
				} else {
					std::vector<std::string> values = pa.getValues(argument.getLongArg());
					if (!validator->validate(values)) {
						throw InvalidArgumentException(cppargparser::toString(values) +
							" is an invalid argument value");
					}
				}
			}
			args.erase(argument.getShortArg());
			args.erase(argument.getLongArg());
			// need to decrement i here because both inner and outer loops
			// increment i by 1
			--i;
		}
		// check if the there are still mandatory arguments in the args map
		// if there are, throw an InvalidArgumentException
		for (std::map<std::string, Argument>::const_iterator i = args.begin(); i != args.end(); ++i) {
			if (i->second.isMandatory()) {
				throw InvalidArgumentException(i->second.getDescription() +
					" is a mandatory argument");
			}
		}

		return pa;
	}

    /**
     * Shows/prints a default help menu. Override this show your custom help.
     * @param programName the program name
     */
	virtual void showHelp(const std::string& programName) const {
		std::cout << "Usage: " << programName << std::endl;
		std::cout << "Options:" << std::endl;
		for (std::vector<Argument>::const_iterator i = vargs.begin(); i != vargs.end(); ++i) {
			std::string options = "";
			if (i->isShortArg()) {
				options += i->getShortArg();
				if (i->isLongArg()) {
					options += ", ";
				}
			}
			if (i->isLongArg()) {
				options += i->getLongArg();
			}
			std::cout << std::string(4, ' ') << options << std::string(40-options.size(), ' ') <<
				i->getDescription() << std::endl;
		}
	}

	virtual ~ArgumentParser() { }

protected:
    std::map<std::string, Argument> args;
    std::vector<Argument> vargs;
};

} /* namespace cppargparser */
#endif /* ARGUMENTPARSER_H_ */
