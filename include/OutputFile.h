/*
	Brandon Pickett
	OutputFile.h
*/

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <semaphore.h>
#include <cstdlib>

using namespace std;

class OutputFile
{
private:
	ofstream out_file;
	sem_t lock;

private:
	void setUp();
	void setUp(const string &file_name);
	void writeHeader(const char* header);
	void writeHeader(const string &header);
	//void writeHeader( string &header);
	void writeHeaders(const vector<string> &headers);

public:
	OutputFile();
	//OutputFile(const OutputFile &output_file);
	OutputFile(const string &file_name);
	OutputFile(const string &file_name, const char* header);
	OutputFile(const string &file_name, const string &header);
	//OutputFile(const string &file_name, string &header);
	OutputFile(const string &file_name, const vector<string> &headers);
	~OutputFile();
	//sem_t getLock() const;
	//const ofstream* getOutFile() const;
	//void shallowCopy(const OutputFile &output_file);
	void changeFile(const string &file_name);
	void changeFile(const string &file_name, const vector<string> &headers);
	OutputFile& write(const char* output);
	OutputFile& write(const string &output);
	OutputFile& write(const vector<string> &output);
	OutputFile& write(stringstream &output);
	OutputFile& operator<<(const char* output);
	OutputFile& operator<<(const string &output);
	OutputFile& operator<<(const vector<string> &output);
	OutputFile& operator<<(stringstream &output);
	//OutputFile& operator=(const OutputFile &output_file);
};
