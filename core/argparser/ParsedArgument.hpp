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

#ifndef PARSEDARGUMENT_H_
#define PARSEDARGUMENT_H_

#include <string>
#include <vector>
#include <map>
#include "InvalidArgumentException.hpp"

namespace cppargparser {

class ParsedArgument {
public:
    /**
     * Puts the argument and argument value.
     * @param arg the argument
     * @param value the argument value
     */
	void putArgument(const std::string& arg, const std::string& value) {
		if (arg.size() == 0) {
			return;
		}
		std::map<std::string, std::vector<std::string> >::iterator i = args.find(arg);
		if (i == args.end()) {
			std::vector<std::string> v;
			v.push_back(value);
			args[arg] = v;
		} else {
			i->second.push_back(value);
		}
	}

    /**
     * Gets the argument value.
     * @param arg the argument
     * @return the argument value
     */
	std::string getValue(const std::string& arg) const {
		std::map<std::string, std::vector<std::string> >::const_iterator i = args.find(arg);
		if (i == args.end()) {
			throw InvalidArgumentException(arg + " is an invalid argument");
		}
		// always return the first index
		return i->second[0];
	}

    /**
     * Gets the argument values.
     * @param arg the argument
     * @return the argument values
     */
	std::vector<std::string> getValues(const std::string& arg) const {
		std::map<std::string, std::vector<std::string> >::const_iterator i = args.find(arg);
		if (i == args.end()) {
			throw InvalidArgumentException(arg + " is an invalid argument");
		}

		return i->second;
	}

    /**
     * Checks if the given arg was called.
     * @return true if a given arg was called; false otherwise
     */
	bool hasArgument(const std::string& arg) const {
		std::map<std::string, std::vector<std::string> >::const_iterator i = args.find(arg);
		return (i == args.end()) ? false : true;
	}

	virtual ~ParsedArgument() { }

private:
    std::map<std::string, std::vector<std::string> > args;
};

} /* namespace cppargparser */
#endif /* PARSEDARGUMENT_H_ */
