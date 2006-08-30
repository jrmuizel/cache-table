/*
 *
 *  $Id$
 *
 *  $URL$
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

#ifndef _CACHE_SET_HPP_
#define _CACHE_SET_HPP_

#include "cache_table.hpp"
#include "hash_fun.hpp"

namespace mm
{
    
using std::equal_to;
using std::allocator;

template < class V >
struct CacheSetDiscardIgnore
{
    void operator() ( const V& old_value, const V& new_value )
    {}    
};

template <class Value>
struct Identity
{
    Value& operator() ( Value& value ) const { return value; }
    const Value& operator() ( const Value& value ) const { return value; }
};

template < class Value,
           class HashFunction = hash<Value>,
           class KeyEqual = equal_to<Value>,
           class DiscardFunction = CacheSetDiscardIgnore< Value >,
           class Allocator = allocator<Value> // not used
>
class cache_set
{
private:
    
    // The actual data
    typedef cache_table< Value, Value,
                         DiscardFunction,
                         HashFunction, KeyEqual,
                         Identity<Value>,
                         Allocator
                       > HT;
    HT m_ht;
        
public:
    typedef typename HT::key_type key_type;
    typedef Value data_type;
    typedef Value mapped_type;
        
    typedef typename HT::value_type value_type;
    typedef typename HT::hasher hasher;
    typedef typename HT::key_equal key_equal;
    
    typedef typename HT::size_type size_type;
    typedef typename HT::difference_type difference_type;
    typedef typename HT::pointer pointer;
    typedef typename HT::const_pointer const_pointer;
    typedef typename HT::reference reference;
    typedef typename HT::const_reference const_reference;
    
    typedef typename HT::iterator iterator;
    typedef typename HT::const_iterator const_iterator;
    
public:
        
    hasher hash_funct() const { return m_ht.hash_funct(); }
    key_equal key_eq() const  { return m_ht.key_eq(); }

    // Iterator functions
    iterator begin()  { return m_ht.begin(); }
    iterator end()    { return m_ht.end();   }
    const_iterator begin() const { return m_ht.begin(); }
    const_iterator end()   const { return m_ht.end();   }
    
public:
    
    cache_set( size_type size ) : m_ht( size ) {}
    
    void set_empty_key( const value_type& value )
    {
        m_ht.set_empty_value( value );
    }
    
    pair<iterator,bool> insert( const value_type& obj )
    { return m_ht.insert( obj ); }

    template <class InputIterator>
    void insert( InputIterator first, InputIterator last )
    { m_ht.insert( first, last ); }

    template <class InputIterator>
    pair<iterator,bool> insert_noresize( InputIterator first,
                                         InputIterator last )
    { m_ht.insert_noresize( first, last ); }

    iterator insert( iterator it, const value_type& obj )
    { return m_ht.insert( obj ).first; }
        
    iterator find( const key_type& key )
    { return m_ht.find( key ); }

    const_iterator find( const key_type& key ) const
    { return m_ht.find( key ); }

    size_type erase( const key_type& key ) { return m_ht.erase( key ); }
    void erase( iterator it ) { m_ht.erase( it ); }
    void erase( iterator first, iterator last ) { m_ht.erase( first, last ); }

    void clear() { m_ht.clear(); }

    void swap( cache_set& other ) { m_ht.swap( other.m_ht ); }

    // Comparison 
    bool operator==( const cache_set& other ) const
    { return m_ht == other.m_ht; }
    bool operator!=( const cache_set& other ) const
    { return m_ht != other.m_ht; }

    size_type size()         const { return m_ht.size(); }
    size_type max_size()     const { return m_ht.max_size(); }
    size_type bucket_count() const { return m_ht.bucket_count(); }
    bool empty()             const { return m_ht.empty(); }
        
    size_type num_collisions() const { return m_ht.num_collisions(); }
    
    friend inline void swap(
        cache_set<Value,HashFunction,KeyEqual,DiscardFunction,Allocator>& m1,
        cache_set<Value,HashFunction,KeyEqual,DiscardFunction,Allocator>& m2 )
    {
        m1.swap( m2 );
    }
        
private:
    /**
     * It's not possible to construct a cache_table without specifying 
     * its size.
     */
    cache_set()
    {}
    
};
    
} // namespace mm

#endif // _CACHE_SET_HPP_
