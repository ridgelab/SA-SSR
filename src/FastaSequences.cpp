/*
	Brandon Pickett
	FastaSequences.cpp
*/

#include "../include/FastaSequences.h"

using namespace std;

FastaSequences::FastaSequences()
{
	sem_init(&(this->lock),0,1);
}
FastaSequences::~FastaSequences()
{
	return;
}
bool FastaSequences::empty()
{
	sem_wait(&(this->lock));
	bool empty = this->headers.empty();
	sem_post(&(this->lock));
	return empty;
}
void FastaSequences::add(string header, string sequence)
{
	sem_wait(&(this->lock));
	this->headers.push(header);
	this->sequences.push(sequence);
	sem_post(&(this->lock));
}
void FastaSequences::get(string &header, string &sequence)
{
	sem_wait(&(this->lock));
	if (this->headers.size() > 0)
	{
		header = this->headers.front();
		sequence = this->sequences.front();
		this->headers.pop();
		this->sequences.pop();
	}
	else
	{
		header = nullptr;
		sequence = nullptr;
	}
	sem_post(&(this->lock));
}
/*
string FastaSequences::toJSON()
{
	sem_wait(&(this->lock));
	
	string output = "FastaSequences: {\n\theaders: [";
	for (uint32_t i = 0; i < this->headers.size(); i++)
	{
		output = output + "\t\t\n" + this->headers[i];
		if (i < this->headers.size() - 1)
		{
			output = output + ","
		}
	}
	output = output + "\n\t],\n\tsequences: [";
	for (uint32_t i = 0; i < this->sequences.size(); i++)
	{
		output = output + "\t\t\n" + this->sequences[i];
		if (i < this->sequences.size() - 1)
		{
			output = output + ","
		}
	}
	output = output + "\n\t]\n}";

	sem_post(&(this->lock));

	return output;
}
*/
