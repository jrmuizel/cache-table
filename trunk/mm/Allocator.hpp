/*
 *
 *  $Id: cache_table.hpp 5 2006-08-30 20:34:38Z matteo.merli $
 *
 *  $URL: https://cache-table.googlecode.com/svn/trunk/mm/cache_table.hpp $
 *
 *  Copyright (C) 2006 Matteo Merli <matteo.merli@gmail.com>
 *
 *
 *  BSD License
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *   o Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   o Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the
 *     distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _MM_ALLOCATOR_HPP_
#define _MM_ALLOCATOR_HPP_

#include <cstdlib>
#include <new>
#include <bits/functexcept.h>

#if _GLIBCXX_HAVE_MMAP == 1 || _GLIBCPP_HAVE_MMAP == 1 
#  include <sys/mman.h>
#  define MM_HAVE_MMAP
#
#  ifdef __APPLE__
#    define MAP_ANONYMOUS MAP_ANON
#  endif
#else
#  warning "MMap not found."
#endif

namespace mm
{

#ifdef MM_HAVE_MMAP
    
template <typename T>
class MMapAllocator
{
public:
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef T*         pointer;
    typedef const T*   const_pointer;
    typedef T&         reference;
    typedef const T&   const_reference;
    typedef T          value_type;
        
    template<typename T1>
    struct rebind
    { typedef MMapAllocator<T1> other; };
        
    MMapAllocator() throw() {}
    
    MMapAllocator( const MMapAllocator& ) throw() {}
        
    template<typename T1>
    MMapAllocator( const MMapAllocator<T1>& ) throw() {}
        
    ~MMapAllocator() throw() {}
        
    pointer address( reference x ) const { return &x; }
        
    const_pointer address( const_reference x ) const { return &x; }
        
    // NB: size is permitted to be 0.  The C++ standard says nothing
    // about what the return value is when n == 0.
    pointer allocate( size_type size, const void* = 0 )
    {
        if ( __builtin_expect( size > this->max_size(), false ) )
            std::__throw_bad_alloc();
        
        pointer ret = static_cast<T*>( mmap( 0, size,
                                             PROT_READ | PROT_WRITE,
                                             MAP_SHARED | MAP_ANONYMOUS,
                                             0, 0 )
                                       );
        if ( ! ret )
            std::__throw_bad_alloc();
        return ret;
    }
        
    // p is not permitted to be a null pointer.
    void deallocate( pointer p, size_type size )
    {
        munmap( p, size );
    }
    
    size_type max_size() const throw() 
    { return size_t(-1) / sizeof(T); }
    
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 402. wrong new expression in [some_] allocator::construct
    void construct( pointer p, const T& val ) 
    { 
        ::new( p ) value_type( val ); 
    }
        
    void destroy( pointer p ) 
    { 
        p->~T(); 
    }
};
    
template<typename T>
inline bool operator==( const MMapAllocator<T>&, 
                        const MMapAllocator<T>& )
{ return true; }
    
template<typename T>
inline bool operator!=( const MMapAllocator<T>&, 
                        const MMapAllocator<T>& )
{ return false; }

template<typename T>
class TableAllocator  : public MMapAllocator<T>
{};

#else // ! MM_HAVE_MMAP

template<typename T>
class TableAllocator  : public std::allocator<T>
{};

#endif // MM_HAVE_MMAP

} // namespace mm

#endif // _MM_ALLOCATOR_HPP_
