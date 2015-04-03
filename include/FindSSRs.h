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
#include "ProgressMeter.h"

using namespace std;

class FindSSRs
{
private:
	//FindSSRsArgs args;
	FindSSRsArgs* args;
	FastaSequences fasta_seqs; // thread-safe data structure
	vector<pthread_t> threads;
	uint32_t num_threads;
	OutputFile out_file; // thread-safe class
	sem_t n; // full space (for the consumer to take) in the buffer...if n increases/decreases, e should decrease/increase
	sem_t e; // empty space (for the producer to fill) in the buffer...if e increases/decreases, n should decrease/increase
	ProgressMeter progress_bar;

public:
	FindSSRs(FindSSRsArgs* _args);
	~FindSSRs();
	sem_t* getN() const;
	sem_t* getE() const;
	uint32_t run();
	uint32_t makeThreads();
	void joinAndForgetAllThreads();
	void processInput();
	void processSequence(const string &header, string &sequence);
	void findSSRsInSequence(const string &header, const string &sequence);
	void findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP);
	void printExtraInformation(const string &header, const string &sequence, const int *SA, const int *LCP);
	static void* consume(void* find_ssrs);
};
