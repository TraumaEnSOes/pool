#include "pool.hpp"

#include <iostream>
#include <string>

int main( ) {
    Pool< std::string > pool;

    auto idx = pool.emplace( );

    pool[idx] = "Hola mundo !\n";
    std::cout << pool[idx];

    pool.free( idx );

    idx = pool.emplace( "Hola mundo 2\n" );
    std::cout << pool[idx];
    pool.free( idx );

    return 0;
}
