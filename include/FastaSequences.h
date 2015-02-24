/*
	Brandon Pickett
	FastaSeqeunces.h
*/

#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <cstddef>
#include <semaphore.h>

using namespace std;

class FastaSequences
{
private:
	queue<string> headers;
	queue<string> sequences;
	sem_t lock;

public:
	FastaSequences();
	~FastaSequences();
	bool empty();
	void add(string header, string sequence);
	void get(string &header, string &sequence);
	//string toJSON();
};
