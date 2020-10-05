#ifndef VECTOR_HH_
#define VECTOR_HH_

#include<iostream>
#include<cstdio>
#include<algorithm>
#include<utility>
#include<math.h>
#include<initializer_list>
#include "riterator.hh"
#include "allocator.hh"
#include<iterator>
#include<type_traits>
#include "VectorBadInputException.hh"
#include "VectorBadAllocException.hh"
#include "VectorOutOfBoundsException.hh"
#include "EmptyVectorException.hh"


/*
 * Posprawdzac czy w kazdym miejscu powinien byc move czy copy
 * Czy wszedzie jest exception guarantee
 * komentarze
 * noexcept
 * sprawdzic czy spelniaja wszystkie wymagania
 * Range Assign - can constructor throw on valid memory?
 * zabezpieczenia przed wyjsciem poza _size
 * getallocator zrobic na noexcept
 * sprawdzic czy sprawdzanie typu w assign (ojciec) dziala
 */



namespace mlab
{
	template<class T, class Allocator = allocator<T> >
	class vector : private Allocator
	{
		public:
		typedef T value_type;
		typedef typename Allocator::reference reference;
		typedef typename Allocator::const_reference const_reference;
		typedef typename Allocator::size_type size_type;
		typedef typename Allocator::pointer pointer;
		typedef typename Allocator::pointer iterator;
		typedef typename Allocator::difference_type difference_type;
		typedef riterator<const T> const_reverse_iterator;
		typedef typename Allocator::const_pointer const_iterator;
		typedef riterator<T> reverse_iterator;
		typedef Allocator allocator_type;

		vector(const vector<value_type,allocator_type>&, const allocator_type& = Allocator());			//Copy constructor
		vector(const size_type = 0, const_reference = T(), const allocator_type& = Allocator());		//Size constructor
		vector(vector<T,Allocator>&&);						//Move constructor
		~vector();									//Destructor
		vector<T,Allocator>& operator=(vector);				//Copy assignment operator
		vector<T,Allocator>& operator=(vector&&);					//Move assignment operator
		vector<T,Allocator>& operator=(std::initializer_list<value_type>);

		//Methods giving information about size of vector
		size_type capacity() const noexcept;
		size_type size() const noexcept;
		bool empty() const noexcept;

		//Iterators
		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		reverse_iterator rbegin() noexcept;
		const_reverse_iterator rbegin() const noexcept;
		const_iterator cbegin() const noexcept;
		const_reverse_iterator crbegin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		reverse_iterator rend() noexcept;
		const_reverse_iterator rend() const noexcept;
		const_iterator cend() const noexcept;
		const_reverse_iterator crend() const noexcept;

		//References to elements
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		iterator data() noexcept;
		const_iterator data() const noexcept;
		reference at(size_type);
		const_reference at(size_type) const;

		//Adding/removing elements
		iterator erase(const_iterator);
		iterator erase(const_iterator, const_iterator);
		iterator insert(const_iterator, const_reference);
		iterator insert(const_iterator, size_type, const_reference);
		iterator insert(const_iterator, value_type&&);
		iterator insert(const_iterator, std::initializer_list<value_type>);
		template<typename inputIterator>
		iterator insert(const_iterator, inputIterator, inputIterator);
		void push_back(const_reference);
		void push_back(value_type&&);
		void pop_back();
		void clear() noexcept;

		template<typename input_iterator>
		void assign(input_iterator,input_iterator);
		void assign(size_type, const_reference);
		void assign(std::initializer_list<value_type>);

		template<class... Args>
		iterator emplace(const_iterator, Args&&...);
		template<class... Args>
		void emplace_back(Args&&... );

		//Memory allocation
		void reserve(const size_type);
		void resize(size_type);
		void resize(const size_type, const_reference);
		void shrink_to_fit();

		reference operator[](size_type);
		const_reference operator[](size_type) const;

		template<typename U>
		friend void swap(vector<U>&, vector<U>&);
		void swap(vector& vec) { swap(*this, vec); }

		allocator_type getAllocator() const { return static_cast<const allocator_type&> (*this); }

		private:
		size_type _size;
		size_type _capacity;
		iterator _array;

