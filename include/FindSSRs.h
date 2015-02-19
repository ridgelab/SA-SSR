/*	
	Brandon Pickett
	FindSSRs.h
*/

#pragma once

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <string>
#include <sstream>

#include "FindSSRsArgs.h"
#include "Results.h"
#include "SingleResult.h"

using namespace std;

class FindSSRs
{
private:
	FindSSRsArgs* args;

public:
	FindSSRs(int argc, char* argv[]);
	~FindSSRs();
	uint32_t run();
	void findSSRsInFile();
	void findSSRsInSequence(const string &header, const string &sequence, ofstream &out_file);
	void findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP, ofstream &out_file);
	void printExtraInformation(const string &header, const string &sequence, const int *SA, const int *LCP, ofstream &out_file) const;
	//string blastAgainstSpecies2(const string &header, const string &sequence, const string &blastdb);
};
