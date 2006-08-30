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

#ifndef _CACHE_TABLE_HPP_
#define _CACHE_TABLE_HPP_

#include <cstddef>

// #include <sys/mman.h>

namespace mm 
{

using std::pair;
    
template < class Value, class Key, class DiscardFunction,
           class HashFunction, class KeyEqual, class KeyExtract,
           class Allocator
         > class cache_table;

// ITERATORS

template <class V, class K, class DF, class HF, class KEq, class KEx, class A>
class cache_table_const_iterator;

////////////////////////////////////////////////////////////////////////

/**
 * Iterator class 
 */
template <class V, class K, class DF, class HF, class KEq, class KEx, class A>
class cache_table_iterator
{
public:
    typedef cache_table<V,K,DF,HF,KEq,KEx,A>                cache_table;
    typedef cache_table_iterator<V,K,DF,HF,KEq,KEx,A>       iterator;
    typedef cache_table_const_iterator<V,K,DF,HF,KEq,KEx,A> const_iterator;
    
    typedef V& reference;
    typedef V* pointer;
    typedef K  key_type;
    typedef V  value_type;
    typedef typename cache_table::size_type size_type;
    
private:
   /**
     * Constructor.
     * Creates an iterator that points to a specified item.
     *
     * \param table pointer to the cache_table instance
     * \param it pointer to an item in the table
     * \param advance_to_next if true, the iterator will
     *        advance until it reach a valid (non-empty)
     *        item.
     */
    cache_table_iterator( const cache_table* table, pointer it,
                          bool advance_to_next = false )
        : m_ht( table ), 
          m_pos( it )
    {
        if ( advance_to_next )
            advance_to_next_item();
    }
    
public:

    /**
     * Default Constructor.
     * Construct an iterator that points to no item.
     */
    cache_table_iterator()
        : m_ht( 0 ), m_pos( 0 )
    {}
    
    /// Copy constructor
    cache_table_iterator( const cache_table_iterator& it )
        : m_ht(  it.m_ht  ), 
          m_pos( it.m_pos )
    {}

    /**
     * Dereferencing operator.
     * Returns a reference to the item pointed by the iterator.
     */
    reference operator*() const { return *m_pos; }
    /// Dereferencing operator
    pointer operator->()  const { return &(operator*()); }

    /// Advance the iterator. Note that no bound check is performed, you
    /// should not pass the 'end()' limit.
    iterator& operator++()
    {
        ++m_pos;
        advance_to_next_item();
        return *this;
    }
        
    iterator operator++(int) { iterator tmp(*this); ++*this; return tmp; }
        
    /// Comparison 
    bool operator==( const iterator& it ) const { return m_pos == it.m_pos; }
    bool operator!=( const iterator& it ) const { return !(m_pos == it.m_pos); }
        
private:

    /// Advance the iterator to the next non-empty item
    void advance_to_next_item()
    {
        const key_type&   empty_key  = m_ht->m_empty_key;
        const value_type* end_marker = m_ht->m_end_marker;
        
        while (    m_pos < end_marker
                && m_ht->m_key_extract( *m_pos ) == empty_key
              )
            ++m_pos;
    }
        
    const cache_table* m_ht;
    pointer            m_pos;

    friend class cache_table_const_iterator<V,K,DF,HF,KEq,KEx,A>;
    friend class cache_table;// <V,K,DF,HF,KEq,KEx,A>;
};

////////////////////////////////////////////////////////////////////////
template <class V, class K, class DF, class HF, class KEq, class KEx, class A>
class cache_table_const_iterator
{
public:
    typedef cache_table<V,K,DF,HF,KEq,KEx,A>                cache_table;
    typedef cache_table_iterator<V,K,DF,HF,KEq,KEx,A>       iterator;
    typedef cache_table_const_iterator<V,K,DF,HF,KEq,KEx,A> const_iterator;
    
    typedef const V& reference;
    typedef const V* pointer;
    typedef K        key_type;
    typedef V        value_type;
    typedef typename cache_table::size_type         size_type;

    cache_table_const_iterator()
        : m_ht( 0 ), m_pos( 0 )
    {}
        
    cache_table_const_iterator( const cache_table* table, pointer it,
                                bool advance_to_next = false )
        : m_ht( table ), 
          m_pos( it )
    {
        if ( advance_to_next )
            advance_to_next_item();
    }
        
    cache_table_const_iterator( const iterator& it )
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
        const key_type&   empty_key  = m_ht->m_empty_key;
        const value_type* end_marker = m_ht->m_end_marker;
        
        while (    m_pos < end_marker
                && m_ht->m_key_extract( *m_pos ) == empty_key
              )
            ++m_pos;
    }
        
