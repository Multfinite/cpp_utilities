#ifndef UTILITIES_BUFFER_HPP
#define UTILITIES_BUFFER_HPP

#include "type_definitions.hpp"

#include <vector>

namespace Utilities
{
	struct _buffer
	{
		std::vector<char> _b;
	
		_buffer() : _b() {}
		_buffer(string& str)	 : _buffer(str.data(), str.size())  {}
		_buffer(void* pData, size_t size)
		{
			_b.resize(size);
			memcpy(_b.data(), pData, _b.size());
		}
		_buffer(stringstream& ss)
		{
			size_t size = ss.tellp();
			_b.resize(size);
			ss.read(_b.data(), _b.size());
		}
		_buffer(size_t size) { _b.resize(size); }

		inline size_t size() const { return _b.size(); }

		inline void* data() { return _b.data(); }
		template<typename T>
		inline T* get()
		{
			return (T*) _b.data();
		}
		template<typename T>
		inline T* get(void* pHead)
		{
			return (T*) pHead;
		}

		template<typename T>
		inline T* offset(size_t offset)
		{
			T*		ptr	= (T*) _b.data();
			ptr			= ptr + offset;
			return ptr;
		}

		template<typename T>
		inline T* push(void* pHead, T item)
		{
			T*			ptr	= (T*) pHead;
			memcpy(ptr, &item, sizeof(T));
			ptr++;
			return ptr;
		}
		inline char* push(void* pHead, string item)
		{
			char*	ptr	= (char*) pHead;
			memcpy(ptr, item.data(), item.size());
			ptr += item.size();
			return ptr;
		}
	};
}

#endif 