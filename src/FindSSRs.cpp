/*	
	Brandon Pickett
	FindSSRs.cpp
*/

#include "../include/FindSSRs.h"
#include "../lib/sais-lite-lcp/sais.c"

using namespace std;
//void* consume(void* find_ssrs_vptr);

//FindSSRs::FindSSRs(int argc, char* argv[])
FindSSRs::FindSSRs(FindSSRsArgs* _args) : out_file(_args->getOutFileName(), "#Header\tSSR\tRepeats\tPosition\n")
{
	this->args = _args;
	this->fasta_seqs = FastaSequences();
	this->num_threads = this->args->getNumThreads();
	//string name = this->args->getOutFileName();
	//string header = "#Header\tSSR\tRepeats\tPosition\n";
	//this->out_file(this->args->getOutFileName(),"#Header\tSSR\tRepeats\tPosition\n");
	//this->out_file(this->args->getOutFileName(),header);
	//this->out_file = OutputFile(this->args->getOutFileName(),header);
	this->finished = false;
	sem_init(&(this->n),0,0);
	sem_init(&(this->e),0,(this->num_threads * 1.5));
}
FindSSRs::~FindSSRs()
{
	delete this->args;
}
bool FindSSRs::isFinished() const
{
	return this->finished;
}
bool FindSSRs::isFastaSeqsEmpty()
{
	return this->fasta_seqs.empty();;
}
sem_t* FindSSRs::getN() const
{
	return (sem_t*) &(this->n);
}
sem_t* FindSSRs::getE() const
{
	return (sem_t*) &(this->e);
}
uint32_t FindSSRs::run()
{
	if (this->args->isArgumentsValid() == false)
	{
		cerr << "\n" << this->args->getUsageStatement() << endl << endl;
		return 1;
	}

	makeThreads();

	try
	{
		produceFromFasta();
	}
	catch (string e)
	{
		cerr << e << endl;
		return 1;
	}

	//string header;
	//string sequence;

	//while (!this->fasta_seqs.empty())
	//{
	//	//sem_wait(&(this->n));
	//	this->fasta_seqs.get(header,sequence);
	//	this->findSSRsInSequence(header, sequence);
	//	sem_post(&(this->e));
	//	header.clear();
	//	sequence.clear();
	//}

	this->joinAndForgetAllThreads();

	return 0;
}
void FindSSRs::makeThreads()
{
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
	
	FindSSRs* find_ssrs_ptr = this;
	for (uint32_t i = 0; i < this->num_threads; i++)
	{
		pthread_t thread;
		this->threads.push_back(thread);

		if ( pthread_create(&thread,&tattr,&consume,(void*) find_ssrs_ptr) != 0 )
		{
			perror("creating threads");
			exit(-1);
		}
	}

	pthread_attr_destroy(&tattr);
}
void FindSSRs::joinAndForgetAllThreads()
{
	cerr << "joining threads" << endl;
	for (uint32_t i = 0; i < this->threads.size(); i++)
	{
		cerr << "inside for loop" << endl;
		if ( pthread_join(this->threads[i],NULL) != 0)
		{
			cerr << "inside if" << endl;
			perror("joining threads");
			cerr << "end if" << endl;
		}
		cerr << "end for loop" << endl;
	}
	cerr << "after for loop" << endl;

	//this->threads.clear();
	cerr << "after clear" << endl;
}
void FindSSRs::produceFromFasta()
{
	ifstream species1_in_file;
	species1_in_file.open(this->args->getSpecies1FastaFileName().c_str());

//	if (this->args->doingBlast() == false) // We're not doing a blast
//	{
//		out_file << "#Header\tSSR\tRepeats\tPosition" << endl;
//	}
//	else // We're doing a blast
//	{
//		out_file << "#Sp-1-Head\tSSR\tRepeats\tPosition\tSp-2-Head" << endl;
//	}

	string header = "";

	string line;
	while (getline(species1_in_file, line))
	{
		if ( (line[0] != '>') && (line.size() >= this->args->getMinSequenceLength()) && (line.size() <= this->args->getMaxSequenceLength()) )
		{
			sem_wait(&(this->e)); 
			this->fasta_seqs.add(header, line + "$");
			sem_post(&(this->n));
		}
		else
		{
			header = line;
		}
	}

	species1_in_file.close();

	this->finished = true;
}

void FindSSRs::findSSRsInSequence(const string &header, const string &sequence)
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
		findSSRsInSA(header, sequence, SA, LCP);
	}
	else
	{
		throw "Sais returned with an error!!";
	}
}

void FindSSRs::findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP)
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
	// TODO -- Protect the out_file with a semaphore
	results.writeToFile(header, sequence, out_file);

	//printExtraInformation(header, sequence, SA, LCP, out_file);
}
void FindSSRs::printExtraInformation(const string &header, const string &sequence, const int *SA, const int *LCP) // not thread safe!!
{
	out_file << "header: " << header << "\n";
	out_file << "sequence: " << sequence << "\n";
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
	out_file << "\n" << "\n";
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
void* FindSSRs::consume(void* find_ssrs_vptr)
//void* consume(void* find_ssrs_vptr)
{
	FindSSRs* find_ssrs_ptr = (FindSSRs*) find_ssrs_vptr;
	string header;
	string sequence;
	//while (!((FindSSRs*) find_ssrs)->isFinished())
	while (!find_ssrs_ptr->isFinished() || !find_ssrs_ptr->isFastaSeqsEmpty())
	{
		//sem_wait((((FindSSRs*) find_ssrs)->getN()));
		sem_wait(find_ssrs_ptr->getN());

		// get the header and sequence
		//((FindSSRs*) find_ssrs)->fasta_seqs.get(header, sequence);
		find_ssrs_ptr->fasta_seqs.get(header, sequence);

		//if (header != nullptr && sequence != nullptr)
		if (header != "" && sequence != "")
		{
			// find the ssrs in the file
			//((FindSSRs*) find_ssrs)->findSSRsInSequence(header, sequence);
			find_ssrs_ptr->findSSRsInSequence(header, sequence);
		}

		//sem_post((((FindSSRs*) find_ssrs)->getE()));
		sem_post(find_ssrs_ptr->getE());
		
		header.clear();
		sequence.clear();
	}

	pthread_exit(NULL);

	//return NULL;
}
