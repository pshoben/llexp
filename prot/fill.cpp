#include <stdio.h>
#include <string.h>
#include <thread>
#include <chrono>

#include "seg.h"

int main() // int argc, char *argv[])
{
  Appendable a{};

  while( true ) {
      a.append('x');
      a.print();
      std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));
  }
  
	return 0;
}
