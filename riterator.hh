#ifndef RITERAOR_HH_
#define RITERAOR_HH_
#include<algorithm>
#include<iterator>

namespace mlab
{
template<class T>
class riterator;
}

namespace std
{
template<class T>
struct iterator_traits< mlab::riterator<T> >
{
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef T*& pointer_reference;
	typedef const T*& const_pointer_reference;
	typedef std::bidirectional_iterator_tag iterator_category;
};
}

namespace mlab
{
	template<typename T>
	class riterator
	{
			typedef std::ptrdiff_t difference_type;
			typedef T value_type;
			typedef T* pointer;
			typedef T& reference;
			typedef T*& pointer_reference;
			typedef const T*& const_pointer_reference;

			std::iterator_traits< riterator<T> > S;
			pointer _ptr;

			riterator();
			riterator(const riterator<value_type>&);
			riterator(riterator<value_type>&&);
			riterator<value_type>& operator=(const riterator<value_type>&);
			riterator<value_type>& operator=(riterator<value_type>&&);
			~riterator() = default;

			riterator& operator++();
			riterator& operator++(int);
			riterator& operator--();
			riterator& operator--(int);
			int operator-(const riterator&) const;
			riterator& operator+(const int) const;
			riterator& operator-(const int);
			riterator& operator+=(const int);
			riterator& operator-=(const int);
			value_type& operator*() const;

			bool operator>(const_pointer_reference) const;
			bool operator>=(const_pointer_reference) const;
			bool operator<(const_pointer_reference) const;
			bool operator<=(const_pointer_reference) const;
			bool operator==(const_pointer_reference) const;
			bool operator!=(const_pointer_reference) const;

			bool operator>(const riterator<value_type>&) const;
			bool operator>=(const riterator<value_type>&) const;
			bool operator<(const riterator<value_type>&) const;
			bool operator<=(const riterator<value_type>&) const;
			bool operator==(const riterator<value_type>&) const;
			bool operator!=(const riterator<value_type>&) const;

			template<typename U>
			friend void swap(riterator<U>&, riterator<U>&);
	};


template<typename U>
void swap(riterator<U>& ri1, riterator<U>& ri2)
{
	std::swap(ri1._ptr, ri2._ptr);
}

template<typename value_type>
mlab::riterator<value_type>::riterator() : _ptr(nullptr) {};

template<typename value_type>
mlab::riterator<value_type>::riterator(const mlab::riterator<value_type>& it)
{
	swap(*this, it);
}

template<typename value_type>
mlab::riterator<value_type>::riterator(riterator<value_type>&& it) : riterator<value_type>()
{
	swap(*this, it);
}

template<typename value_type>
riterator<value_type>& riterator<value_type>::operator=(const riterator<value_type>& it)
{
	swap(*this, it);
	return *this;
}

template<typename value_type>
mlab::riterator<value_type>& riterator<value_type>::operator=(mlab::riterator<value_type>&& it)
{
	*this = mlab::riterator<value_type>();
	swap(*this, it);
	return *this;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator++()
{
	_ptr--;
	return *this;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator++(int)
{
	mlab::riterator<value_type> temp = *this;
	_ptr--;
	return temp;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator--()
{
	_ptr++;
	return *this;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator--(int)
{
	mlab::riterator<value_type> temp = *this;
	_ptr++;
	return temp;
}

template<typename value_type>
int mlab::riterator<value_type>::operator-(const riterator& it) const
{
	return	_ptr - it._ptr;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator+(const int idx) const
{
	mlab::riterator<value_type> temp;
	temp._ptr = _ptr + idx;
	return temp;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator-(const int idx)
{
	mlab::riterator<value_type> temp;
	temp._ptr = _ptr - idx;
	return temp;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator+=(const int idx)
{
	*this = *this + idx;
}

template<typename value_type>
mlab::riterator<value_type>& mlab::riterator<value_type>::operator-=(const int idx)
{
	*this = *this - idx;
}

template<typename value_type>
value_type& mlab::riterator<value_type>::operator*() const
{
	return *_ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator>(const_pointer_reference ptr) const
{
	return _ptr > ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator>=(const_pointer_reference ptr) const
{
	return _ptr >= ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator<(const_pointer_reference ptr) const
{
	return _ptr < ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator<=(const_pointer_reference ptr) const
{
	return _ptr <= ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator==(const_pointer_reference ptr) const
{
	return _ptr == ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator!=(const_pointer_reference ptr) const
{
	return _ptr != ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator>(const mlab::riterator<value_type>& it) const
{
	return _ptr > it._ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator>=(const mlab::riterator<value_type>& it) const
{
	return _ptr >= it._ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator<(const mlab::riterator<value_type>& it) const
{
	return _ptr < it._ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator<=(const mlab::riterator<value_type>& it) const
{
	return _ptr <= it._ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator==(const mlab::riterator<value_type>& it) const
{
	return _ptr == it._ptr;
}

template<typename value_type>
bool mlab::riterator<value_type>::operator!=(const mlab::riterator<value_type>& it) const
{
	return _ptr != it._ptr;
}
}
#endif /* RIvalue_typeERAvalue_typeOR_HH_ */
