/*	
	Brandon Pickett
	FindSSRsMain.cpp -- Main
*/

// include statements
#include <pthread.h>
#include "../include/FindSSRs.h"
#include "../include/FindSSRsArgs.h"

// using statements
using namespace std;

// main
int main(int argc, char* argv[])
{
	FindSSRsArgs* args = new FindSSRsArgs(argc, argv);

	int ret_val = 0;
	if (args->isArgumentsValid())
	{
		FindSSRs find_ssrs(args);
		ret_val = find_ssrs.run();
	}
	else
	{
		args->printHelp();
		ret_val = 1;
	}

	delete args;

	return ret_val;
}
