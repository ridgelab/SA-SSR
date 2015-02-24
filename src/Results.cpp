/*
	Brandon Pickett
	Results.cpp
*/

//#define NDEBUG

#include "../include/Results.h"

using namespace std;

Results::Results(const uint32_t &sequence_length, unordered_set<string>* enumerated_ssrs)
{
	this->enumerated_ssrs = enumerated_ssrs;
	initialize(sequence_length);
}
Results::~Results()
{
	this->enumerated_ssrs = NULL; // the actual set will be destroyed by a different class
}
bool  Results::isStartPositionAvailableAt(uint32_t index) const
{
	return available_start_positions[index];	
}
bool Results::hasResults() const
{
	return (this->results.empty() == true) ? false : true;
}
void Results::initialize(const uint32_t &sequence_length)
{
	this->available_start_positions.assign(sequence_length, true);
	// shrink to fit???
}
//void  Results::processComparison(FindSSRsArgs* args, const string &sequence, const uint32_t &left, const uint32_t &right, const uint32_t &lcp)
bool  Results::processComparison(FindSSRsArgs* args, const string &sequence, const uint32_t &left, const uint32_t &right, const uint32_t &lcp)
{
	assert (lcp > 0); // Technically lcp == 0 is okay, but completely useless.

	uint32_t k = (uint32_t ) abs((int) (right - left));
	uint32_t r = lcp / k;
	uint32_t p = min(left, right);

	if (isSignificantSubSequence(args,k,r,p,lcp) == true)
	{
		addResult(sequence,k,r,p);
	}
	else if (r == 0) // a.k.a., r == 0 AND the subsequence was insignificant
	{
		return false; // a.k.a., don't stop, keep incrementing j to find repeats
	}
	
	return true; // a.k.a., (you've found something) OR (r > 0 AND the subsequence was insignificant)
}
bool Results::isSignificantSubSequence(const FindSSRsArgs* args, const uint32_t &k, const uint32_t &r, const uint32_t &p, const uint32_t &lcp) const
{
	if (r == 0) // check that there is actually a repeat occuring
	{
		return false;
	}
	else if (k * r + k < args->getMinNucleotideLength()) // check min nucleotide length
	{
		return false;
	}
	else if (k < args->getMinSSRLength()) // check min ssr length
	{
		return false;
	}
	else if (k > args->getMaxSSRLength()) // check max ssr length
	{
		return false;
	}
	else if ( (r + 1) < args->getMinRepeats() ) // check min repeats
	{
		return false;
	}
	else if ( (r + 1) > args->getMaxRepeats() ) // check max repeats
	{
		return false;
	}
	return true;
}
void  Results::addResult(const string &sequence, const uint32_t &k, const uint32_t &r, const uint32_t &p)
{
	if (enumeratedSSRFilter(sequence.substr(p,k)) == 1)
	{
		results.insert(SingleResult(k, r, p));
		//updateAvailableStartPositions(k, r, p);
	}
}
void  Results::updateAvailableStartPositions(uint32_t k, uint32_t r, uint32_t p)
{
	//for (uint32_t i = (p + k); i < available_start_positions.size() && i < (p + k * r); i++)
	for (uint32_t i = p; i < available_start_positions.size() && i < (p + k * r); i++)
	//for (uint32_t i = p; i < available_start_positions.size() && i < (p + k * (r + 1)); i++)
	{
		available_start_positions[i] = false;
	}
}
uint32_t Results::enumeratedSSRFilter(const string &ssr)
{
	if (enumerated_ssrs->empty() == true)
	{
		return 1;
	}
	return enumerated_ssrs->count(ssr);
}
//void Results::addBlastInfo(const string &blast_result)
//{
//	if (blast_result.size() == 0)
//	{
//		this->blast_result = "-";
//	}
//	else
//	{
//		bool first_time_through = true;
//		uint32_t i = 0;
//		while (i < blast_result.size())
//		{
//			if (first_time_through == false)
//			{
//				this->blast_result = this->blast_result + ",";
//			}
//			else
//			{
//				first_time_through = false;
//			}
//
//			while (blast_result[i] != '\t')
//			{
//				i++;
//			}
//			i++;
//
//			while (blast_result[i] != '\t')
//			{
//				this->blast_result = this->blast_result + blast_result[i];
//				i++;
//			}
//			
//			while (blast_result[i] != '\n')
//			{
//				i++;
//			}
//			i++;
//		}
//	}
//}
//void Results::writeToFile(const string &header, const string &sequence, ofstream &out_file, const bool &doing_blast)
void Results::writeToFile(const string &header, const string &sequence, OutputFile &out_file)
{
	string output;
	if (!results.empty())
	{
		for (set<SingleResult>::iterator itr = this->results.begin(); itr != this->results.end(); ++itr)
		{
			if (isStartPositionAvailableAt(itr->getP()) == true)
			{
				itr->writeToFile(header, sequence, output);
	//			if (doing_blast == true)
	//			{
	//				out_file << "\t" << this->blast_result;
	//			}
				output = output + "\n";

				updateAvailableStartPositions(itr->getK(), itr->getR(), itr->getP());
			}
		}
	}
	else
	{
		output = output + header.substr(1,string::npos) + "\t-\t0\t0\n";
	}
	out_file << output;
}
string  Results::toString()
{
	string temp;
	stringstream strm;
	temp = "Results\n=======\n";
	
	temp = temp + "Position Availability[ ";
	for (uint32_t i = 0; i < available_start_positions.size(); i++)
	{
		if (i > 0)
		{
			temp = temp + ", ";
		}
		
		if (available_start_positions[i] == true)
		{
 			temp = temp + "true";
		}
		else
		{
			temp = temp + "false";
		}
	}
	temp = temp + " ]\n";

	for (set<SingleResult>::iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		if (isStartPositionAvailableAt(itr->getP()) == true)
		{
			temp = temp + itr->toString() + "\n";
			updateAvailableStartPositions(itr->getK(), itr->getR(), itr->getP());
		}
	}

	return temp;
}
