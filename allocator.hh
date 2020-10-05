#ifndef ALLOCATOR_HH_
#define ALLOCATOR_HH_

#include<cstddef>					//size_t, ptrdiff_t
#include<cstdlib>					//malloc()
#include<new>						//bad_alloc()

namespace mlab
{
	template<typename T>
	class allocator
	{
		public:
			typedef T value_type;
			typedef value_type* pointer;
			typedef const value_type* const_pointer;
			typedef value_type& reference;
			typedef const value_type& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

			allocator() {};
			allocator(const allocator&) {}
			~allocator() {};

			template<typename U>							//Rebinding
			allocator(const allocator<U>&) {}

			template<typename U>
			struct rebind { typedef allocator<U> other; };

			pointer address(const reference x) const { return &x; }
			const_pointer address(const_reference x) const { return &x; }

			pointer allocate(const size_type, const_pointer = nullptr);
			void deallocate(pointer, const size_type = 0);
			template<class... Args>
			void construct(pointer, Args&&...);
			void destroy(pointer);
			size_type max_size() const { return static_cast<size_type>(-1) / sizeof(value_type); }

		private:											//Move constructor, move assignment and copy assignment disabled
			allocator(allocator&&) {};
			void operator=(allocator&&);
			void operator=(const allocator&);
	};


template<>
class allocator<void>
{

	typedef void value_type;
	typedef void* pointer_type;
	typedef const void* const_pointer;

	template<typename U>
	struct rebind{ typedef allocator<U> other; };
};


template<typename T>
typename allocator<T>::pointer allocator<T>::allocate(const size_type size, const_pointer ptr)
{
	void* mem = std::malloc(size*sizeof(T));
	if(!mem)
		throw std::bad_alloc();

	return static_cast<allocator<T>::pointer> (mem);
}

template<typename T>
void allocator<T>::deallocate(pointer ptr, const size_type)
{
	std::free(ptr);
}

template<typename T> template<class... Args>
void allocator<T>::construct(pointer ptr, Args&&... args)
{
	new(ptr) value_type(args...);
}

template<typename T>
void allocator<T>::destroy(pointer ptr)
{
	ptr->~value_type();
}

template<typename T>
inline bool operator==(const allocator<T>&, const allocator<T>&)			//Allocators are always equal
{
	return true;
}

template<typename T>
inline bool operator!=(const allocator<T>&, const allocator<T>&)
{
	return false;
}
}
#endif /* ALLOCATOR_HH_ */
