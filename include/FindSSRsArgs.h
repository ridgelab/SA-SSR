/*
	Brandon Pickett
	FindSSRsArgs.h
*/

#pragma once

#include <cassert> // assert
#include <iostream>
#include <string>
#include <unordered_set>
#include <cstring> // strcmp
#include <sstream> // stringstream
#include <cstdlib> // exit

using namespace std;

class FindSSRsArgs
{
private:
	bool arguments_valid; // default true
	bool quick_and_dirty; // default true 
	//bool doing_blast; // default false
	uint32_t min_nucleotide_length; // default 16
	uint32_t min_ssr_length; // default 4
	uint32_t max_ssr_length; // default 8
	uint32_t min_sequence_length; // default 100
	uint32_t max_sequence_length; // default 1500
	uint32_t min_repeats; // default 1
	uint32_t max_repeats; // default none (uint32_t max)
	uint32_t num_threads; // default 1
	string usage_statement;
	string species_1_fasta_file_name;
	string species_2_blastdb;
	unordered_set<string>* enumerated_ssrs;
	string out_file_name;

public:
	FindSSRsArgs(int argc, char* argv[]);
	~FindSSRsArgs();
	bool isArgumentsValid() const;
	bool isQuickAndDirty() const;
	//bool doingBlast() const;
	uint32_t getMinNucleotideLength() const;
	uint32_t getMinSSRLength() const;
	uint32_t getMaxSSRLength() const;
	uint32_t getMinSequenceLength() const;
	uint32_t getMaxSequenceLength() const;
	uint32_t getMinRepeats() const;
	uint32_t getMaxRepeats() const;
	uint32_t getNumThreads() const;
	string getUsageStatement() const;
	string getSpecies1FastaFileName() const;
	//string& getSpecies2Blastdb();
	unordered_set<string>* getEnumeratedSSRs() const;
	string getOutFileName() const;
	void setQuickAndDirtyStatus(bool quick_and_dirty);
	void setSpecies1FastaFileName(string species_1_fasta_file_name);
	void addEnumeratedSSR(string enumerated_ssr);
	void addEnumeratedSSRs(string enumerated_ssrs_comma_separated);
	void setOutFileName(string out_file_name);
	void processArgs(int argc, char* argv[]);
	void printHelp() const;
	string toString() const;
};
