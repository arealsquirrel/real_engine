#ifndef REALLIB_SPAN_HPP
#define REALLIB_SPAN_HPP

#include "real/core/types.hpp"
namespace real {


/* 
 * represents a continugious string of memory 
 *
 * This class has no constructor or anything because it is meant
 * to be returned from other classes
 */
template<typename T>
struct Span {
public:
	const u32 elements 	{0};
	T *buffer 		{nullptr};

public:
	bool valid() { return (elements != 0); }
};

}

#endif
