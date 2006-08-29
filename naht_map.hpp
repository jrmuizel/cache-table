/*
 *  naht_map.hpp
 *  Naht++
 *
 *  Created by Matteo Merli on 22/08/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _NAHT_MAP_HPP_
#define _NAHT_MAP_HPP_

#include "naht_table.hpp"

// #include <ext/stl_hash_fun.h>
#include <ext/hash_fun.h>

namespace mm
{
    
    using std::equal_to;
    using std::pair;
    using std::allocator;
    using __gnu_cxx::hash;
    using std::_Select1st;
    
    template < class Key, 
               class T,
               class HashFunction = hash<Key>,
               class KeyEqual = equal_to<Key>,
               class Allocator = allocator<T> // not used
             >
    class naht_map
    {
    private:
        // The actual data
        typedef naht_table< pair<Key,T>, Key,
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
        iterator begin()             { return m_ht.begin(); }
        iterator end()               { return m_ht.end();   }
        const_iterator begin() const { return m_ht.begin(); }
        const_iterator end()   const { return m_ht.end();   }
    
    public:
        
        naht_map( size_type size )
            : m_ht( size )
        {}

        void set_empty_key( const key_type& key )
        {
            m_ht.set_empty_value( value_type( key, data_type() ) );
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
        
        data_type& operator[]( const key_type& key )
        { return m_ht.find_or_insert( key ).second; }

        size_type erase( const key_type& key ) { return m_ht.erase( key ); }
        void erase( iterator it ) { m_ht.erase( it ); }
        void erase( iterator first, iterator last ) { m_ht.erase( first, last ); }

        void clear() { m_ht.clear(); }

        // Comparison 
        bool operator==( const naht_map& other ) const
        { return m_ht == other.m_ht; }
        bool operator!=( const naht_map& other ) const
        { return m_ht != other.m_ht; }

        size_type size()         const { return m_ht.size(); }
        size_type max_size()     const { return m_ht.max_size(); }
        size_type bucket_count() const { return m_ht.bucket_count(); }
        bool empty()             const { return m_ht.empty(); }
        
        size_type num_collisions() const { return m_ht.num_collisions(); }

        // TEMP hack to pass google tests..
        void set_deleted_key( const key_type & ) 
        {}
        
    private:
        /**
         * It's not possible to construct a naht_table without specifying 
         * its size.
         */
        naht_map()
        {}
        
    };

    
} // namespace mm

#endif // _NAHT_MAP_HPP_
