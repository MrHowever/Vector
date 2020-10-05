/*
 * VectorBadAllocException.hh
 *
 *  Created on: 21 kwi 2018
 *      Author: mrhowever
 */

#ifndef VECTORBADALLOCEXCEPTION_HH_
#define VECTORBADALLOCEXCEPTION_HH_

#include<exception>
#include<string>

class VectorBadAllocException : public std::exception {
	public:
		std::string specificInfo;

		VectorBadAllocException() : specificInfo("Nierozpoznany blad typu ElementNotFoundException") {}
		VectorBadAllocException(const std::string& info) : specificInfo("ElementNotFoundException:\n"+info) {}
		~VectorBadAllocException() = default;

		virtual const char* what() const throw()
		{
			return specificInfo.c_str();
		}
};

#endif /* VECTORBADALLOCEXCEPTION_HH_ */
