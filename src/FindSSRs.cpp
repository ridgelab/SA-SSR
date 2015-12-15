/*	
	Brandon Pickett
	FindSSRs.cpp
*/

#include "../include/FindSSRs.h"
#include "../lib/sais-lite-lcp/sais.c"

using namespace std;

static
uint32_t calculateDataSizeFromFasta(ifstream &fasta);

FindSSRs::FindSSRs(FindSSRsArgs* _args) : out_file(_args->getOutFileName(), _args->getOutFileHeader())
{
	this->args = _args;
	this->num_threads = this->args->getNumThreads();
	sem_init(&(this->n),0,0);
	sem_init(&(this->e),0,(this->num_threads * 2));
	sem_init(&(this->d),0,1);
	sem_init(&(this->s),0,0);
	this->finished_threads = 1;
	
	this->progress_bar = ProgressMeter();
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
sem_t* FindSSRs::getD() const
{
	return (sem_t*) &(this->d);
}
sem_t* FindSSRs::getS() const
{
	return (sem_t*) &(this->s);
}
uint32_t FindSSRs::getFinishedThreadsCount() const
{
	//uint32_t temp;
	//sem_wait(&(this->d)); // acquire lock for finished threads
	//temp = this->finished_threads;
	//sem_post(&(this->d)); // release lock for finished threads
	//return temp;
	return this->finished_threads;
}
void FindSSRs::incrementFinishedThreads()
{
	sem_wait(&(this->d)); // acquire lock for finished threads
	++this->finished_threads;
	sem_post(&(this->d)); // release lock for finished threads
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

	//cerr << "About to start joining threads..." << endl;
	//this->joinAndForgetAllThreads(); // clean up consumers
	
	while (this->finished_threads < this->num_threads)
	{
		//cerr << "Waiting on threads! (" << this->finished_threads << "/" << this->num_threads << ")" << endl;
		sem_wait(&(this->s)); // wait for signal from a thread
	}
	
	//int nval, eval, dval, sval = 0;
	//sem_getvalue(&(this->n), &nval);
	//sem_getvalue(&(this->e), &eval);
	//sem_getvalue(&(this->d), &dval);
	//sem_getvalue(&(this->s), &sval);
	//cerr << "final n,e,d,s: " << nval << "," << eval << "," << dval << "," << sval << endl;
	
	return 0;
}
uint32_t FindSSRs::makeThreads()
{
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE); // Maybe omit because this is already default?
	
	for (uint32_t i = 1; i < this->num_threads; ++i)
	{
		pthread_t thread;
		this->threads.push_back(thread);

		//if ( pthread_create(&thread,NULL,&consume,(void*) this) != 0 )
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
	for (uint32_t i = 0; i < this->threads.size(); ++i)
	{
		//cerr << "Attempting to join thread #" << i << " with ID=" << this->threads[i] << endl;
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

	// init (set the data_size of) the progress bar
	this->progress_bar.initialize(calculateDataSizeFromFasta(species1_in_file));

	string header = "";
	string sequence = "";
	string line = "";
	while (getline(species1_in_file, line))
	{
		if (line.size() > 0)
		{
			if (line[0] != '>')
			{
				for (uint32_t i = 0; i < line.size(); ++i)
				{
					//sequence = sequence + (char) toupper(line[i]);
					sequence += (char) toupper(line[i]);
				}
			}
			else
			{
				this->processSequence(header,sequence);

				header = line;
				sequence = "";
			}
		}
		
		//if ( (line[0] != '>') && (line.size() >= this->args->getMinSequenceLength()) && (line.size() <= this->args->getMaxSequenceLength()) )
		//{
		//	line.append("$");
		//	switch (this->num_threads)
		//	{
		//		case 1:
		//			this->findSSRsInSequence(header, line);
		//			break;
		//		default:
		//			sem_wait(&(this->e)); // decrease num empty slots
		//			this->fasta_seqs.add(header, line); // fill a slot
		//			sem_post(&(this->n)); // increase num occupied slots
		//			break;
		//	}
		//}
		//else
		//{
		//	header = line;
		//}
	}
	
	this->processSequence(header,sequence); // catch the last sequence

	species1_in_file.close();

	switch (this->num_threads)
	{
		case 1:
			break;
		default:
			this->fasta_seqs.dryUp(); // tell the FastaSequences object it will never recieve more input
			for (uint32_t i = 1; i < this->num_threads; ++i)
			{
				sem_post(&(this->n)); // tell the consumers there's another thing to consume (which will be the stop code), aka, it will return a stop code.
			}
			break;
	}
}

void FindSSRs::processSequence(const string &header, string &sequence)
{
	//uint32_t seq_size = sequence.size();

	if ( (sequence.size() >= this->args->getMinSequenceLength()) && (sequence.size() <= this->args->getMaxSequenceLength()) )
	{
		vector<uint32_t> starts;
		vector<uint32_t> sizes;

		if (!this->args->isCharsToIgnore())
		{
			starts.push_back(0);
			sizes.push_back(sequence.size());
		}
		else
		{
			uint32_t ignored_chars_count = 0;
			splitStringOnIgnoredChars(starts, sizes, sequence, ignored_chars_count);
			this->progress_bar.updateProgress(ignored_chars_count);
		}
		
		for (uint32_t i = 0; i < starts.size(); ++i)
		{
			string sub_seq = sequence.substr(starts[i], sizes[i]);
			sub_seq.append("$");

			switch (this->num_threads)
			{
				case 1:
					this->findSSRsInSequence(header, sub_seq, starts[i]);
					break;
				default:
					sem_wait(&(this->e)); // decrease num empty slots
					this->fasta_seqs.add(header, sub_seq, starts[i]); // fill a slot
					sem_post(&(this->n)); // increase num occupied slots
					break;
			}
		}
		
		//sequence.append("$");
		//switch (this->num_threads)
		//{
		//	case 1:
		//		this->findSSRsInSequence(header, sequence);
		//		break;
		//	default:
		//		sem_wait(&(this->e)); // decrease num empty slots
		//		this->fasta_seqs.add(header, sequence); // fill a slot
		//		sem_post(&(this->n)); // increase num occupied slots
		//		break;
		//}
	}
	//this->progress_bar.updateProgress(seq_size);
	//this->progress_bar.updateProgress(sequence.size()); //this is not the ideal place for this...it reports completion before processing actually happens..
}

void FindSSRs::findSSRsInSequence(const string &header, const string &sequence, uint32_t ignore_chars_offset)
{
	int SA[sequence.size()];
	int LCP[sequence.size()];
	
	for (uint32_t i = 0; i < sequence.size(); ++i)
	{
		SA[i] = 0;
		LCP[i] = 0;
	}

	// run sais to generate the SA and LCP arrays
	if (sais(reinterpret_cast<const unsigned char*>(sequence.c_str()), SA, LCP, sequence.size()) == 0) // returns 0 if no error occured, non-zero otherwise //int sais(const unsigned char *T, int *SA, int *LCP, int n)
	{
		findSSRsInSA(header, sequence, SA, LCP, ignore_chars_offset);
	}
	else
	{
		throw "Sais returned with an error!!";
	}
	
	this->progress_bar.updateProgress(sequence.size() - 1); // minus 1 for the '$' at the end
}

//void FindSSRs::findSSRsInSequence(const string &header, const string &sequence)
//{
//	//cerr << header << endl; // <------------------------- DELETE THIS LINE!!!
//	vector<uint32_t> starts;
//	vector<uint32_t> sizes;
//
//	if (!this->args->isCharsToIgnore())
//	{
//		starts.push_back(0);
//		sizes.push_back(sequence.size());
//	}
//	else
//	{
//		splitStringOnIgnoredChars(starts, sizes, sequence);
//	}
//
//	//for (uint32_t i = 0; i < starts.size(); ++i)
//	//{
//	//	cerr << "starts " << i << ": " << starts[i] << "\tsizes " << i << ": " << sizes[i] << endl;
//	//}
//
//	vector<Results> res_vec;
//
//	for (uint32_t j = 0; j < starts.size(); ++j)
//	{
//		// prepare all input for sais (saca & lcpca)
//		int SA[sizes[j]];
//		int LCP[sizes[j]];
//		
//		for (uint32_t i = 0; i < sizes[j]; ++i)
//		{
//			SA[i] = 0;
//			LCP[i] = 0;
//		}
//
//		//int sais(const unsigned char *T, int *SA, int *LCP, int n)
//		// run sais to generate the SA and LCP arrays
//		string temp_seq = sequence.substr(starts[j], sizes[j]);
//		temp_seq.append("$");
//		//if (sais(reinterpret_cast<const unsigned char*>(sequence.c_str()) + starts[j], SA, LCP, sizes[j]) == 0) // returns 0 if no error occured, non-zero otherwise //int sais(const unsigned char *T, int *SA, int *LCP, int n)
//		if (sais(reinterpret_cast<const unsigned char*>(temp_seq.c_str()), SA, LCP, temp_seq.size()) == 0) // returns 0 if no error occured, non-zero otherwise //int sais(const unsigned char *T, int *SA, int *LCP, int n)
//		{
//			// TODO -- Make sure the pointer arithmetic is valid
//			//findSSRsInSA(header, sequence.substr(starts[j], sizes[j]), SA, LCP, res_vec); // TODO -- make sure I don't need to create the string object BEFORE passing it in
//			findSSRsInSA(header, temp_seq, SA, LCP, res_vec);
//		}
//		else
//		{
//			throw "Sais returned with an error!!";
//		}
//	}
//
//	for (uint32_t i = 0; i < res_vec.size(); ++i)
//	{
//		res_vec[i].writeToFile(this->args->isIncludeZero(), this->args->isAdditionalOutput(),header, sequence.substr(starts[i], sizes[i]), this->out_file, starts[i]); // TODO -- make sure I don't need to create the string object BEFORE passing it in
//	}
//}

//void FindSSRs::findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP)
//void FindSSRs::findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP, vector<Results> &res_vec)
void FindSSRs::findSSRsInSA(const string &header, const string &sequence, const int *SA, const int *LCP, uint32_t ignore_chars_offset)
{
	Results results = Results(sequence.size(), this->args->getEnumeratedSSRs());
	for (uint32_t i = 0; i < sequence.size() - 1; ++i)
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
		while ( (j < sequence.size()) && ( this->args->isExhaustive() || !stop ) );
		//while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (stop == false) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 3)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 4)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 5)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 6)) ) );
//		while ( (j < sequence.size()) && ( (this->args->isQuickAndDirty() == false) || (j < (i + 7)) ) );
	}	

	//results.writeToFile(this->args->isIncludeZero(), this->args->isAdditionalOutput(),header, sequence, this->out_file);
	//res_vec.push_back(results);
	results.writeToFile(this->args->isIncludeZero(), this->args->isAdditionalOutput(),header, sequence, this->out_file, ignore_chars_offset, this->args->isWriteOverlaps());

	//printExtraInformation(header, sequence, SA, LCP, out_file);
}
void FindSSRs::splitStringOnIgnoredChars(vector<uint32_t> &starts, vector<uint32_t> &sizes, const string &sequence, uint32_t &actually_ignored_chars)
{
	//uint32_t effective_seq_size = sequence.size() - 1; // to account for the '$'
	//cerr << "seq: " << sequence << endl;
	// Find the first start position
	uint32_t i = 0;
	//while (i < effective_seq_size && this->args->getIgnoreChars()->count(sequence[i]))
	while (i < sequence.size() && this->args->getIgnoreChars()->count(sequence[i]))
	{
		++actually_ignored_chars;
		++i;
	}

	//cerr << "skipped bad. pos: " << i << endl;

	// Check to make sure we still have sequence left
	//if (i >= effective_seq_size)
	if (i >= sequence.size())
	{
		return;
	}

	// Push back the start position
	starts.push_back(i);
	
	//cerr << "added start. pos: " << i << endl;

	// Find the rest of the splits
	//while (i < effective_seq_size)
	while (i < sequence.size())
	{
		// We assume the previous character was valid at this point.  Meaning, we're looking
		// for an invalid character to add to the sizes vector.  The sizes vector should be
		// one smaller than the starts vector.
		
		if (this->args->getIgnoreChars()->count(sequence[i]))
		{
			// We found an invalid character and can append the size tot he sizes vector
			sizes.push_back(i - starts[starts.size() - 1]);
			//sizes.push_back(i - starts[starts.size() - 1] + 1); // add 1 to account for the trailing '$'
			//cerr << "added size. pos: " << i << "\tsize: " << i - starts[starts.size() - 1] << endl;
			++actually_ignored_chars;
			++i;

			// Now, let's skip all the other invalid characters adjacent to this one
			//while (i < effective_seq_size && this->args->getIgnoreChars()->count(sequence[i]))
			while (i < sequence.size() && this->args->getIgnoreChars()->count(sequence[i]))
			{
				//cerr << "skipping 'bad'. pos: " << i << endl;
				++actually_ignored_chars;
				++i;
			}
			
			//cerr << "done skipping 'bad'. pos: " << i << endl;

			// If there is still sequence left, i is on a valid character
			//if (i < effective_seq_size)
			if (i < sequence.size())
			{
				starts.push_back(i);
				//cerr << "added start. pos: " << i << endl;
			}
		}

		++i;
	}

	// we've gone through the entire sequence now.  Chances are, we still need to insert size into sizes for the last stretch
	if (starts.size() != sizes.size())
	{
		//sizes.push_back(effective_seq_size - starts[starts.size() - 1]);
		sizes.push_back(sequence.size() - starts[starts.size() - 1]);
		//sizes.push_back(sequence.size() - starts[starts.size() - 1] + 1); // add 1 to account for the trailing '$'
	}

	// Sanity check
	if (starts.size() != sizes.size())
	{
		cerr << "\n\tERROR! starts and sizes are not the same size!\n" << endl;
		exit(1);
	}

	//for (uint32_t i = 0; i < starts.size(); ++i)
	//{
	//	cerr << "starts " << i << ": " << starts[i] << "\tsizes " << i << ": " << sizes[i] << endl;
	//}
}
void FindSSRs::printExtraInformation(const string &header, const string &sequence, const int *SA, const int *LCP) // not thread safe!!
{
	out_file << "header: " << header << "\n";
	out_file << "sequence: " << sequence << "\n";
	out_file << " SA: ";
	for (uint32_t i = 0; i < sequence.size(); ++i)
	{
		string temp;
		stringstream strm;
		strm << SA[i];
		strm >> temp;
		out_file << temp << " ";
	}
	out_file << "\nLCP: - ";
	for (uint32_t i = 1; i < sequence.size(); ++i)
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
	uint32_t ignore_chars_offset = 0;
	while (go)
	{
		sem_wait(find_ssrs_ptr->getN()); // decrease num occupied slots
		if (!find_ssrs_ptr->fasta_seqs.get(header, sequence, ignore_chars_offset)) // take
		{
			sem_post(find_ssrs_ptr->getE()); // increase num empty slots
			find_ssrs_ptr->findSSRsInSequence(header, sequence, ignore_chars_offset); // consume (find the ssrs in the given sequence)
		}
		else
		{
			go = false;
		}
	}
	
	//cout << "I'm exiting!" << endl;
	//long* status = 0;
	//pthread_exit((void*) status);
	find_ssrs_ptr->incrementFinishedThreads();
	sem_post(find_ssrs_ptr->getS()); // send a signal to main thread
	return NULL;
}


// --------------- STATIC FUNCTIONS ------------------------------ ||

static
uint32_t calculateDataSizeFromFasta(ifstream &fasta)
{
	uint32_t size = 0;
	string line;
	uint32_t pos = fasta.tellg();

	fasta.seekg(0,fasta.beg);

	while(getline(fasta,line))
	{
		if (line.size() > 0)
		{
			if (line[0] != '>')
			{
				//size = size + line.size();
				size += line.size();
			}
		}
	}

	fasta.clear();
	fasta.seekg(pos, fasta.beg);
	return size;
}

//void FindSSRs::findSSRsInSequence(const string &header, const string &sequence)
//{
//	// prepare all input for sais (saca & lcpca)
//	//cout << "sequence size: " << sequence.size() << endl; // <-------------- DELETE THIS LINE!!!
//	int SA[sequence.size()];
//	int LCP[sequence.size()];
//	//cout << "sequence size: " << sequence.size() << endl; // <-------------- DELETE THIS LINE!!!
//	//cout.flush();
//	for (uint32_t i = 0; i < sequence.size(); ++i)
//	{
//		SA[i] = 0;
//		LCP[i] = 0;
//	}
//
//	// run sais to generate the SA and LCP arrays
//	if (sais(reinterpret_cast<const unsigned char*>(sequence.c_str()), SA, LCP, sequence.size()) == 0) // returns 0 if no error occured, non-zero otherwise //int sais(const unsigned char *T, int *SA, int *LCP, int n)
//	{
//		findSSRsInSA(header, sequence, SA, LCP);
//	}
//	else
//	{
//		throw "Sais returned with an error!!";
//	}
//}
