#include <iostream>
#include <unistd.h>

#include "Outils.h"

int main(int argc, const char * argv[])
{
	std::cout << "Hello, World!\n";
	
	InitialiserApplication( XTERM );
	sleep(2);
	TerminerApplication();

	return 0;
}