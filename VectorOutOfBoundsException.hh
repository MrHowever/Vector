/*
 * VectorOutOfBoundsException.hh
 *
 *  Created on: 21 kwi 2018
 *      Author: mrhowever
 */

#ifndef VECTOROUTOFBOUNDSEXCEPTION_HH_
#define VECTOROUTOFBOUNDSEXCEPTION_HH_

#include<exception>
#include<string>
#include "VectorBadInputException.hh"

class VectorOutOfBoundsException : public VectorBadInputException {
	public:
		std::string specificInfo;

		VectorOutOfBoundsException() : specificInfo("Nierozpoznany blad typu ElementNotFoundException") {}
		VectorOutOfBoundsException(const std::string& info) : specificInfo("ElementNotFoundException:\n"+info) {}
		~VectorOutOfBoundsException() = default;

		virtual const char* what() const throw()
		{
			return specificInfo.c_str();
		}
};

#endif /* VECTOROUTOFBOUNDSEXCEPTION_HH_ */
