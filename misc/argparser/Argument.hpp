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

#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <string>
#include <vector>
#include "Validator.hpp"

namespace cppargparser {

class Argument {
public:
    enum Type { SHORT, LONG };

    /**
     * Creates a new instance of Argument.
     *
     * @param shortArg the short argument
     * @param longArg the long argument
     * @param desc the description
     * @param numArgs the number of arguments, for infinity use Argument::INFINITY constant
     * @param mandatory true if a given argument is mandatory; false otherwise
     */
    Argument(const std::string& shortArg, const std::string& longArg,
		const std::string& desc, int numArgs, bool mandatory):
		shortArg(shortArg),
		longArg(longArg),
		description(desc),
		numArgs(numArgs),
		mandatory(mandatory),
		validator(NULL) {
	}

    /**
     * Creates a new instance of Argument.
     *
     * @param shortArg the short argument
     * @param longArg the long argument
     * @param desc the description
     * @param numArgs the number of arguments, for infinity use Argument::INFINITY constant
     * @param mandatory true if a given argument is mandatory; false otherwise
     * @param validator the validator
     */
    Argument(const std::string& shortArg, const std::string& longArg,
        const std::string& desc, int numArgs, bool mandatory,
		Validator* validator):
		shortArg(shortArg),
		longArg(longArg),
		description(desc),
		numArgs(numArgs),
		mandatory(mandatory) {
		this->validator = validator;
	}

    /**
     * Creates a new instance of Argument.
     *
     * @param arg the argument (can be short/long depending on the arg type)
     * @param desc the description (can be short/long depending on the arg type)
     * @param type the argument type
     * @param numArgs the number of arguments, for infinity use Argument::INFINITY constant
     * @param mandatory true if a given argument is mandatory; false otherwise
     */
    Argument(const std::string& arg, const std::string& desc, Type type,
		int numArgs, bool mandatory):
		shortArg(""),
		longArg(""),
		description(desc),
		numArgs(numArgs),
		mandatory(mandatory),
		validator(NULL) {
		switch (type) {
		case SHORT:
			shortArg = arg;
			break;
		case LONG:
			longArg = arg;
			break;
		}
	}

    /**
     * Creates a new instance of Argument.
     *
     * @param arg the argument (can be short/long depending on the arg type)
     * @param desc the description (can be short/long depending on the arg type)
     * @param type the argument type
     * @param numArgs the number of arguments, for infinity use Argument::INFINITY constant
     * @param mandatory true if a given argument is mandatory; false otherwise
     * @param validator the validator
     */
    Argument(const std::string& arg, const std::string& desc, Type type,
		int numArgs, bool mandatory, Validator* validator):
		shortArg(""),
		longArg(""),
		description(desc),
		numArgs(numArgs),
		mandatory(mandatory) {
		switch (type) {
		case SHORT:
			shortArg = arg;
			break;
		case LONG:
			longArg = arg;
			break;
		}
		this->validator = validator;
	}

    /**
     * Gets the short argument.
     * @return the short argument
     */
	inline std::string getShortArg() const {
		return shortArg;
	}

    /**
     * Gets the long argument.
     * @return the long argument
     */
	inline std::string getLongArg() const{
		return longArg;
	}

    /**
     * Checks if the argument is a short argument.
     * @return true if the argument is a short argument; false otherwise
     */
	inline bool isShortArg() const {
		return (shortArg.size() > 0) ? true : false;
	}

    /**
     * Checks if the argument is a long argument.
     * @return true if the argument is a long argument; false otherwise
     */
	bool isLongArg() const {
		return (longArg.size() > 0) ? true : false;
	}

    /**
     * Gets the argument description.
     * @return the argument description
     */
	inline std::string getDescription() const {
		return description;
	}

    /**
     * Gets the number of arguments.
     * @return the number of arguments
     */
	inline int getNumArgs() const {
		return numArgs;
	}

    /**
     * Checks if the argument is a mandatory.
     * @return true if it's mandatory; false otherwise
     */
	inline bool isMandatory() const {
		return mandatory;
	}

    /**
     * Gets the validator.
     * @return the validator
     */
	inline Validator* getValidator() {
		return validator;
	}

	virtual ~Argument() { }

private:
    std::string shortArg;
    std::string longArg;
    std::string description;
    int numArgs;
    bool mandatory;
    Validator* validator;
};

} /* namespace cppargparser */
#endif /* ARGUMENT_H_ */
