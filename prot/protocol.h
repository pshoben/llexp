#pragma once

#include <memory>
#include <iostream>
#include <cstdlib>

using std::cout;

namespace llprot {

typedef int int_array3[3];

constexpr static uint32_t BLOCK_SIZE = 64;

using block_int_t = int[ BLOCK_SIZE ];

static int_array3 ia{1,2,3};
 
//block_int_t & create_block() 
//{
//    return (block_int_t &) new block_int_t;
//}


int_array3& foo() { return ia; }


/*
void fill_block( int[ BLOCK_SIZE ] & t, int v ) 
{
    for( int& tv : t ) {
        tv = v;
    }
}

void print_block( int[ BLOCK_SIZE ] & t ) 
{
    for( int tv : t ) {
        cout << v << " ";
    }
    cout << "\n";
}
*/
}
