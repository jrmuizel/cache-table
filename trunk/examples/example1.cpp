
#include <iostream>

#include <mm/cache_map.hpp>

static const int S =  80000;

typedef mm::cache_map<std::string,double> Map;

int main ( int argc, char * const argv[] ) 
{
    Map map( S );
    map.set_empty_key( "" );

    map.insert( Map::value_type( "ciao", 11.0 ) );    
    
    map[ "prova" ] = 1.5;
    map[ "hola" ] = 3.1;
    map[ "2" ] = 2.2;

    for ( Map::iterator it = map.begin(); it != map.end(); ++it  )
    {
        std::cout << "map[ " << it->first
                  << " ] = " << it->second << std::endl;
    }

    std::cout << "Items: " << map.size() << std::endl;
    std::cout << "Collisions: " << map.num_collisions() << std::endl;

    std::cout << "sizeof( Map ): " << sizeof( Map ) << std::endl;
  
    return 0;
}
