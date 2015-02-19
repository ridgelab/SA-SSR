/*
	Brandon Pickett
	Results.h
*/

#pragma once

#include <cassert> // assert
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <cmath> // abs
#include <algorithm> // min

#include "SingleResult.h"
#include "FindSSRsArgs.h"

using namespace std;

class Results
{
private:
	set<SingleResult> results;
	vector<bool> available_start_positions;
	unordered_set<string>* enumerated_ssrs;
	//string blast_result;

public:
	Results(const uint32_t &sequence_length, unordered_set<string>* enumerated_ssrs);
	~Results();
	bool isStartPositionAvailableAt(uint32_t index) const;
	bool hasResults() const;
	void initialize(const uint32_t &sequence_length);
	//void processComparison(FindSSRsArgs* args, const string &sequence,const uint32_t &left, const uint32_t &right, const uint32_t &lcp);
	bool processComparison(FindSSRsArgs* args, const string &sequence,const uint32_t &left, const uint32_t &right, const uint32_t &lcp);
	bool isSignificantSubSequence(const FindSSRsArgs* args, const uint32_t &k, const uint32_t &r, const uint32_t &p, const uint32_t &lcp) const;
	void addResult(const string &sequence, const uint32_t &k, const uint32_t &r, const uint32_t &p);
	void updateAvailableStartPositions(uint32_t k, uint32_t r, uint32_t p);
	uint32_t enumeratedSSRFilter(const string &ssr);
	//void addBlastInfo(const string &blast_result);
	//void writeToFile(const string &header, const string &sequence, ofstream &out_file, const bool &doing_blast);
	void writeToFile(const string &header, const string &sequence, ofstream &out_file);
	string toString(); 
};
