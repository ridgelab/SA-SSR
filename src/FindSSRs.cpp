/*	
	Brandon Pickett
	FindSSRs.cpp
*/

#include "../include/FindSSRs.h"
#include "../lib/sais-lite-lcp/sais.c"

using namespace std;

FindSSRs::FindSSRs(FindSSRsArgs* _args) : out_file(_args->getOutFileName(), "#Header\tSSR\tRepeats\tPosition\n")
{
	this->args = _args;
	this->num_threads = this->args->getNumThreads();
	sem_init(&(this->n),0,0);
	sem_init(&(this->e),0,(this->num_threads * 2));
}
FindSSRs::~FindSSRs()
{
	//sem_destroy(&(this->n));
	//sem_destroy(&(this->e));
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

	uint32_t error = this->makeThreads(); // set up consumers

	if (error) { return error; }

	try
	{
		this->processInput(); // produce (and consume if only main thread)
	}
	catch (string e)
	{
		cerr << e << endl;
		return 1;
	}

	this->joinAndForgetAllThreads(); // clean up consumers
	
	sleep(2); // give everything a chance to really finish.  For some reason, when I don't wait for 1-2 seconds, I miss one or two results in the output.

	//int nval, eval = 0;
	//sem_getvalue(&(this->n), &nval);
	//sem_getvalue(&(this->e), &eval);
	//cerr << "final n,e: " << nval << "," << eval << endl;
	
	return 0;
}
uint32_t FindSSRs::makeThreads()
{
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
	
	for (uint32_t i = 1; i < this->num_threads; i++)
	{
		pthread_t thread;
		this->threads.push_back(thread);

		if ( pthread_create(&thread,&tattr,&consume,(void*) this) != 0 )
		{
			perror("creating threads");
			return errno;
		}
	}

	pthread_attr_destroy(&tattr);

	return 0;
}
void FindSSRs::joinAndForgetAllThreads()
{
	for (uint32_t i = 0; i < this->threads.size(); i++)
	{
		long status = 0L;
		long* status_ptr = &status;

		int joinerr = 0;
		joinerr = pthread_join(this->threads[i],(void**) &status_ptr); //if ( pthread_join(this->threads[i],(void**) &status_ptr) != 0)
		if ( joinerr != 0 && joinerr != EINVAL)
		{
			perror("joining thread");
			//cerr << "joinerr,errno" << joinerr << "," << errno << endl;
		}
	}

	this->threads.clear();
}
void FindSSRs::processInput() // produce
{
	ifstream species1_in_file;
	species1_in_file.open(this->args->getSpecies1FastaFileName().c_str());

	string header = "";
	string line = "";
	while (getline(species1_in_file, line))
	{
		if ( (line[0] != '>') && (line.size() >= this->args->getMinSequenceLength()) && (line.size() <= this->args->getMaxSequenceLength()) )
		{
			line.append("$");
			switch (this->num_threads)
			{
				case 1:
					this->findSSRsInSequence(header, line);
					break;
				default:
					sem_wait(&(this->e)); // decrease num empty slots
					this->fasta_seqs.add(header, line); // fill a slot
					sem_post(&(this->n)); // increase num occupied slots
					break;
			}
		}
		else
		{
			header = line;
		}
	}

	species1_in_file.close();

	switch (this->num_threads)
	{
		case 1:
			break;
		default:
			this->fasta_seqs.dryUp(); // tell the FastaSequences object it will never recieve more input
			for (uint32_t i = 1; i < this->num_threads; i++)
			{
				sem_post(&(this->n)); // tell the consumers there's another thing to consume (which will be the stop code)
			}
			break;
	}
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
void* FindSSRs::consume(void* find_ssrs_vptr) // void* (*)(void* )
{
	bool go = true;
	FindSSRs* find_ssrs_ptr = (FindSSRs*) find_ssrs_vptr;
	string header;
	string sequence;
	while (go)
	{
		sem_wait(find_ssrs_ptr->getN()); // decrease num occupied slots
		if (!find_ssrs_ptr->fasta_seqs.get(header, sequence)) // take
		{
			sem_post(find_ssrs_ptr->getE()); // increase num empty slots
			find_ssrs_ptr->findSSRsInSequence(header, sequence); // consum (find the ssrs in the given sequence)
		}
		else
		{
			go = false;
		}
	}

	pthread_exit(NULL);
}
