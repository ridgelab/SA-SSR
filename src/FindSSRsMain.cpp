/*	
	Brandon Pickett
	FindSSRsMain.cpp -- Main
*/

// include statements
#include "../include/FindSSRs.h"
#include "../include/FindSSRsArgs.h"

// using statements
using namespace std;

// main
int main(int argc, char* argv[])
{
	FindSSRsArgs* args = new FindSSRsArgs(argc, argv);

	FindSSRs find_ssrs(args);

	int ret_val = find_ssrs.run();

	delete args;

	return ret_val;
}
