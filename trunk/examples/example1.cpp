
#include <iostream>

#include <mm/cache_map.hpp>

const int Size =  80000;

typedef mm::cache_map<std::string,double> Map;

int main ( int argc, char * const argv[] ) 
{
    // Create the map with the specified size and set the empty key
    // value (this is actually the empty string).
    Map map( Size );
    map.set_empty_key( "" );

    // Insert an item in the map. Map::value_type is an alias to, in
    // this case, std::pair<std::string,double>.
    map.insert( Map::value_type( "ciao", 11.0 ) );    
    
    // Like std::map, this is the alternate form for inserting items
    // in the map.
    map[ "prova" ] = 1.5;
    map[ "hola" ] = 3.1;
    map[ "2" ] = 2.2;

    // Be careful that, as in std::map, if you look for a key that
    // doesn't exists, it will be inserted with default value. 
    double d = map[ "not existant key" ];

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
