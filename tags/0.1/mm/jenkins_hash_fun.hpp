/*
 *
 *  $Id: hash_fun.hpp 3 2006-08-30 17:22:09Z matteo.merli $
 *
 *  $URL: https://cache-table.googlecode.com/svn/trunk/mm/hash_fun.hpp $
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

#ifndef _MM_JENKINS_HASH_FUN_HPP__
#define _MM_JENKINS_HASH_FUN_HPP__

#include <cstddef>
#include <cstring>
#include <string>
#include "detail/jenkins_hash.h"     

namespace mm
{

using std::size_t;

template <class T> inline void jenkins_hash_combine( size_t& seed, const T& v );


///////////////////////////////////////////////////////////////////////
// Scalar integers
///////////////////////////////////////////////////////////////////////

inline size_t jenkins_hash_value( int8_t n )
{ return static_cast<size_t>( n ); }

inline size_t jenkins_hash_value( uint8_t n, size_t seed = 0 )
{ return static_cast<size_t>( n ); }

inline size_t jenkins_hash_value( int16_t n, size_t seed = 0 )
{ return static_cast<size_t>( n ); }

inline size_t jenkins_hash_value( uint16_t n, size_t seed = 0 )
{ return static_cast<size_t>( n ); }

inline size_t jenkins_hash_value( int32_t n, size_t seed = 0 )
{ return static_cast<size_t>( n ); }

inline size_t jenkins_hash_value( uint32_t n, size_t seed = 0 )
{ return static_cast<size_t>( n ); }

inline size_t jenkins_hash_value( int64_t n, size_t seed = 0 )
{ return static_cast<size_t>( n ); }

inline size_t jenkins_hash_value( uint64_t n, size_t seed = 0 )
{ return static_cast<size_t>( n ); }

///////////////////////////////////////////////////////////////////////
// Sequences
///////////////////////////////////////////////////////////////////////

template <class Iterator>
inline size_t jenkins_hash_range( Iterator first, Iterator last )
{
    size_t h = 0;
    for ( ; first != last; ++first )
        jenkins_hash_combine( h, *first );

    return h;
}

template <class Iterator>
inline void jenkins_hash_range( size_t& seed, Iterator first, Iterator last )
{
    for ( ; first != last; ++first )
        jenkins_hash_combine( seed, *first );
}

inline size_t jenkins_hash_value( char* s ) 
{
    return hashlittle( reinterpret_cast<uint8_t*>( s ),
                       std::strlen(s), 0 );
}

inline size_t jenkins_hash_value( const char* s ) 
{
    return hashlittle( reinterpret_cast<const uint8_t*>( s ),
                       std::strlen(s), 0 );
}

inline size_t jenkins_hash_value( const std::string& s )
{
    return jenkins_hash_value( s.c_str() );
}

template <class T1, class T2>
inline size_t jenkins_hash_value( const std::pair<T1,T2>& p )
{
    size_t seed = 0;
    jenkins_hash_combine( seed, p.first );
    jenkins_hash_combine( seed, p.second );
    return seed;
}

///////////////////////////////////////////////////////////////////////

template <class Key>
struct jenkins_hash
    : std::unary_function<Key, size_t>
{
    size_t operator() ( const Key& key ) const
    {
        size_t h = jenkins_hash_value( key );
        return h;
    }
};

template <class T>
inline void jenkins_hash_combine( size_t& seed, const T& v )
{
    seed = jenkins_hash_value( v, seed );
}

} // namespace mm

#endif // _MM_JENKINS_HASH_FUN_HPP__
