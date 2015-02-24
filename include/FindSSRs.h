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
#include <semaphore.h>
#include <pthread.h>

#include "FindSSRsArgs.h"
#include "Results.h"
#include "SingleResult.h"
#include "FastaSequences.h"
#include "OutputFile.h"

using namespace std;

class FindSSRs
{
private:
	FindSSRsArgs* args;
	FastaSequences fasta_seqs; // thread-safe data structure
	vector<pthread_t> threads;
	uint32_t num_threads;
	OutputFile out_file; // thread-safe class
	bool finished; // reading in the fasta file
	sem_t n; // signals number of waiting threads / available sequences
	sem_t e; // signals space in buffer for acceptance of new sequences

public:
	//FindSSRs(int argc, char* argv[]);
	FindSSRs(FindSSRsArgs* _args);
	~FindSSRs();
	bool isFinished() const;
	bool isFastaSeqsEmpty();
	sem_t* getN() const;
	sem_t* getE() const;
	uint32_t run();
	void makeThreads();
	void joinAndForgetAllThreads();
	void produceFromFasta();
	void findSSRsInSequence(const string &header, const string &sequence);
	void findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP);
	void printExtraInformation(const string &header, const string &sequence, const int *SA, const int *LCP);
	//string blastAgainstSpecies2(const string &header, const string &sequence, const string &blastdb);
	static void* consume(void* find_ssrs);
};
