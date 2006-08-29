/*
 *  naht_table.hpp
 *  Naht++
 *
 *  Created by Matteo Merli on 22/08/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _NAHT_TABLE_HPP_
#define _NAHT_TABLE_HPP_

#include <iostream> // debug...

namespace mm 
{
    using std::pair;
    
    template < class Value, class Key, class HashFunction, 
               class KeyEqual, class KeyExtract, class Allocator
             > class naht_table;

    // ITERATORS

    template <class V, class K, class HF, class KEq, class KEx, class A>
    class naht_table_const_iterator;

    ////////////////////////////////////////////////////////////////////////
    template <class V, class K, class HF, class KEq, class KEx, class A>
    class naht_table_iterator
    {
    public:
        typedef naht_table<V,K,HF,KEq,KEx,A>                naht_table;
        typedef naht_table_iterator<V,K,HF,KEq,KEx,A>       iterator;
        typedef naht_table_const_iterator<V,K,HF,KEq,KEx,A> const_iterator;

        typedef V& reference;
        typedef V* pointer;
        typedef K  key_type;
        typedef V  value_type;
        typedef typename naht_table::size_type size_type;

        naht_table_iterator()
            : m_ht( 0 ), m_pos( 0 )
        {}
        
        naht_table_iterator( const naht_table* table, pointer it,
                             bool advance_to_next = false )
            : m_ht( table ), 
              m_pos( it )
        {
            if ( advance_to_next )
                advance_to_next_item();
        }
        
        naht_table_iterator( const naht_table_iterator& it )
            : m_ht(  it.m_ht  ), 
              m_pos( it.m_pos )
        {}
        
        reference operator*() const { return *m_pos; }
        pointer operator->()  const { return &(operator*()); }

        iterator& operator++()
        {
            ++m_pos;
            advance_to_next_item();
            return *this;
        }
        
        iterator operator++(int) { iterator tmp(*this); ++*this; return tmp; }
        
        // Comparison.
        bool operator==( const iterator& it ) const { return m_pos == it.m_pos; }
        bool operator!=( const iterator& it ) const { return !(m_pos == it.m_pos); }
        
    private:

        void advance_to_next_item()
        {
            const key_type empty_key = m_ht->m_empty_key;
            const value_type* end_marker = m_ht->m_end_marker;

            while (    m_ht->m_key_extract( *m_pos ) != empty_key 
                    && m_pos < end_marker )
                ++m_pos;
        }
        
        const naht_table* m_ht;
        pointer m_pos;

        friend class naht_table_const_iterator<V,K,HF,KEq,KEx,A>;
    };

    ////////////////////////////////////////////////////////////////////////
    template <class V, class K, class HF, class KEq, class KEx, class A>
    class naht_table_const_iterator
    {
    public:
        typedef naht_table<V,K,HF,KEq,KEx,A>                naht_table;
        typedef naht_table_iterator<V,K,HF,KEq,KEx,A>       iterator;
        typedef naht_table_const_iterator<V,K,HF,KEq,KEx,A> const_iterator;
        
        typedef const V& reference;
        typedef const V* pointer;
        typedef K        key_type;
        typedef V        value_type;
        typedef typename naht_table::size_type         size_type;

        naht_table_const_iterator()
            : m_ht( 0 ), m_pos( 0 )
        {}
        
        naht_table_const_iterator( const naht_table* table, pointer it,
                                   bool advance_to_next = false )
            : m_ht( table ), 
              m_pos( it )
        {
            if ( advance_to_next )
                advance_to_next_item();
        }
        
        naht_table_const_iterator( const iterator& it )
            : m_ht(  it.m_ht  ), 
              m_pos( it.m_pos )
        {}
        
        reference operator*() const { return *m_pos; }
        pointer operator->()  const { return &(operator*()); }

        const_iterator& operator++()
        {
            ++m_pos;
            advance_to_next_item();
            return *this;
        }
        
        const_iterator operator++(int)
        { const_iterator tmp(*this); ++*this; return tmp; }
        
        // Comparison.
        bool operator==( const const_iterator& it ) const
        { return m_pos == it.m_pos; }
        bool operator!=( const const_iterator& it ) const
        { return !(m_pos == it.m_pos); }
        
    private:

        void advance_to_next_item()
        {
            const key_type empty_key = m_ht->m_empty_key;
            const value_type* end_marker = m_ht->m_end_marker;

            while (    m_ht->m_key_extract( *m_pos ) != empty_key 
                    && m_pos < end_marker )
                ++m_pos;
        }
        
        const naht_table* m_ht;
        pointer m_pos;
    };

    ////////////////////////////////////////////////////////////////////////
    /**
     ****
     */
    template < class Value, 
               class Key, 
               class HashFunction, 
               class KeyEqual,
               class KeyExtract,
               class Allocator // not used
             >
    class naht_table
    {
    public:
        typedef Key             key_type;
        typedef Value           value_type;
        typedef HashFunction    hasher;
        typedef KeyEqual        key_equal;
        typedef KeyExtract      key_extract;
        
        typedef size_t            size_type;
        typedef ptrdiff_t         difference_type;
        typedef value_type*       pointer;
        typedef const value_type* const_pointer;
        typedef value_type&       reference;
        typedef const value_type& const_reference;

        typedef naht_table_iterator< Value, Key, HashFunction, KeyEqual,
                                     KeyExtract, Allocator > iterator;
        typedef naht_table_const_iterator< Value, Key, HashFunction, KeyEqual,
                                           KeyExtract, Allocator > const_iterator;
        
        typedef Allocator allocator_type;
         
    private:
        HashFunction m_hasher;
        KeyEqual     m_key_equal;
        KeyExtract   m_key_extract;
        
    public:
        
        hasher hash_funct() const { return m_hasher; }
        key_equal key_eq() const { return m_key_equal; }
        
    public:
            
        naht_table( size_type size ) 
            : m_table( 0 ),
              m_buckets( round_to_power2( size ) ),
              m_num_elements( 0 ),
              m_num_collisions( 0 ),
              empty_key_is_set( false )
        {
            m_mask = m_buckets - 1;
            
            /*
            std::cout << "size: " << size << std::endl;
            std::cout << "ItemSize: " << ItemSize << std::endl;
            std::cout << "m_buckets: " << m_buckets << std::endl;
            std::cout << "Memory Size: " << ( m_buckets * ItemSize / 1024.0 )
                      << " Kb" << std::endl;
            */
        }

        void set_empty_value( const value_type& empty_value )
        {
            assert( empty_key_is_set == false );
            
            m_empty_key = m_key_extract( empty_value );
            m_empty_value = empty_value;

            m_table = static_cast<value_type*>( malloc( m_buckets * ItemSize ) );
            assert( m_table );
            m_end_marker = m_table + m_buckets;

            clear();
            empty_key_is_set = true;
        }
        
        ~naht_table() 
        {
            if ( empty_key_is_set ) free( m_table );
        }

        // INSERTIONS
        
        pair<iterator,bool> insert( const value_type& obj )
        {
            assert( empty_key_is_set == true );
            
            const key_type& key = m_key_extract( obj );
            size_t buck = m_hasher( key ) & m_mask;
            
            if ( m_table[ buck ].first != m_empty_key )
            {
                // Element offset[i] is discarded
                // TODO: Use a policy to execute actions at
                // this moment.
                ++m_num_collisions;
                /*
                std::cout << "Discarding element: "
                          << m_extract_key( m_table[ buck ] ) << std::endl;
                */
            }
            else
                ++m_num_elements;
            
            m_table[ buck ] = obj;
            return pair<iterator,bool>( iterator( this, m_table + buck ), true );
        }

        template <class InputIterator>
        void insert( InputIterator first, InputIterator last )
        {
            while ( first != last ) insert( *first++ );
        }

        // SEARCHES
        
        iterator find( const key_type& key )
        {
            assert( empty_key_is_set == true );
            
            if ( size() == 0 ) return end();
            size_t buck = m_hasher( key ) & m_mask;

            if ( ! m_key_equal( m_key_extract( m_table[ buck ] ), 
                                key ) )
            {
                return end();
            }
            
            return iterator( this, m_table + buck );
        }

        value_type& find_or_insert( const key_type& key )
        {
            assert( empty_key_is_set == true );
            
            size_t buck = m_hasher( key ) & m_mask;

            if ( ! m_key_equal( m_table[ buck ].first, key ) )
            {
                // Key not found, return a reference to an
                // empty item.
                return m_table[ buck ];
            }
            
            return *iterator( this, m_table + buck );
        }

        size_type erase( const key_type& key )
        {
            size_type n = 0;

            iterator it = find( key );
            if ( it != end() )
                *it = m_empty_value;
            
            return n;
        }
        
        void erase( iterator it ) { *it = m_empty_value; }
        
        void erase( iterator first, iterator last )
        {
            while ( first != last )
                *first++ = m_empty_value;
        }

        void resize( size_type num_elements_hint )
        {
            // The resize calls are simply ignored,
            // our size is fixed..
        }

        void clear()
        {
            std::uninitialized_fill( m_table,
                                     m_table + m_buckets,
                                     m_empty_value );
        }

        // Iterator functions
        iterator begin() { return iterator( this, m_table, true ); }
        iterator end()   { return iterator( this, m_end_marker ); }
        const_iterator begin() const { return const_iterator( this, m_table, true ); }
        const_iterator end()   const { return const_iterator( this, m_end_marker ); }

        size_type size()         const { return m_num_elements; }
        size_type max_size()     const { return m_buckets; }
        size_type bucket_count() const { return m_buckets; }
        bool empty()             const { return size() == 0; }
        
        size_type num_collisions() const { return m_num_collisions; }

        // Comparison
        bool operator==( const naht_table& other ) const
        {
            return (    m_table == other.m_table
                     && m_num_elements == m_num_elements );
        }

        bool operator!=( const naht_table& other ) const
        { return !( *this == other ); }
        
        void swap( naht_table& other)
        {
            std::swap( m_hasher, other.m_hasher );
            std::swap( m_key_equal, other.m_key_equal );
            std::swap( m_key_extract, other.m_key_extract );
            std::swap( m_mask, other.m_mask );
            std::swap( m_buckets, other.m_buckets );
            std::swap( m_num_elements, other.m_num_elements );
            std::swap( m_num_collisions, other.m_num_collisions );
            std::swap( empty_key_is_set, other.empty_key_is_set );
            std::swap( m_table, other.m_table );
            std::swap( m_empty_key, other.m_empty_key );
            std::swap( m_empty_value, other.m_empty_value );
            std::swap( m_end_marker, other.m_end_marker );
        }
        
    private:
        /**
         * It's not possible to construct a naht_table without specifying 
         * its size.
         */
        naht_table() {}
        
        // Internal data
        
        size_t m_mask;
        size_t m_buckets;
        size_t m_num_elements;
        size_t m_num_collisions;

        bool empty_key_is_set;

        value_type* m_table;
        key_type m_empty_key;
        value_type m_empty_value;

        value_type* m_end_marker;
        
        static const size_t ItemSize = sizeof(value_type);
        
        /**
         * Round the number to the next power of 2.
         */
        static size_t round_to_power2( size_t __n )
        {
            size_t x = 1, i = 0;
            while ( (i++ < 64) &&  (x < __n) )
                x <<= 1;
            
            return x;
        }

        friend class naht_table_iterator< Value, Key, HashFunction, KeyEqual,
                                          KeyExtract, Allocator >;
        friend class naht_table_const_iterator< Value, Key, HashFunction, KeyEqual,
                                                KeyExtract, Allocator >;
        
    };
    
} // namespace mm

template <class V, class K, class HF, class KEq, class KEx, class A>
inline void swap( mm::naht_table<V,K,HF,KEq,KEx,A>& ht1, 
                  mm::naht_table<V,K,HF,KEq,KEx,A>& ht2 )
{
    std::cout << "TABLE SWAP..." << std::endl;
    ht1.swap( ht2 );
}

#endif // _NAHT_TABLE_HPP_
