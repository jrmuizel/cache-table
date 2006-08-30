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

#ifndef _CACHE_MAP_HPP_
#define _CACHE_MAP_HPP_

#include "cache_table.hpp"
#include "hash_fun.hpp"

namespace mm
{
    
using std::equal_to;
using std::pair;
using std::allocator;
using std::_Select1st;

/*! Default "discarding" policy.
 *  Ignores discarded item. You can write another policy to do something
 *  useful with this item.
 */
template < class V >
struct DiscardIgnore
{
    void operator() ( const V& old_value, const V& new_value )
    {}
};

/*! Cache map...
 *
 *  <b>Template Parameters</b>
 * 
 *  - @a Key : The key type of the map.
 *  - @a T   : The data type of the map.
 *
 *
 *  @author Matteo Merli
 *  @date 2006
 */
template < class Key, 
           class T,
           class HashFunction = hash<Key>,
           class KeyEqual = equal_to<Key>,
           class DiscardFunction = DiscardIgnore< pair<Key,T> >,
           class Allocator = allocator<T> // not used
>
class cache_map
{
private:
    /// The actual hash table
    typedef cache_table< pair<Key,T>, Key,
                         DiscardFunction,
                         HashFunction, KeyEqual,
                         _Select1st< pair<Key,T> >,
                         Allocator
                       > HT;
    HT m_ht;
        
public:
    typedef typename HT::key_type key_type;
    typedef T data_type;
    typedef T mapped_type;
        
    typedef typename HT::value_type value_type;
    typedef typename HT::hasher hasher; ///< Hash function policy

    /// Key Comparision policy
    typedef typename HT::key_equal key_equal;
    
    typedef typename HT::size_type size_type;
    typedef typename HT::difference_type difference_type;
    typedef typename HT::pointer pointer;
    typedef typename HT::const_pointer const_pointer;
    typedef typename HT::reference reference;
    typedef typename HT::const_reference const_reference;
    
    typedef typename HT::iterator iterator;

    /// Const iterator type
    typedef typename HT::const_iterator const_iterator;
    
public:
        
    hasher hash_funct() const { return m_ht.hash_funct(); }
    key_equal key_eq() const  { return m_ht.key_eq(); }

    /// Get an iterator to first item
    iterator begin()             { return m_ht.begin(); }
    /// Get an iterator to the end of the table
    iterator end()               { return m_ht.end();   }
    /// Get a const iterator to first item
    const_iterator begin() const { return m_ht.begin(); }
    /// Get a const iterator to the end of the table
    const_iterator end()   const { return m_ht.end();   }
    
public:

    /*! Constructor.  You have to specify the size of the underlying table,
     *  in terms of the maximum number of allowed elements.
     * 
     *  @attention After the cache_map is constructed, you have to call the
     *  set_empty_key() method to set the value of an unused key.
     *
     *  @param size The (fixed) size of table.
     *  @see set_empty_key
     */
    cache_map( size_type size ) : m_ht( size ) {}

    /*! Sets the value of the empty key.
     *   
     *  @param key the key value that will be used to identify empty items.
     */
    void set_empty_key( const key_type& key )
    {
        m_ht.set_empty_value( value_type( key, data_type() ) );
    }
    
    pair<iterator,bool> insert( const value_type& obj )
    { return m_ht.insert( obj ); }

    pair<iterator,bool> insert( const key_type& key, const data_type& data )
    { return m_ht.insert( value_type( key, data ) ); }

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
        
    data_type& operator[]( const key_type& key )
    { return m_ht.find_or_insert( key ).second; }

    size_type erase( const key_type& key ) { return m_ht.erase( key ); }
    void erase( iterator it ) { m_ht.erase( it ); }
    void erase( iterator first, iterator last ) { m_ht.erase( first, last ); }

    void clear() { m_ht.clear(); }

    void swap( cache_map& other ) { m_ht.swap( other.m_ht ); }

    // Comparison
    bool operator==( const cache_map& other ) const
    { return m_ht == other.m_ht; }
    bool operator!=( const cache_map& other ) const
    { return m_ht != other.m_ht; }

    /*! Get the size of the map.
     *  The size represent the number of non-empty elements that can be
     *  found in the container.
     *  @return the number of elements in the map
     */
    size_type size()         const { return m_ht.size(); }
    
    /*! Get the maximum size of the map.
     *  This corresponds to the maximum number of item that the map can
     *  contain.
     *  @return the maximum number of elements
     */
    size_type max_size()     const { return m_ht.max_size(); }

    /*! Get the number of allocated buckets.
     *  This corresponds to the max_size() value.
     *  @return the number of buckets
     *  @see max_size
     */
    size_type bucket_count() const { return m_ht.bucket_count(); }
    
    /*! Test for empty.
     *  @return true if the cache_map does not contains items.
     */
    bool empty()             const { return m_ht.empty(); }

    /*!
     * 
     *  @return the number of hash key collisions
     */
    size_type num_collisions() const { return m_ht.num_collisions(); }

    /*! Swap the content of two cache_map instances.
     */
    friend inline void swap(
        cache_map<Key,T,HashFunction,KeyEqual,DiscardFunction,Allocator>& m1,
        cache_map<Key,T,HashFunction,KeyEqual,DiscardFunction,Allocator>& m2 )
    {
        m1.swap( m2 );
    }
        
private:
    /*! It's not possible to construct a cache_table without specifying its
     *  size.
     */
    cache_map()
    {}
    
};
    
} // namespace mm

#endif // _CACHE_MAP_HPP_
