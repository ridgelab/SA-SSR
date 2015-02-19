/*	
	Brandon Pickett
	FindSSRsMain.cpp -- Main
*/

// include statements
#include "../include/FindSSRs.h"

// using statements
using namespace std;

// main
int main(int argc, char* argv[])
{
	FindSSRs find_ssrs = FindSSRs(argc, argv);
	return find_ssrs.run();
}
