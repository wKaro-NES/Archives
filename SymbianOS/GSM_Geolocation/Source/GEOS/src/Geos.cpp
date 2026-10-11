// INCLUDE FILES
#include <stdio.h>
#include <iostream>

// This is a GCCE toolchain workaround needed when compiling with GCCE
// and using main() entry point
#ifdef __GCCE__
#include <staticlibinit_gcce.h>
#endif

int main(void)
	{
	std::cout << "GEOS." << std::endl;
	
	std::cout << std::endl << "[press any key]" << std::endl;
	
	int c;
	std::cin >> c;

	return 0;
	}