    const cache_table* m_ht;
    pointer            m_pos;

    friend class cache_table;// <V,K,DF,HF,KEq,KEx,A>;
};

////////////////////////////////////////////////////////////////////////
/*
 * CacheTable low-level implementation
 */
template < class Value, 
           class Key,
           class DiscardFunction,
           class HashFunction, 
           class KeyEqual,
           class KeyExtract,
           class Allocator // not used
         >
class cache_table
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
    
    typedef cache_table_iterator< Value, Key, DiscardFunction, HashFunction,
                                  KeyEqual, KeyExtract, Allocator > iterator;
    typedef cache_table_const_iterator< Value, Key, DiscardFunction, HashFunction,
                                        KeyEqual, KeyExtract, Allocator
                                      > const_iterator;
        
    typedef Allocator allocator_type;
    
private:
    HashFunction    m_hasher;
    KeyEqual        m_key_equal;
    KeyExtract      m_key_extract;
    
    DiscardFunction m_discard;
    
public:
    
    hasher hash_funct() const { return m_hasher;    }
    key_equal key_eq()  const { return m_key_equal; }
        
public:
            
    cache_table( size_type size ) 
        : m_buckets( round_to_power2( size ) ),
          m_mask( m_buckets - 1 ),
          m_num_elements( 0 ),
          m_num_collisions( 0 ),
          m_empty_key_is_set( false ),
          m_table( 0 )
    {}

    void set_empty_value( const value_type& empty_value )
    {
        assert( m_empty_key_is_set == false );
        
        m_empty_key = m_key_extract( empty_value );
        m_empty_value = empty_value;
        
        m_table = static_cast<value_type*>( malloc( m_buckets * ItemSize ) );
        // m_table = static_cast<value_type*>( mmap( 0, m_buckets * ItemSize,
        //                                          PROT_READ | PROT_WRITE,
        //                                          MAP_SHARED | MAP_ANONYMOUS,
        //                                          0, 0 )
        //                                   );
        assert( m_table );
        m_end_marker = m_table + m_buckets;
        
        initialize_memory();
        m_empty_key_is_set = true;
    }
        
    ~cache_table() 
    {
        if ( m_empty_key_is_set )
        {
            clear();
            free( m_table );
            // munmap( m_table, m_buckets * ItemSize );
        }
    }

    // INSERTIONS
        
    pair<iterator,bool> insert( const value_type& obj )
    {
        assert( m_empty_key_is_set == true );
        
        const key_type& obj_key = m_key_extract( obj );
        size_t buck = m_hasher( obj_key ) & m_mask;
            
        const key_type& table_key = m_key_extract( m_table[ buck ] );
        
        if ( ! m_key_equal( table_key, m_empty_key ) )
        {
            // Element is discarded
            ++m_num_collisions;
            m_discard( m_table[ buck ], obj );
        }
        else
            ++m_num_elements;
        
        m_table[ buck ] = obj;
        return pair<iterator,bool>( iterator( this, m_table + buck ), false );
    }

    template <class InputIterator>
    void insert( InputIterator first, InputIterator last )
    {
        while ( first != last )
            insert( *( first++ ) );
    }
    
    // SEARCHES
    
    iterator find( const key_type& key )
    {
        assert( m_empty_key_is_set == true );

        if ( size() == 0 ) return end();

        // First of all, obtain the bucket number corresponding with the
        // supplied key
        size_t buck = m_hasher( key ) & m_mask;
        
        // Next, check that the key found in table bucket is equal to the
        // supplied key. If not, either the bucket is empty and so the key
        // wasn't found, or the bucket is hosting a different value with a
        // hash collision.
        if ( ! m_key_equal( m_key_extract( m_table[ buck ] ), key ) )
            return end();

        // else return the iterator to found item
        return iterator( this, m_table + buck );
    }

    value_type& find_or_insert( const key_type& key )
    {
        assert( m_empty_key_is_set == true );
        
        size_t buck = m_hasher( key ) & m_mask;
        key_type& table_key = m_key_extract( m_table[ buck ] );
        
        if ( ! m_key_equal( key, table_key ) )
        {            
            // The bucket is either empty or it contains another item.  In
            // this case we have to mark it as discarded, because we cannot
            // know how the reference will be used ( probably to overwrite
            // the item with a new one that has a different key ).
            if ( ! m_key_equal( table_key, m_empty_key ) )
            {
                // The bucket already contained an item. This item is
                // discarded and replaced with an empty one. The destructor
                // is called on it.
                // The m_num_elements does not change because 
                m_discard( m_table[ buck ], m_empty_value );
                m_table[ buck ].~value_type();
                m_table[ buck ] = m_empty_value;
            }
            else
            {
                // The bucket was empty, so we have to increment the items
                // counter
                ++m_num_elements;
            }

            // Set the key in the empty item
            table_key = key;
        }

        // Returns the reference to the found or recently added item
        return m_table[ buck ];
    }
    
    size_type erase( const key_type& key )
    {
        iterator it = find( key );
        if ( it != end() )
        {
            erase( it );
            return 1;
        }
            
        return 0;
    }
        
    void erase( iterator it )
    {
        if (    it != end()
                && ! m_key_equal( m_key_extract( *it ), m_empty_key ) )
        {
            it->~value_type();
            *it = m_empty_value;
            --m_num_elements;
        }
    }
        
    void erase( iterator first, iterator last )
    {
        while ( first != last )
            erase( first++ );
    }

    void resize( size_type num_elements_hint )
    {
        // The resize calls are simply ignored,
        // our size is fixed..
    }

    void clear()
    {
        erase( begin(), end() );
    }

    // Iterator functions
    iterator begin()             { return iterator( this, m_table, true ); }
    iterator end()               { return iterator( this, m_end_marker ); }
    const_iterator begin() const { return const_iterator( this, m_table, true ); }
    const_iterator end()   const { return const_iterator( this, m_end_marker ); }
    
    size_type size()         const { return m_num_elements; }
    size_type max_size()     const { return m_buckets; }
    size_type bucket_count() const { return m_buckets; }
    bool empty()             const { return size() == 0; }
    
    size_type num_collisions() const { return m_num_collisions; }

    // Comparison
    bool operator==( const cache_table& other ) const
    {
        return (    m_table == other.m_table
                 && m_num_elements == m_num_elements );
    }

    bool operator!=( const cache_table& other ) const
    { return !( *this == other ); }
        
    void swap( cache_table& other)
    {
        // Swap all the fields
        std::swap( m_hasher,           other.m_hasher           );
        std::swap( m_key_equal,        other.m_key_equal        );
        std::swap( m_key_extract,      other.m_key_extract      );
        std::swap( m_mask,             other.m_mask             );
        std::swap( m_buckets,          other.m_buckets          );
        std::swap( m_num_elements,     other.m_num_elements     );
        std::swap( m_num_collisions,   other.m_num_collisions   );
        std::swap( m_empty_key_is_set, other.m_empty_key_is_set );
        std::swap( m_table,            other.m_table            );
        std::swap( m_empty_key,        other.m_empty_key        );
        std::swap( m_empty_value,      other.m_empty_value      );
        std::swap( m_end_marker,       other.m_end_marker       );
    }
        
