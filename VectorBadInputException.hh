#ifndef VECTORBADINPUTEXCEPTION_HH_
#define VECTORBADINPUTEXCEPTION_HH_

#include<exception>
#include<string>

class VectorBadInputException : public std::exception {
	public:
		std::string specificInfo;

		VectorBadInputException() : specificInfo("Nierozpoznany blad typu ElementNotFoundException") {}
		VectorBadInputException(const std::string& info) : specificInfo("ElementNotFoundException:\n"+info) {}
		~VectorBadInputException() = default;

		virtual const char* what() const throw()
		{
			return specificInfo.c_str();
		}
};

#endif /* VECTORBADINPUTEXCEPTION_HH_ */