		const double _growthRatio = 1.5;				//Growth ratio of vectors' capacity. Can be changed to 2 for quicker insertion of big data sets
													//Ratio has to be > 1 to achieve amortized constant time of push_back method
		template<typename RandomAccessIterator>
		void assign(RandomAccessIterator, RandomAccessIterator, std::random_access_iterator_tag);
		template<typename ForwardIterator>
		void assign(ForwardIterator, ForwardIterator, std::forward_iterator_tag);
		template<typename InputIterator>
		void assign(InputIterator, InputIterator, std::input_iterator_tag);
	};

template<class T, class Allocator>
vector<T, Allocator>::vector(const vector<T, Allocator>& vec, const allocator_type& alloc) : Allocator(alloc), _size(vec._size), _capacity(vec._capacity)
{
	try
	{
		_array = Allocator::allocate(vec._capacity);
	}
	catch(...)
	{
		throw VectorBadAllocException();
	}

	for(size_type i=0; i < _size; i++)
		_array[i] = vec[i];
}

template<class T, class Allocator>
vector<T,Allocator>::vector(const size_type s, const_reference initValue, const allocator_type& alloc) : Allocator(alloc), _size(s), _capacity(ceil(s*_growthRatio)), _array(nullptr)
{
	if(s != 0)
	{
		_array = Allocator::allocate(s);
		size_type i;

		try
		{
		for(i = 0; i < s; i++)
			Allocator::construct(_array+i, initValue);
		}
		catch(...)
		{
			if(!std::is_trivially_destructible<T>::value)
			{
				for(size_type j = 0; j < i; j++)
					Allocator::destroy(_array+j);
			}

			Allocator::deallocate(_array, s);
			throw;
		}
	}
}

template<typename T, typename Allocator>
vector<T,Allocator>::~vector()
{
	if(_array != nullptr)
	{
		if(!std::is_trivially_destructible<T>::value)
		{
			for(iterator i = begin(); i < end(); i++)
				Allocator::destroy(i);
		}

		Allocator::deallocate(_array, _size);
	}
}

template<typename U>
void swap(vector<U>& vec1, vector<U>& vec2)
{
	std::swap(vec1._size, vec2._size);
	std::swap(vec1._capacity, vec2._capacity);
	std::swap(vec1._array, vec2._array);
}

/*
 * Copy assignment operator
 * The parameter is a copy thus we cant alter the actual state of parameter. This method uses swap() to swap the values from parameter to current
 * object. Parameter recieves the data from current object however as it is only a temporary copy, it dies at the end of function.
 */
template<typename T, typename Allocator>
vector<T,Allocator>& vector<T,Allocator>::operator=(vector vec)
{
	swap(*this, vec);
	return *this;
}

/* Move constructor
 * Current element is being initialized with default constructor. Then we swap current element and parameter (vec). After the procedure is done
 * our current element contains all the values from vec and vec contains default values that were in current element after being initialized with
 * default constructor. Move constructor has to set the parameter to default after moving so this function works as expected.
 */
template<class T, class Allocator>
vector<T,Allocator>::vector(vector&& vec) : vector<T,Allocator>()
{
	swap(*this, vec);
}



/*
 * Move assignment operator works in the same way as move constructor but returns the moved object afterwards.
 */
template<typename T, class Allocator>
vector<T,Allocator>& vector<T,Allocator>::operator=(vector<T,Allocator>&& vec)
{
	*this = vector();
	swap(*this, vec);
	return *this;
}

template<typename T, typename Allocator>
vector<T,Allocator>& vector<T,Allocator>::operator=(std::initializer_list<T> il)
{
	if(il.size() > _capacity)
	{
		pointer p = nullptr;

		try
		{
			p = Allocator::allocate(_capacity*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		_capacity *= _growthRatio;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(iterator it = begin(); it < end(); it++)
				Allocator::destroy(it);
		}

		Allocator::deallocate(_array);

		size_type idx = 0;
		for(value_type i : il)
			p[idx++] = i;

		_array = p;
	}
	else
	{
		iterator temp = _array;
		for(value_type i : il)
			*temp++ =  i;									//Assign new elements to container

		if(!std::is_trivially_destructible<value_type>::value)
			for(; temp != end(); temp++)								//Destroy remaining elements
				Allocator::destroy(temp);

	}

	_size = il.size();

	return *this;
}


template<typename T, class Allocator>
typename vector<T,Allocator>::size_type vector<T,Allocator>::capacity() const noexcept
{
	return _capacity;
}

template<typename T, class Allocator>
typename vector<T,Allocator>::size_type vector<T,Allocator>::size() const noexcept
{
	return _size;
}

template<typename T, class Allocator>
bool vector<T,Allocator>::empty() const noexcept
{
	return !_size;
}

template<typename T, class Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::begin() noexcept
{
	return _array;
}

template<typename T, class Allocator>
typename vector<T,Allocator>::const_iterator vector<T,Allocator>::begin() const noexcept
{
	return _array;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reverse_iterator vector<T,Allocator>::rbegin() const noexcept
{
	return _array;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::reverse_iterator vector<T,Allocator>::rbegin() noexcept
{
	return _array;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_iterator vector<T,Allocator>::cbegin() const noexcept
{
	return _array;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reverse_iterator vector<T,Allocator>::crbegin() const noexcept
{
	return _array;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::end() noexcept
{
	return _array+_size;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_iterator vector<T,Allocator>::end() const noexcept
{
	return _array+_size;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::reverse_iterator vector<T,Allocator>::rend() noexcept
{
	return _array+_size;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reverse_iterator vector<T,Allocator>::rend() const noexcept
{
	return _array+_size;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_iterator vector<T,Allocator>::cend() const noexcept
{
	return _array+_size;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reverse_iterator vector<T,Allocator>::crend() const noexcept
{
	return _array+_size;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::reference vector<T, Allocator>::front()
{
	if(_array)
		return *(_array);
	else
		throw VectorOutOfBoundsException();
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reference vector<T, Allocator>::front() const
{
	if(_array)
		return *(_array);
	else
		throw VectorOutOfBoundsException();
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::reference vector<T,Allocator>::back()
{
	if(_size)
		return _array[_size-1];
	else
		throw VectorOutOfBoundsException();
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reference vector<T,Allocator>::back() const
{
	if(_size)
		return _array[_size-1];
	else
		throw VectorOutOfBoundsException();
}

template<typename T, typename Allocator>
void vector<T,Allocator>::reserve(size_type newCap)
{
	if(newCap > _capacity)
	{
		pointer newBuffer = nullptr;
		try
		{
			newBuffer = Allocator::allocate(ceil(newCap));
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		for(size_type i=0; i < _size; i++)
			newBuffer[i] = _array[i];

		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(iterator it = begin(); it != end(); it++)
				Allocator::destroy(it);
		}

		Allocator::deallocate(_array);

		_array = newBuffer;
		_capacity = ceil(newCap);
	}
}

template<typename T, typename Allocator>
void vector<T,Allocator>::push_back(const_reference item)
{
	if(_size == _capacity)
		reserve((_capacity+1)*_growthRatio);				//+1 because multiplying by zero if we push_back on empty vector lol

	_array[_size++] = item;
}

template<typename T, typename Allocator>
void vector<T,Allocator>::push_back(value_type&& item)
{
	if(_size == _capacity)
		reserve((_capacity+1)*_growthRatio);

	_array[_size++] = item;
}

template<typename T, typename Allocator>
void vector<T,Allocator>::pop_back()
{
	if(_size)
	{
		_size--;

		if(!std::is_trivially_destructible<value_type>::value)
			Allocator::destroy(end());
	}
	else
		throw EmptyVectorException();
}
template<typename T, typename Allocator>
void vector<T,Allocator>::clear() noexcept
{
	if(!std::is_trivially_destructible<value_type>::value)			//Trivially destructible objects need not to be destroyed
	{
		for(iterator it = begin(); it != end(); it++)
			Allocator::destroy(it);
	}

	_size = 0;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::reference vector<T,Allocator>::operator[](size_type idx)
{
	return _array[idx];
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reference vector<T,Allocator>::operator[](size_type idx) const
{
	return _array[idx];
}

template<typename T, typename Allocator>
void vector<T,Allocator>::resize(size_type newSize)
{
	if(newSize < _size)
	{
		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(iterator it = begin() + newSize; it != end(); it++)
				Allocator::destroy(it);
		}
	}
	else if(newSize > _size)
		{
			if(newSize > _capacity)
				reserve(ceil(newSize*_growthRatio));

			for(iterator it = begin() + _size; it != (begin() + newSize); it++)
				Allocator::construct(it);
		}

	_size = newSize;
}

template<typename T, typename Allocator>
void vector<T,Allocator>::resize(size_type newSize, const_reference initValue)
{
	if(newSize < _size)
	{
		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(iterator it = begin() + newSize; it != end(); it++)
				Allocator::destroy(it);
		}

	}
	else if(newSize > _size)
		{
			if(newSize > _capacity)
				reserve(ceil(newSize*_growthRatio));

			for(iterator it = begin() + _size; it != (begin() + newSize); it++)
				Allocator::construct(it, initValue);
		}

	_size = newSize;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::data() noexcept
{
	return _array;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_iterator vector<T,Allocator>::data() const noexcept
{
	return _array;
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::reference vector<T,Allocator>::at(size_type idx)
{
	if(idx < _size)
		return _array[idx];
	else
		throw VectorOutOfBoundsException();
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::const_reference vector<T,Allocator>::at(size_type idx) const
{
	if(idx < _size)
		return _array[idx];
	else
		throw VectorOutOfBoundsException();
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::erase(const_iterator pos)
{
	if(pos > end())
		throw VectorOutOfBoundsException();

	if(pos != end())
	{
		for(iterator it = pos; it < end() - 1; it++)					//Element to be deleted is swapped towards the end of the vector
			swap(*it, *(it+1));											//kinda like in bubblesort

		if(!std::is_trivially_destructible<value_type>::value)			//Now the element to be removed is the last element
			destroy(end());

		_size--;

		return pos;
	}
	else
	{
		if(!std::is_trivially_destructible<value_type>::value)			//Now the element to be removed is the last element
			destroy(end());

		_size--;

		return end();
	}
}

template<typename T, typename Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::erase(const_iterator first, const_iterator last)
{
	difference_type diff = last-first;

	if(first > end() && last > end() && first > last)
		throw VectorBadInputException();

	iterator temp = first;
	for(iterator it = last; it != end(); it++)
		*temp = std::move(*it);

	if(!std::is_trivially_destructible<value_type>::value)
	{
		reverse_iterator it = end() - 1;
		for(difference_type elemsToRemove = diff; elemsToRemove != 0; elemsToRemove--, it++)
			Allocator::destroy(it);
	}

	if( diff > (end() - last) )
	{
		_size -= diff;
		return end();
	}
	else
	{
		_size -= diff;
		return last;
	}
}

/*
 * Function assigning new elements to a vector. Old elements are destroyed.
 * If amount of new elements exceeds current vector capacity the vector will grow and the memory will be reallocated
 * This function has basic exception guarantee.
 */
template<typename T, typename Allocator> template<typename input_iterator>
void vector<T,Allocator>::assign(input_iterator inputBegin,input_iterator inputEnd)
{
	if(std::is_convertible<typename std::iterator_traits<input_iterator>::value_type, value_type>::value)	//Check if input_iterator points to an object
		assign(inputBegin, inputEnd, std::iterator_traits<input_iterator>::iterator_category());	//that T can be constructed from
	else
		throw VectorBadInputException();
}

template<typename T, typename Allocator> template<typename RandomAccessIterator>
void vector<T,Allocator>::assign(RandomAccessIterator inputBegin, RandomAccessIterator inputEnd, std::random_access_iterator_tag)
{
	pointer p;												//Helper pointer used to hold new allocated memory if needed

	difference_type newSize = inputEnd - inputBegin;			//Amount of elements to be added

	if(newSize < 0)
		throw VectorBadInputException();

	if( newSize > _capacity)									//If the amount of elements to be added exceeds current capacity, expand the vector
	{
		try
		{
			p = Allocator::allocate( newSize*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}
	}

	if(!std::is_trivially_destructible<value_type>::value)
	{
		for(iterator it = begin(); it != end(); it++)		//Destroy all old elements
			Allocator::destroy(it);
	}

	if(newSize > _capacity)													//If the vector has to be reallocated, free old memory
	{
		Allocator::deallocate(_array);
		_array = p;															//and assign new memory to vector
		_capacity = newSize*_growthRatio;
	}

	_size = newSize;															//Update size

	iterator vecItr = begin();							//Construct new elements
	for(iterator it = inputBegin; it != inputEnd; it++, vecItr++)
		Allocator::construct(vecItr,*it);						//Can object constructor throw exception if memory is valid?
																//It can but placement new wont throw
																//What if the reference is invalid?
}


template<typename T, typename Allocator> template<typename ForwardIterator>
void vector<T,Allocator>::assign(ForwardIterator inputBegin, ForwardIterator inputEnd, std::forward_iterator_tag)
{
	pointer p;												//Helper pointer used to hold new allocated memory if needed

	difference_type newCap = 1;
	for(ForwardIterator temp = inputBegin; temp != inputEnd; newCap++, temp++) {}

	if(newCap < 0)
		throw VectorBadInputException();

	if( newCap > _capacity)									//If the amount of elements to be added exceeds current capacity, expand the vector
	{
		try
		{
			p = Allocator::allocate( newCap*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}
	}

	if(!std::is_trivially_destructible<value_type>::value)
	{
		for(iterator it = begin(); it != end(); it++)		//Destroy all old elements
			Allocator::destroy(it);
	}

	if(newCap > _capacity)													//If the vector has to be reallocated, free old memory
	{
		Allocator::deallocate(_array);
		_array = p;															//and assign new memory to vector
		_capacity = newCap*_growthRatio;
	}

	_size = newCap;															//Update size

	iterator vecItr = begin();							//Construct new elements
	for(ForwardIterator it = inputBegin; it != inputEnd; it++, vecItr++)
		Allocator::construct(vecItr,*it);						//Can object constructor throw exception if memory is valid?
																//It can but placement new wont throw
																//What if the reference is invalid?
}

template<typename T, typename Allocator> template<typename InputIterator>
void vector<T,Allocator>::assign(InputIterator inputBegin, InputIterator inputEnd, std::input_iterator_tag)
{

	iterator vecItr = begin();
	size_type elemsAssigned = 0;
	for(InputIterator it = inputBegin; it != inputEnd - 1; it++, vecItr++)
	{
		if(elemsAssigned == _capacity)
		{
			reserve(_capacity*_growthRatio);
			vecItr = begin() + elemsAssigned;
		}

		if(vecItr != end())
			Allocator::destroy(vecItr);

		Allocator::construct(vecItr,*it);						//Can object constructor throw exception if memory is valid?
	}
																//It can but placement new wont throw
																//What if the reference is invalid?
}


template<typename T, typename Allocator>
void vector<T,Allocator>::assign(size_type inputSize, const_reference defaultValue)
{
	pointer p;

	if(inputSize > _capacity)
	{
		try
		{
			p = Allocator::allocate(inputSize*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}
	}

	if(!std::is_trivially_destructible<value_type>::value)
	{
		for(iterator it = begin(); it != end(); it++)
			Allocator::destroy(it);
	}

	if(inputSize > _capacity)
	{
		Allocator::deallocate(_array);
		_array = p;
	}

	iterator it = begin();
	for(size_type i = 0; i < inputSize; i++, it++)
		Allocator::construct(it,defaultValue);

	_size = inputSize;
}

template<typename T, typename Allocator>
void vector<T,Allocator>::assign(std::initializer_list<T> il)
{
	pointer p;

	if(il.size() > _capacity)
	{
		try
		{
			p = Allocator::allocate(il.size() * _growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}
	}

	if(!std::is_trivially_destructible<value_type>::value)
	{
		for(iterator it = begin(); it != end(); it++)
			Allocator::destroy(it);
	}

	if(il.size() > _capacity)
	{
		Allocator::deallocate(_array);
		_array = p;
	}

	iterator it = begin();
	for(value_type i : _array)
	{
		Allocator::construct(it,i);
		it++;
	}

	_size = il.size();
}


template<typename T, class Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::insert(const_iterator position, const_reference ref)
{
	pointer p;

	if(_size == _capacity)
	{
		try
		{
			p = Allocator::allocate(_capacity*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		_capacity *= _growthRatio;

		iterator tempPtr = p;
		iterator it = begin();
		for(; it < position; it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			it = begin();
			for(; it < position; it++)
				Allocator::destroy(it);
		}

		iterator it2 = it;

		*tempPtr++ = ref;

		for(; it < end(); it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<T>::value)
		{
			for(; it2 < end(); it2++)
			Allocator::destroy(it2);
		}

		Allocator::deallocate(_array);
		_array = p;
	}
	else
	{
		value_type temp = ref;

		for(vector<T,Allocator>::iterator itr = position; itr <= end(); itr++)
		{
			swap(*itr, temp);
		}
	}

	_size++;

	return position;
}

template<typename T, class Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::insert(const_iterator pos, size_type insertSize, const_reference defaultValue)
{
	pointer p;

	if(_size+insertSize > _capacity)
	{
		try
		{
			p = Allocator::allocate( (_size+insertSize)*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		_capacity = (_size+insertSize)*_growthRatio;

		iterator tempPtr = p;
		vector<T,Allocator>::iterator it = begin();
		for(; it < pos; it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			it = begin();
			for(; it < pos; it++)
				Allocator::destroy(it);
		}

		it = pos;
		for(size_type i = 0; i <= insertSize; i++)
			*tempPtr++ = defaultValue;

		iterator it2 = it;

		for(; it < end(); it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(; it2 < end(); it2++)
				Allocator::destroy(it2);
		}

		Allocator::deallocate(_array);
		_array = p;
		}
	else
	{
		iterator temp = pos;				//Move elements to make room for insertSize new elements
		for(vector<T,Allocator>::iterator itr = (pos + insertSize); itr <= end(); itr++, temp++)
			swap(*itr, *temp);

		temp = pos;											//Insert new elements into the gap
		for(size_type i = 0; i <= insertSize; i++, temp++)
			_array[i] = defaultValue;
	}

	_size += insertSize;

	return pos+insertSize;
}

template<typename T, class Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::insert(const_iterator position, value_type&& moveElem)
{
	pointer p;

	if(_size == _capacity)
	{
		try
		{
			p = Allocator::allocate(_capacity*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		_capacity *= _growthRatio;

		iterator tempPtr = p;
		vector<T,Allocator>::iterator it = begin();
		for(; it < position; it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<T>::value)
		{
			it = begin();
			for(; it < position; it++, tempPtr++)
				Allocator::destroy(it);
		}

		iterator it2 = it;

		*tempPtr++ = moveElem;

		for(; it < end(); it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(; it2 < end(); it2++)
				Allocator::destroy(it2);
		}

		Allocator::deallocate(_array);
		_array = p;
	}
	else
	{
		value_type temp = moveElem;

		for(vector<T,Allocator>::iterator itr = position; itr <= end(); itr++)
		{
			swap(*itr, temp);
		}
	}

	_size++;

	return position;
}


template<typename T, class Allocator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::insert(const_iterator position, std::initializer_list<value_type> il)
{
	pointer p;

	if(_size + il.size() > _capacity)
	{
		try
		{
			p = Allocator::allocate((_size+il.size())*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		_capacity = (_size+il.size())*_growthRatio;

		iterator tempPtr = p;
		vector<T,Allocator>::iterator it = begin();
		for(; it < position; it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			it = begin();
			for(; it < position; it++, tempPtr++)
				Allocator::destroy(it);
		}

		iterator it2 = it;

		for(value_type i : il)
			*tempPtr++ = i;

		for(; it < end(); it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(; it2 < position; it2++)
				Allocator::destroy(it2);
		}

		Allocator::deallocate(_array);
		_array = p;
	}
	else
	{
		iterator temp = position + il.size();
		for(vector<T,Allocator>::iterator itr = position; itr <= end(); itr++, temp++)
			swap(*temp, *itr);

		temp = position;
		for(value_type i : il)
		{
			swap(*temp, i);
			temp++;
		}
	}

	_size += il.size();

	return position + il.size();
}

template<typename T, class Allocator> template<typename inputIterator>
typename vector<T,Allocator>::iterator vector<T,Allocator>::insert(const_iterator position, inputIterator first, inputIterator last)
{
	pointer p;
	difference_type diff = last-first;
	size_type newSize = _size + diff;

	if( newSize > _capacity)
	{
		try
		{
			p = Allocator::allocate( newSize*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		_capacity = newSize*_growthRatio;

		iterator tempPtr = p;
		vector<T,Allocator>::iterator it = begin();
		for(; it < position; it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			it = begin();
			for(; it < position; it++, tempPtr++)
				Allocator::destroy(it);
		}

		iterator it2 = it;

		it = position;
		for(inputIterator inputItr = first; inputItr < last; inputItr++)
			*tempPtr++ = *inputItr;


		for(; it < end(); it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<T>::value)
		{
			for(; it2 < end(); it2++)
				Allocator::destroy(it2);
		}

		Allocator::deallocate(_array);
		_array = p;
		}
	else
	{
		vector<T,Allocator>::iterator temp = position;				//Move elements to make room for insertSize new elements
		for(vector<T,Allocator>::iterator itr = (position + diff); itr <= end(); itr++, temp++)
			swap(*itr, *temp);

		temp = position;											//Insert new elements into the gap
		for(inputIterator inputItr = first; inputItr < last; inputItr++)
			*temp++ = *inputItr;
	}

	_size = newSize;

	return position+diff;
}

template<typename T, typename Allocator>
void vector<T,Allocator>::shrink_to_fit()
{
	pointer p = nullptr;

	try
	{
		p = Allocator::allocate(_size);
	}
	catch(...)
	{
		throw VectorBadAllocException();
	}

	iterator pItr = p;
	for(iterator it = begin(); it < end(); it++, pItr++)
		*pItr = std::move(*it);

	if(!std::is_trivially_destructible<value_type>::value)
		for(iterator it = begin(); it < end(); it++, pItr++)
			Allocator::destroy(it);

	Allocator::deallocate(_array);
	_array = p;
	_capacity = _size;
}

template<typename T, typename Allocator> template<class... Args>
typename vector<T,Allocator>::iterator vector<T,Allocator>::emplace(const_iterator position, Args&&... args)
{
	pointer p;

	if(_size == _capacity)
	{
		try
		{
			p = Allocator::allocate(_capacity*_growthRatio);
		}
		catch(...)
		{
			throw VectorBadAllocException();
		}

		_capacity *= _growthRatio;

		iterator tempPtr = p;
		iterator it = begin();
		for(; it != position; it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			it = begin();
			for(; it < position; it++)
				Allocator::destroy(it);
		}

		iterator it2 = it;

		Allocator::construct(tempPtr,args...);
		tempPtr++;

		for(; it != end(); it++, tempPtr++)
			*tempPtr = *it;

		if(!std::is_trivially_destructible<value_type>::value)
		{
			for(; it2 < end(); it2++)
				Allocator::destroy(it2);
		}

		Allocator::deallocate(_array);
		_array = p;
		}
	else
	{
		value_type temp = *position;

		if(!std::is_trivially_destructible<value_type>::value)
			Allocator::destroy(position);

		Allocator::construct(position,args...);

		for(iterator itr = position+1; itr != end() + 1; itr++)		//Nie szybciej by bylo od konca move o jedna pozycje? Sprawdzic
			swap(*itr, temp);
	}

	_size++;

	return position;
}

template<typename T, typename Allocator> template<class... Args>
void vector<T,Allocator>::emplace_back(Args&&... args)
{
	if(_size == _capacity)
		reserve(_capacity*_growthRatio);

	Allocator::construct(end(),args...);

	_size++;
}

template<typename T, typename Allocator>
bool operator==(const vector<T,Allocator>& vec1, const vector<T,Allocator>& vec2)
{
	if(vec1._size == vec2._size)
	{
		for(typename vector<T,Allocator>::size_type idx = 0; idx < vec1._size; idx++)
			if(vec1[idx] != vec2[idx])
				return false;

		return true;
	}

	return false;
}

template<typename T, typename Allocator>
bool operator<(const vector<T,Allocator>& vec1, const vector<T,Allocator>& vec2)
{
	if(vec1._size < vec2._size)
		return true;
	else if(vec1._size > vec2._size)
		return false;

	for(typename vector<T,Allocator>::size_type idx = 0; idx < vec1._size; idx++)
		if(vec1[idx] > vec2[idx])
			return false;

	return true;
}

template<typename T, typename Allocator>
bool operator!=(const vector<T,Allocator>& vec1, const vector<T,Allocator>& vec2)
{
	return !(vec1 == vec2);
}

template<typename T, typename Allocator>
bool operator>(const vector<T,Allocator>& vec1, const vector<T,Allocator>& vec2)
{
	return vec2 < vec1;
}

template<typename T, typename Allocator>
bool operator<=(const vector<T,Allocator>& vec1, const vector<T,Allocator>& vec2)
{
	return !(vec2 < vec1);
}

template<typename T, typename Allocator>
bool operator>=(const vector<T,Allocator>& vec1, const vector<T,Allocator>& vec2)
{
	return !(vec1 < vec2);
}

template<typename T>
std::ostream& operator<<(std::ostream& Strm, mlab::vector<T> vec)
{
	std::cout<<"wut"<<vec.size()<<std::endl;
	for(typename mlab::vector<T>::iterator it = vec.begin(); it != vec.end(); it++)
		Strm<<*it<<", ";

	return Strm;
}

}
#endif /* VECTOR_HH_ */
