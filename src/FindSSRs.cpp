/*	
	Brandon Pickett
	FindSSRs.cpp
*/

#include "../include/FindSSRs.h"
#include "../lib/sais-lite-lcp/sais.c"

using namespace std;

FindSSRs::FindSSRs(int argc, char* argv[])
{
	this->args = new FindSSRsArgs(argc, argv);
}
FindSSRs::~FindSSRs()
{
	delete this->args;
}
uint32_t FindSSRs::run()
{
	if (this->args->isArgumentsValid() == false)
	{
		cerr << "\n" << this->args->getUsageStatement() << endl << endl;
		return 1;
	}

	try
	{
		findSSRsInFile();
	}
	catch (string e)
	{
		cerr << e << endl;
		return 1;
	}

	return 0;
}
void FindSSRs::findSSRsInFile()
{
	ifstream species1_in_file;
	species1_in_file.open(this->args->getSpecies1FastaFileName().c_str());

	ofstream out_file;
	out_file.open(this->args->getOutFileName().c_str());

//	if (this->args->doingBlast() == false) // We're not doing a blast
//	{
//		out_file << "#Header\tSSR\tRepeats\tPosition" << endl;
//	}
//	else // We're doing a blast
//	{
//		out_file << "#Sp-1-Head\tSSR\tRepeats\tPosition\tSp-2-Head" << endl;
//	}

	out_file << "#Header\tSSR\tRepeats\tPosition" << endl;

	string header = "";

	string line;
	while (getline(species1_in_file, line))
	{
		if ( (line[0] != '>') && (line.size() >= this->args->getMinSequenceLength()) && (line.size() <= this->args->getMaxSequenceLength()) )
		{
			findSSRsInSequence(header, line + "$", out_file);
		}
		else
		{
			header = line;
		}
	}

	species1_in_file.close();
	out_file.close();
}

void FindSSRs::findSSRsInSequence(const string &header, const string &sequence, ofstream &out_file)
{
	// prepare all input for sais (saca & lcpca)
	//cout << "sequence size: " << sequence.size() << endl; // <-------------- DELETE THIS LINE!!!
	int SA[sequence.size()];
	int LCP[sequence.size()];
	//cout << "sequence size: " << sequence.size() << endl; // <-------------- DELETE THIS LINE!!!
	//cout.flush();
	for (uint32_t i = 0; i < sequence.size(); i++)
	{
		SA[i] = 0;
		LCP[i] = 0;
	}

	// run sais to generate the SA and LCP arrays
	if (sais(reinterpret_cast<const unsigned char*>(sequence.c_str()), SA, LCP, sequence.size()) == 0) // returns 0 if no error occured, non-zero otherwise //int sais(const unsigned char *T, int *SA, int *LCP, int n)
	{
		findSSRsInSA(header, sequence, SA, LCP, out_file);
	}
	else
	{
		throw "Sais returned with an error!!";
	}
}

void FindSSRs::findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP, ofstream &out_file)
{
	Results results = Results(sequence.size(), this->args->getEnumeratedSSRs());
	for (uint32_t i = 0; i < sequence.size() - 1; i++)
	{
		bool stop = true;	
		uint32_t j = i + 1;
		uint32_t lcp = LCP[j];
		do
		{
			lcp = ((uint32_t) LCP[j] < lcp) ? LCP[j] : lcp;
			if ( lcp >= this->args->getMinSSRLength() ) // MinSSRLength (a.k.a., min k value) will always be >= 1 (thus > 0)
			//if (lcp > 0)
			{
				//results.processComparison(this->args, sequence, SA[i], SA[j], lcp);
				stop = results.processComparison(this->args, sequence, SA[i], SA[j], lcp);
			}
			else
			{
				break;
			}
			j++;
		}
		//while (j < sequence.size() && this->args->isQuickAndDirty() == false);
		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (stop == false) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 3)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 4)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 5)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 6)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 7)) ) );
	}	

//	if (this->args->doingBlast() == true && results.hasResults() == true)
//	{
//		results.addBlastInfo(blastAgainstSpecies2(header, sequence, this->args->getSpecies2Blastdb()));
//	}

	//results.writeToFile(header, sequence, out_file, this->args->doingBlast());
	results.writeToFile(header, sequence, out_file);

	//printExtraInformation(header, sequence, SA, LCP, out_file);
}
void FindSSRs::printExtraInformation(const string &header, const string &sequence, const int *SA, const int *LCP, ofstream &out_file) const
{
	out_file << "header: " << header << endl;
	out_file << "sequence: " << sequence << endl;
	out_file << " SA: ";
	for (uint32_t i = 0; i < sequence.size(); i++)
	{
		string temp;
		stringstream strm;
		strm << SA[i];
		strm >> temp;
		out_file << temp << " ";
	}
	out_file << "\nLCP: - ";
	for (uint32_t i = 1; i < sequence.size(); i++)
	{
		string temp;
		stringstream strm;
		strm << LCP[i];
		strm >> temp;
		out_file << temp << " ";
	}
	out_file << endl << endl;
}
//string FindSSRs::blastAgainstSpecies2(const string &header, const string &sequence, const string &blastdb)
//{
//	string s;
//	char* cmd = (char*) ("echo -e \"" + header + "\n" + sequence.substr(0,sequence.size() - 1) + "\" | blastn -db " + blastdb + " -query - -outfmt 6").c_str();
//	FILE* pipe = popen( cmd, "r" );
//	
//	if ( !pipe )
//	{
//		throw "ERROR: Pipe error.";
//	}
//
//	char buffer[ 256 ];
//	while( !feof( pipe ) )
//	{
//		if( fgets( buffer, 256, pipe ) != NULL )
//		{
//			s = s + buffer;
//		}
//	}
//	pclose( pipe );
//
//	return s;
//}
