/*
 * EmptyVectorException.hh
 *
 *  Created on: 21 kwi 2018
 *      Author: mrhowever
 */

#ifndef EMPTYVECTOREXCEPTION_HH_
#define EMPTYVECTOREXCEPTION_HH_

#include<exception>
#include<string>

class EmptyVectorException : public std::exception {
	public:
		std::string specificInfo;

		EmptyVectorException() : specificInfo("Nierozpoznany blad typu ElementNotFoundException") {}
		EmptyVectorException(const std::string& info) : specificInfo("ElementNotFoundException:\n"+info) {}
		~EmptyVectorException() = default;

		virtual const char* what() const throw()
		{
			return specificInfo.c_str();
		}
};

#endif /* EMPTYVECTOREXCEPTION_HH_ */
