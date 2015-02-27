/*
	Brandon Pickett
	FindSSRsArgs.cpp
*/

//#define NDEBUG

#include "../include/FindSSRsArgs.h"

using namespace std;

FindSSRsArgs::FindSSRsArgs(int argc, char* argv[])
{
	this->arguments_valid = true;
	this->ehaustive = false;
	//this->doing_blast = false;
	this->min_nucleotide_length = 16;
	this->min_ssr_length = 4;
	this->max_ssr_length = 8;
	this->min_sequence_length = 100;
	this->max_sequence_length = 1500;
	this->min_repeats = 1;
	this->max_repeats = 4294967295; // max uint32_t size (2^32 - 1)
	this->num_threads = 1;
	this->usage_statement = "findSSRs [options] <input-file.fasta> <output-file>";
	this->species_1_fasta_file_name = "";
	this->enumerated_ssrs = new unordered_set<string>;
	this->out_file_name = "";
	
	processArgs(argc, argv);
}
FindSSRsArgs::FindSSRsArgs(const FindSSRsArgs &args)
{
	this->deepCopy(args);
}
void FindSSRsArgs::deepCopy(const FindSSRsArgs &args)
{
	this->arguments_valid = args.isArgumentsValid();
	this->ehaustive = args.isExhaustive();
	this->min_nucleotide_length = args.getMinNucleotideLength();
	this->min_ssr_length = args.getMinSSRLength();
	this->max_ssr_length = args.getMaxSSRLength();
	this->min_sequence_length = args.getMinSequenceLength();
	this->max_sequence_length = args.getMaxSequenceLength();
	this->min_repeats = args.getMinRepeats();
	this->max_repeats = args.getMaxRepeats();
	this->num_threads = args.getNumThreads();
	this->usage_statement = args.getUsageStatement();
	this->species_1_fasta_file_name = args.getSpecies1FastaFileName();
	this->species_2_blastdb = args.getSpecies2BlastdbName();
	this->enumerated_ssrs = args.getEnumeratedSSRs();
	this->out_file_name = args.getOutFileName();
}
FindSSRsArgs::~FindSSRsArgs()
{
	this->enumerated_ssrs->clear();
	delete this->enumerated_ssrs;
}
bool FindSSRsArgs::isArgumentsValid() const
{
	return this->arguments_valid;
}
bool FindSSRsArgs::isExhaustive() const
{
	return this->ehaustive;;
}
//bool FindSSRsArgs::doingBlast() const
//{
//	return this->doing_blast;
//}
uint32_t FindSSRsArgs::getMinNucleotideLength() const
{
	return this->min_nucleotide_length;
}
uint32_t FindSSRsArgs::getMinSSRLength() const
{
	return this->min_ssr_length;
}
uint32_t FindSSRsArgs::getMaxSSRLength() const
{
	return this->max_ssr_length;
}
uint32_t FindSSRsArgs::getMinSequenceLength() const
{
	return this->min_sequence_length;
}
uint32_t FindSSRsArgs::getMaxSequenceLength() const
{
	return this->max_sequence_length;
}
uint32_t FindSSRsArgs::getMinRepeats() const
{
	return this->min_repeats;
}
uint32_t FindSSRsArgs::getMaxRepeats() const
{
	return this->max_repeats;
}
uint32_t FindSSRsArgs::getNumThreads() const
{
	return this->num_threads;
}
string FindSSRsArgs::getUsageStatement() const
{
	return "\033[0;31mUSAGE: " + this->usage_statement + " (--help or -h for help)\033[0m";
}
string FindSSRsArgs::getSpecies1FastaFileName() const
{
	return this->species_1_fasta_file_name;
}
string FindSSRsArgs::getSpecies2BlastdbName() const
{
	return this->species_2_blastdb;
}
//string& FindSSRsArgs::getSpecies2Blastdb()
//{
//	return this->species_2_blastdb;
//}
unordered_set<string>* FindSSRsArgs::getEnumeratedSSRs() const
{
	return this->enumerated_ssrs;
}
string FindSSRsArgs::getOutFileName() const
{
	return this->out_file_name;
}
void FindSSRsArgs::setExhaustiveStatus(bool _ehaustive)
{
	this->ehaustive = _ehaustive;
}
void FindSSRsArgs::setSpecies1FastaFileName(string species_1_fasta_file_name)
{
	this->species_1_fasta_file_name = species_1_fasta_file_name;
}
void FindSSRsArgs::addEnumeratedSSR(string enumerated_ssr)
{
	assert (enumerated_ssr.size() > 0);

	if (enumerated_ssr.size() > 0)
	{
		this->enumerated_ssrs->insert(enumerated_ssr);
	}
}
void FindSSRsArgs::addEnumeratedSSRs(string enumerated_ssrs_comma_separated)
{
	string temp;
	for (uint32_t i = 0; i < enumerated_ssrs_comma_separated.size(); i++)
	{
		temp = temp + enumerated_ssrs_comma_separated[i];

		if (enumerated_ssrs_comma_separated[i] == ',')
		{
			addEnumeratedSSR(temp.substr(0,i)); // everything up to the comma
			temp = "";
		}
	}
	addEnumeratedSSR(temp);
}
void FindSSRsArgs::setOutFileName(string out_file_name)
{
	this->out_file_name = out_file_name;	
}
void FindSSRsArgs::processArgs(int argc, char* argv[])
{
	uint32_t expected_args = 3; // 0=FindSSRsArgs, 1=input_file.fasta, 2=output_file
	if (argc > 1)
	{
		for (uint32_t i = 0; i < (uint32_t) argc; i++)
		{
			if (strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0)
			{
				printHelp();
				exit(0);
			}
			else if (strcmp(argv[i],"--exhaustive") == 0 || strcmp(argv[i],"-e") == 0)
			//else if (strcmp(argv[i],"--quick") == 0 || strcmp(argv[i],"-q") == 0)
			{
				this->ehaustive = true;
				expected_args = expected_args + 1; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
			}
			else if (strcmp(argv[i],"--ssrs") == 0 || strcmp(argv[i],"-s") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					addEnumeratedSSRs(argv[i]);
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--min-nucs") == 0 || strcmp(argv[i], "-n") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				
				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->min_nucleotide_length;
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--min-ssr-len") == 0 || strcmp(argv[i], "-m") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				
				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->min_ssr_length;

					if (this->min_ssr_length <= 0) // technically < 0 is impossible, but this is more clear to human interpretation
					{
						this->min_ssr_length = 1; // SSR length must be at least 1, obviously
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--min-repeats") == 0 || strcmp(argv[i], "-r") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->min_repeats;
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--max-repeats") == 0 || strcmp(argv[i], "-R") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->max_repeats;
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--max-ssr-len") == 0 || strcmp(argv[i], "-M") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->max_ssr_length;
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--min-seq-len") == 0 || strcmp(argv[i], "-l") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->min_sequence_length;
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--max-seq-len") == 0 || strcmp(argv[i], "-L") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->max_sequence_length;
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--num-threads") == 0 || strcmp(argv[i], "-t") == 0)
			{
				expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					int temp;
					strm << argv[i];
					strm >> temp;
					
					if (temp >= 1)
					{
						this->num_threads = (uint32_t) temp;
					}
					else
					{
						this->arguments_valid = false;
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
		}

		if (expected_args == (uint32_t) argc)
		{
			this->species_1_fasta_file_name = argv[argc - 2];
			this->out_file_name = argv[argc - 1];
		}
		else
		{
			this->arguments_valid = false;
		}
		
	}
	else
	{
		this->arguments_valid = false;
	}
}
void FindSSRsArgs::printHelp() const
{
	cerr << "VERSION:" << endl << endl;
	cerr << "\t0.1 alpha" << endl << endl;
	cerr << "USAGE:" << endl << endl;
	cerr << "\t" << this->usage_statement << "\t(all options must come before the input and output files, which must also be in order)" << endl << endl;
	cerr << "REQUIRED:" << endl << endl;
	cerr << "\t<input-file>" << endl << "\t\tThe input fasta file.  All sequences must contain only UPPERCASE nucleotides.  All sequences must be on only 1 line." << endl << endl;
	cerr << "\t\tGOOD EXAMPLE:\n\t\t\t>seq1\n\t\t\tAGCTGCGCGTATATACGTACGACTCGTATGCAGTC.......\n\t\t\t>seq2\n\t\t\tGATCGATCGATGCTAGCGATGCTACTAGTCTATCGATGCA.......\n\t\t\t...\n\t\t\t...\n\t\t\t..." << endl << endl;
	cerr << "\t\tBAD EXAMPLE:\n\t\t\t>seq1\n\t\t\tagctgcgcgtatatacgtacgactcgtatgcagtc.......\n\t\t\t>seq2\n\t\t\tgAtCGATcgatgcTAGcgATGcTACTaGTCTaTCGaTGca.......\n\t\t\t>seq3\n\t\t\tTCATTTTAGCTAGCTAGCTAGTCGATCTATCATCGATCGTA\n\t\t\tCGCGCGCGATCGATCGATGCTAGTCGACAACGTACGACTAG\n\t\t\tACGTACGTAGCATCGATGCATCGATCGTAGCACGTGTTTTT\n\t\t\tGCTAGCTAGCTGATCGTAGTGCATCGACTACGTAGCTAGCT\n\t\t\t...\n\t\t\t...\n\t\t\t..." << endl << endl;
	cerr << "\t<output_file>" << endl << "\t\tThe name of the output file." << endl << endl;
	cerr << "OPTIONAL:" << endl << endl;
	cerr << "\t-e, --exhaustive" << endl << "\t\tTake an exhaustive approach to finding the SSRs.  By default a quick approach is taken.  Note that all meaninful SSRs will be found within the\n\t\tuser-specified constraints using the exhaustive approach.  Although unlikely, the exhuastive approach could take (worst-case) order n^2\n\t\ttime (if, for example, your data were primarily or entirely one nucleotide OR you specify the minimum base ssr length to be 1 or 2).\n\t\tFor many cases, the default approach is sufficient and will save (possibly significant) time." << endl << endl;
	cerr << "\t-h, --help" << endl << "\t\tShow this help and exit.  All other arguments will be ignored." << endl << endl;
	cerr << "\t-l, --min-seq-len" << endl << "\t\tThe min length of a sequence required before finding SSRs will be attempted. [default: 100]" << endl << endl;
	cerr << "\t-L, --max-seq-len" << endl << "\t\tThe max length of a sequence after which finding SSRs will not be attempted. [default: 1500]" << endl << endl;
	cerr << "\t-m, --min-ssr-len" << endl << "\t\tThe min number of nucleotides in the base of the SSR (e.g., ACGTACGT has base-ssr-len of 4, AAAAAAAAAAAAAAAAAA has base-ssr-len of 1). [default: 4]" << endl << endl;
	cerr << "\t-M, --max-ssr-len" << endl << "\t\tThe max number of nucleotides in the base of the SSR (e.g., ACGTACGT has base-ssr-len of 4, AAAAAAAAAAAAAAAAAA has base-ssr-len of 1). [default: 8]" << endl << endl;
	cerr << "\t-n, --min-nucs" << endl << "\t\tThe min number of nucleotides in the entire SSR (e.g., ACGTACGT has 8 nucleotides, AAAAAAAAAAAAAAAAAA has 18 nucleotides). [default: 16]" << endl << endl;
	//cerr << "\t-q, --quick" << endl << "\t\tTake a faster, less careful approach to finding the SSRs.  By default an exhaustive approach is taken; however, if this flag is specified\n\t\ta \"quick and dirty\" approach will be followed.  Note that all meaninful SSRs will be found within the user-specified constraints using the exhaustive approach.\n\t\tAlthough unlikely, the exhuastive approach could take (worst-case) order n^2 time (if, for example, your data were primarily or entirely one nucleotide OR you specify the minimum base ssr length to be 1 or 2).\n\t\tFor many cases, the \"quick and dirty\" approach is sufficient and will save (possibly significant) time." << endl << endl;
	cerr << "\t-r, --min-repeats" << endl << "\t\tThe min number of repeats in the entire SSR (e.g., ACGTACGT has 2 repeats, AAAAAAAAAAAAAAAAAA has 18 repeats). [default: 1]" << endl << endl;
	cerr << "\t-R, --max-repeats" << endl << "\t\tThe max number of repeats in the entire SSR (e.g., ACGTACGT has 2 repeats, AAAAAAAAAAAAAAAAAA has 18 repeats). [default: none (technically: 4,294,967,295)]" << endl << endl;
	cerr << "\t-s, --ssrs" << endl << "\t\tEnumerated base SSRs to filter output by.  If an SSR is found of a different base, it will be discarded.\n\t\tThe enumerated SSRs should take the form of an UPPERCASE, comma-separated list: BASE-SSR1,BASE-SSR2,...,BASE-SSRn (e.g., AC,GGTCA,TCA,TTCCGAAGGC)." << endl << endl;
	cerr << "\t-t, --num-threads" << endl << "\t\tThe number of threads to use. [default: 1]" << endl << endl;
	cerr << "DEPENDENCIES:" << endl << endl;
	cerr << "\tNone." << endl << endl;
	cerr << "NOTE:" << endl << endl;
	cerr << "\t--We compiled using gcc 4.9.0.  Your compiler must support C++11." << endl << endl;
}
string FindSSRsArgs::toString() const
{
	string output = "FindSSRsArgs\n============\n";
	string temp;
	stringstream strm;
	strm << boolalpha << this->arguments_valid << " " << this->ehaustive << " ";
	strm >> temp;
	output = output + "Valid Arguments=" + temp;
	strm >> temp;
	output = output + "\nExhaustive=" + temp;
	output = output + "\nUsage Statement=" + this->usage_statement;
	output = output + "\nSpecies 1 Fasta File Name=" + this->species_1_fasta_file_name;
	output = output + "\nEnumerated SSRs[ ";
	for (unordered_set<string>::iterator itr = this->enumerated_ssrs->begin(); itr != this->enumerated_ssrs->end(); ++itr)
	{
		output = output + ", " + *itr;
	}
	output = output + " ]\nOut File Name=" + this->out_file_name + "\n";
	return output;
}
void FindSSRsArgs::operator=(const FindSSRsArgs &args)
{
	this->deepCopy(args);
}