private:
    /// It's not possible to construct a cache_table without specifying its
    /// size.
    cache_table() {}

    /// Array item size
    static const size_t ItemSize = sizeof(value_type);

    /// Initialize the whole hash table with the empty value
    void initialize_memory()
    {
        std::uninitialized_fill( m_table,
                                 m_table + m_buckets,
                                 m_empty_value );
    }
        
    /// Round the number to the next power of 2.
    static size_t round_to_power2( size_t n )
    {
        size_t x = 1, i = 0;
        while ( (i++ < 64) &&  (x < n) )
            x <<= 1;
        
        return x;
    }

    friend class cache_table_iterator< Value, Key, DiscardFunction,
                                       HashFunction, KeyEqual, KeyExtract,
                                       Allocator >;
    friend class cache_table_const_iterator< Value, Key, DiscardFunction,
                                             HashFunction, KeyEqual,
                                             KeyExtract, Allocator >;
        
    // Internal data 
        
    size_t m_buckets;           ///< Number of buckets in the hash table
    size_t m_mask;              ///< Mask used to calculate bucket
    size_t m_num_elements;      ///< Number of elements in the table
    size_t m_num_collisions;    ///< Number of collisions
    bool   m_empty_key_is_set;  ///< Tells whether the empty key has
                                      ///  been set

    value_type* m_table;       ///< The 'real' hash table array     
    key_type    m_empty_key;   ///< The key value that identifies empty items
    value_type  m_empty_value; ///< The value that identifies empty items
    value_type* m_end_marker;  ///< Pointer to the end of the table
};

/**
 * @relates cache_table
 */
template <class V, class K, class DF, class HF, class KEq, class KEx, class A>
inline void swap( cache_table<V,K,DF,HF,KEq,KEx,A>& ht1, 
                  cache_table<V,K,DF,HF,KEq,KEx,A>& ht2 )
{
    ht1.swap( ht2 );
}

} // namespace mm

#endif // _CACHE_TABLE_HPP_
