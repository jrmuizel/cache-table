
#include <iostream>

#include "naht_map.hpp"

// #include <boost/functional/hash.hpp>

static const int S =  80000;
static const int N = 141000;

int main ( int argc, char * const argv[] ) 
{
    typedef mm::naht_map<int,int> Map;
    
    Map map( S );
    map.set_empty_key( -1 );

    for ( int i=0; i < N; i++  )
        map.insert( Map::value_type( i, i+1 ) );
    
/*
    map.insert( Map::value_type( "ciao", 11.0 ) );    
    
    map[ "prova" ] = 1.5;
    map[ "hola" ] = 3.1;
    map[ "2" ] = 2.2;

    Map::iterator it = map.begin();
    for ( ; it != map.end(); ++it  )
    {
        std::cout << "map[ " << it->first
                  << " ] = " << it->second << std::endl;
    }
*/
    std::cout << "N: " << map.size() << std::endl;
    std::cout << "Collisions: " << map.num_collisions()
              << " ( "  << double( map.num_collisions() ) / N * 100
              << " % )" << std::endl;

    std::cout << "sizeof( Map ): " << sizeof( Map ) << std::endl;
  
    return 0;
}
