
#include <mm/cache_set.hpp>
#include <ext/stl_hash_fun.h>
#include <mm/jenkins_hash_fun.hpp>
#include <boost/functional/hash.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include <vector>

#include <sys/time.h>

using std::string;

// std::hash doesn't work with std::string... 
namespace __gnu_cxx
{
template <> struct hash<string>
{
    size_t operator()( const string& s ) { return __stl_hash_string( s.c_str() ); }
};
}

struct str_loop
{
    template <class Hasher>
    static size_t loop( int N, Hasher hasher )
    {
        size_t h = 0;
        
        for ( int i = 0; i < N; i++ )
        {
            h ^= hasher( "mid size string, you be easy" );
        }
        
        return h;
    }
};

struct words_loop
{
    template <class Hasher>
    static size_t loop( int N, Hasher hasher )
    {
        // Read 'words' file
        std::ifstream file( "words" );
        string line;
        int n = 0;

        std::vector<string> words;
        
        if ( ! file )
        {
            std::cout << "Error opening words file.." << std::endl;
            std::exit( -1 );
        }
        
        while ( file && ! file.eof() )
        {
            file >> line;
        
            if ( line.length() )
            {
                ++n;
                words.push_back( line );
            }
        }
    
        file.close();
        
        size_t h = 0;
        size_t s = words.size();

        for ( int i = 0; i < N; i++ )
            for ( size_t j = 0; j < s; j++ )
                h ^= hasher( words[ j ] );
        
        return h;
    }
};

struct int_loop
{
    template <class Hasher>
    static size_t loop( int N, Hasher hasher )
    {
        size_t h = 0;
        
        for ( int i = 0; i < N; i++ )
        {
            h ^= hasher( i );
        }

        return h;
    }
};

template <class T, class Hasher>
size_t speed_test( int N, const string& name )
{
    struct timeval ts, te;
    Hasher hasher;

    gettimeofday( &ts, NULL );

    size_t h = T::loop( N, hasher );

    gettimeofday( &te, NULL );

    double time_diff = ( te.tv_sec - ts.tv_sec )
                     + ( te.tv_usec - ts.tv_usec ) / 1e6;
    
    time_diff = time_diff * 1e9 / N;
    
    std::cout << "\t" << name << " : "
              << time_diff << " ns" << std::endl;

    return h;
}

////////////////////////////////////////////////////////////////////////

template <class Hasher>
void collision_test_int( int N, const string& name )
{
    typedef mm::cache_set< int, Hasher > Set;
    
    Set set( N );
    set.set_empty_key( -1 );

    int i = 0;
    while ( i < N )
    {
        int n = std::rand();
        
        if ( set.find( n ) != set.end() )
            continue;
        
        set.insert( n );
        ++ i;
    }
    
    std::cout << "\t" << name << " : "
              << set.num_collisions() << " ( "
              << set.num_collisions() * 100.0 / N
              << " % )" << std::endl;
}

template <class Hasher>
void collision_test_str( const string& name )
{
    typedef mm::cache_set< string, Hasher > Set;

    // this is about double the number of the words in the file
    Set set( 20000 ); 
    set.set_empty_key( "" );

    // Read 'words' file
    std::ifstream file( "words" );
    string line;
    int n = 0;

    if ( ! file )
    {
        std::cout << "Error opening words file.." << std::endl;
        std::exit( -1 );
    }
    
    while ( file && ! file.eof() )
    {
        file >> line;
        
        if ( line.length() )
        {
            ++n;
            set.insert( line );
        }
    }
    
    file.close();
    
    std::cout << "\t" << name << " : "
              << "Size: "  << set.size() << " - Collisions: "
              << set.num_collisions() << "/" << n
              <<  " ( "
              << set.num_collisions() * 100.0 / n
              << " % )" << std::endl;
    
    set.resize( 1500 );

    set.clear();
}

////////////////////////////////////////////////////////////////////////

int main()
{
    int N = (int)1E8;
    std::cout << std::endl
              << "--------- Hash Speed Test ----------"
              << std::endl << std::endl;
    std::cout << "# Repetitions: " << N << std::endl << std::endl;

    std::cout << "Integers: " << std::endl;
    speed_test< int_loop, mm::hash<int> >(         N, "DB Hash     " );
    speed_test< int_loop, mm::jenkins_hash<int> >( N, "Jenkins Hash" );
    speed_test< int_loop, __gnu_cxx::hash<int>  >( N, "STD Hash    " );
    speed_test< int_loop, boost::hash<int>      >( N, "Boost Hash  " );
    std::cout << std::endl;

    N = (int)1E6;
    std::cout << "Strings: " << std::endl;
    speed_test< str_loop, mm::hash<string> >(         N, "DB Hash     " );
    speed_test< str_loop, mm::jenkins_hash<string> >( N, "Jenkins Hash" );
    speed_test< str_loop, __gnu_cxx::hash<string> >(  N, "STD Hash    " );
    speed_test< str_loop, boost::hash<string> >(      N, "Boost Hash  " );
    std::cout << std::endl;
/*
    N = (int)1E2;
    std::cout << "Words: " << std::endl;
    speed_test< words_loop, mm::hash<string> >(         N, "DB Hash     " );
    speed_test< words_loop, mm::jenkins_hash<string> >( N, "Jenkins Hash" );
    speed_test< words_loop, __gnu_cxx::hash<string> >(  N, "STD Hash    " );
    speed_test< words_loop, boost::hash<string> >(      N, "Boost Hash  " );
    std::cout << std::endl;
*/
    std::srand( std::time( NULL ) );
    std::cout << std::endl
              << "--------- Hash Collision Test ----------"
              << std::endl << std::endl;
    std::cout << "# Repetitions: " << N << std::endl << std::endl;

    N = (int)1E7;
    std::cout << "Integers: " << std::endl;
    collision_test_int< mm::hash<int> >(         N, "DB Hash     " );
    collision_test_int< mm::jenkins_hash<int> >( N, "Jenkins Hash" );
    collision_test_int< __gnu_cxx::hash<int>  >( N, "STD Hash    " );
    collision_test_int< boost::hash<int>      >( N, "Boost Hash  " );
    std::cout << std::endl;

    std::cout << "Strings: " << std::endl;
    collision_test_str< mm::hash<string> >(         "DB Hash     " );
    collision_test_str< mm::jenkins_hash<string> >( "Jenkins Hash" );
    collision_test_str< __gnu_cxx::hash<string> >(  "STD Hash    " );
    collision_test_str< boost::hash<string> >(      "Boost Hash  " );
    std::cout << std::endl;
}
