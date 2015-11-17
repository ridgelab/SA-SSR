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
	this->additional_output = false;
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
	this->out_file_header = "#Sequence_Name\tSSR\tRepeats\tPosition (zero-based)\n";
	this->ignore_chars = new unordered_set<char>;
	
	processArgs(argc, argv);
}
FindSSRsArgs::FindSSRsArgs(const FindSSRsArgs &args)
{
	this->deepCopy(args);
}
void FindSSRsArgs::deepCopy(const FindSSRsArgs &args)
{
	this->arguments_valid = args.isArgumentsValid();
	this->additional_output = args.isAdditionalOutput();
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
	//this->species_2_blastdb = args.getSpecies2BlastdbName();
	this->enumerated_ssrs = args.getEnumeratedSSRs();
	this->out_file_name = args.getOutFileName();
	this->out_file_header = args.getOutFileHeader();
	this->ignore_chars = args.getIgnoreChars();
}
FindSSRsArgs::~FindSSRsArgs()
{
	this->enumerated_ssrs->clear();
	delete this->enumerated_ssrs;
	
	this->ignore_chars->clear();
	delete this->ignore_chars;
}
bool FindSSRsArgs::isArgumentsValid() const
{
	return this->arguments_valid;
}
bool FindSSRsArgs::isAdditionalOutput() const
{
	return this->additional_output;
}
bool FindSSRsArgs::isIncludeZero() const
{
	return this->include_zero;
}
bool FindSSRsArgs::isCharsToIgnore() const
{
	return this->ignore_chars->empty() ? false : true;
}
bool FindSSRsArgs::isExhaustive() const
{
	return this->ehaustive;
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
//uint32_t FindSSRsArgs::getMinRepeats(const uint32_t &k) const
//{
//	return this->min_repeats(k);
//}
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
//string FindSSRsArgs::getSpecies2BlastdbName() const
//{
//	return this->species_2_blastdb;
//}
//string& FindSSRsArgs::getSpecies2Blastdb()
//{
//	return this->species_2_blastdb;
//}
unordered_set<string>* FindSSRsArgs::getEnumeratedSSRs() const
{
	return this->enumerated_ssrs;
}
unordered_set<char>* FindSSRsArgs::getIgnoreChars() const
{
	return this->ignore_chars;
}
string FindSSRsArgs::getOutFileName() const
{
	return this->out_file_name;
}
string FindSSRsArgs::getOutFileHeader() const
{
	return this->out_file_header;
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
void FindSSRsArgs::addIgnoreChar(char ignore_char)
{
	this->ignore_chars->insert(ignore_char);
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
void FindSSRsArgs::addIgnoreChars(string ignore_chars_comma_separated)
{
	for (uint32_t i = 0; i < ignore_chars_comma_separated.size(); i++)
	{
		if (ignore_chars_comma_separated[i] != ',')
		{
			addIgnoreChar((char) toupper(ignore_chars_comma_separated[i]));
		}
	}
}
void FindSSRsArgs::setOutFileName(string out_file_name)
{
	this->out_file_name = out_file_name;	
}
void FindSSRsArgs::processArgs(int argc, char* argv[])
{
	uint32_t expected_args = 3; // 0=FindSSRsArgs, 1=input_file.fasta, 2=output_file
	if (argc > 2)
	{
		for (uint32_t i = 0; i < (uint32_t) argc; i++)
		{
			if (strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0)
			{
				printHelp();
				exit(0);
			}
			else if (strcmp(argv[i],"--add-full-seq") == 0 || strcmp(argv[i],"-f") == 0)
			{
				this->additional_output = true;
				this->out_file_header = "#Sequence_Name\tSequence\tSSR\tRepeats\tPosition (zero-based)\n";
				//expected_args = expected_args + 1;
				++expected_args;
			}
			else if (strcmp(argv[i],"--include-zero") == 0 || strcmp(argv[i],"-z") == 0)
			{
				this->include_zero = true;
				//expected_args = expected_args + 1; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				++expected_args; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
			}
			else if (strcmp(argv[i],"--exhaustive") == 0 || strcmp(argv[i],"-e") == 0)
			//else if (strcmp(argv[i],"--quick") == 0 || strcmp(argv[i],"-q") == 0)
			{
				this->ehaustive = true;
				//expected_args = expected_args + 1; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				++expected_args; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
			}
			else if (strcmp(argv[i],"--ssrs") == 0 || strcmp(argv[i],"-s") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

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
			else if (strcmp(argv[i],"--ignore") == 0 || strcmp(argv[i],"-i") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					addIgnoreChars(argv[i]);
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--min-nucs") == 0 || strcmp(argv[i], "-n") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				
				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->min_nucleotide_length;
					
					if (strm.fail())
					{
						this->arguments_valid = false;
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--min-ssr-len") == 0 || strcmp(argv[i], "-m") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				
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
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->min_repeats;

					if (strm.fail())
					{
						this->arguments_valid = false;
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			//else if (strcmp(argv[i], "--min-repeats") == 0 || strcmp(argv[i], "-r") == 0) // this would have to be processed AFTER min/max ssr lengths!!
			//{
			//	//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
			//	expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
                          
			//	if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
			//	{
			//		i++;
			//		string str;
			//		stringstream strm;
			//		strm << argv[i];
			//		
			//		while (getline(strm, str, ','))
			//		{
			//			uint32_t temp_int = 0;
			//			stringstream other_strm;
			//			other_strm << str;
			//			other_strm >> temp_int;
                          
			//			if (!strm.fail())
			//			{
			//				this->min_repeats.push_back(temp_int):
			//			}
			//			else
			//			{
			//				this->min_repeats.push_back(0); // So we put each of the following repeat limits into the correct slot
			//				this->arguments_valid = false;
			//			}
			//		}
                          
			//		if (this->min_repeats.size() != (this->max_ssr_length - this->min_ssr_length + 1))
			//		{
			//			this->arguments_valid = false;
			//		}
			//	}
			//	else
			//	{
			//		this->arguments_valid = false;
			//	}
			//}
			else if (strcmp(argv[i], "--max-repeats") == 0 || strcmp(argv[i], "-R") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->max_repeats;
					
					if (strm.fail())
					{
						this->arguments_valid = false;
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--max-ssr-len") == 0 || strcmp(argv[i], "-M") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->max_ssr_length;
					
					if (strm.fail())
					{
						this->arguments_valid = false;
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--min-seq-len") == 0 || strcmp(argv[i], "-l") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->min_sequence_length;
					
					if (strm.fail())
					{
						this->arguments_valid = false;
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--max-seq-len") == 0 || strcmp(argv[i], "-L") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					strm << argv[i];
					strm >> this->max_sequence_length;
					
					if (strm.fail())
					{
						this->arguments_valid = false;
					}
				}
				else
				{
					this->arguments_valid = false;
				}
			}
			else if (strcmp(argv[i], "--num-threads") == 0 || strcmp(argv[i], "-t") == 0)
			{
				//expected_args = expected_args + 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file
				expected_args += 2; // 0=FindSSRsArgs, (1:argc-3)=( (-e || --exhaustive) && ((-s || --ssrs) SSR1,SSR2,SSR3,...,SSRn) && ((-b || --blast) species2-blastdb) && ((-n || --min-nucs) 16) && ((-m || --min-ssr-len) 4) && ((-M || --max-ssr-len) 8) ), (argc-2)=input_file.fasta, (argc-1)=output_file

				if (i < (uint32_t) (argc - 3)) // as long as there is more space for the input and output files...
				{
					i++;
					stringstream strm;
					int temp;
					strm << argv[i];
					strm >> temp;
					
					if (strm.fail())
					{
						this->arguments_valid = false;
					}
					
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
	cerr << "    0.1 alpha" << endl << endl;
	cerr << "USAGE:" << endl << endl;
	cerr << "    " << this->usage_statement << "\n\n    Options may be in any order, but must come before the input and output files." << endl << endl;
	cerr << "REQUIRED:" << endl << endl;
	cerr << "    <input-file>" << endl << "        The input fasta file. Header lines must contain no tabs. All sequences\n        must not contain whitespace (except newlines). This is a positional\n        argument." << endl << endl;
	//cerr << "        GOOD EXAMPLE:\n            >seq1\n            AGCTGCGCGTATATACGTACGACTCGTATGCAGTC.......\n            >seq2\n            GATCGATCGATGCTAGCGATGCTACTAGTCTATCGATGCA.......\n            ...\n            ...\n            ..." << endl << endl;
	//cerr << "        BAD EXAMPLE:\n            >seq1\n            agctgcgcgtatatacgtacgactcgtatgcagtc.......\n            >seq2\n            gAtCGATcgatgcTAGcgATGcTACTaGTCTaTCGaTGca.......\n            >seq3\n            TCATTTTAGCTAGCTAGCTAGTCGATCTATCATCGATCGTA\n            CGCGCGCGATCGATCGATGCTAGTCGACAACGTACGACTAG\n            ACGTACGTAGCATCGATGCATCGATCGTAGCACGTGTTTTT\n            GCTAGCTAGCTGATCGTAGTGCATCGACTACGTAGCTAGCT\n            ...\n            ...\n            ..." << endl << endl;
	cerr << "    <output_file>" << endl << "        The name of the output file. This is a positional argument." << endl << endl;
	cerr << "OPTIONAL:" << endl << endl;
	cerr << "    INPUT CONTROL:" << endl << endl;
	cerr << "        -i, --ignore" << endl << "            Ignore listed characters from the input sequences.  This parameter\n            must be a comma-separated list of UPPERCASE characters:\n            \"CHAR-1,CHAR-2,...,CHAR-n\" (e.g., \"N\" or \"N,K\").  Please note that\n            when using this option, the SSRs in each sequence are neither\n            guaranteed to be in order nor grouped together.  Accordingly, sorting\n            the output file may be desirable:\n                cat ssr_file \\\n                    | sort -n -t '\\t' -k 4,4 \\\n                    | sort -s -t '\\t' -k 1,1 \\\n                    > sorted_ssr_file" << endl << endl;
	cerr << "        -l, --min-seq-len" << endl << "            The min length of a fasta sequence required before finding SSRs will\n            be attempted. [default: 100]" << endl << endl;
	cerr << "        -L, --max-seq-len" << endl << "            The max length of a fasta sequence after which finding SSRs will not\n            be attempted. [default: 1500]" << endl << endl;
	cerr << "    OUTPUT CONTROL:" << endl << endl;
	cerr << "        -f, --add-full-seq" << endl << "            Add an additional column, containing the full sequence, to the output\n            file.  This column will be added after the `Sequence_Name' column." << endl << endl;
	cerr << "        -m, --min-ssr-len" << endl << "            The min number of nucleotides in the base of the SSR (e.g., ACGTACGT\n            has base-ssr-len of 4, AAAAAAAAAAAAAAAAAA has base-ssr-len of 1).\n            [default: 4]" << endl << endl;
	cerr << "        -M, --max-ssr-len" << endl << "            The max number of nucleotides in the base of the SSR (e.g., ACGTACGT\n            has base-ssr-len of 4, AAAAAAAAAAAAAAAAAA has base-ssr-len of 1).\n            [default: 8]" << endl << endl;
	cerr << "        -n, --min-nucs" << endl << "            The min number of nucleotides in the entire SSR (e.g., ACGTACGT has 8\n            nucleotides, AAAAAAAAAAAAAAAAAA has 18 nucleotides). [default: 16]" << endl << endl;
	cerr << "        -r, --min-repeats" << endl << "            The min number of repeats in the entire SSR (e.g., ACGTACGT has 2\n            repeats, AAAAAAAAAAAAAAAAAA has 18 repeats). [default: 1]" << endl << endl;
	cerr << "        -R, --max-repeats" << endl << "            The max number of repeats in the entire SSR (e.g., ACGTACGT has 2\n            repeats, AAAAAAAAAAAAAAAAAA has 18 repeats). [default: none\n            (technically: 4,294,967,295)]" << endl << endl;
	cerr << "        -s, --ssrs" << endl << "            Filter output by the supplied list of base SSR units (e.g., AC for\n            ACACAC). If an SSR with a different base unit is found, it will be\n            discarded. The enumerated SSRs should take the form of an UPPERCASE,\n            comma-separated list: SSR-BASE1,SSR-BASE2,...,SSR-BASEn\n            (e.g., AC,GGTCA,TCA,TTCCGAAGGC)." << endl << endl;
	cerr << "        -z, --include-zero" << endl << "            Include an output record for sequences where no SSRs are found\n            (within the provided constraints)." << endl << endl;
	cerr << "    MISC:" << endl << endl;
	cerr << "        -e, --exhaustive" << endl << "            Take an exhaustive approach to finding the SSRs.  By default a quick\n            approach is taken.  Note that all meaninful SSRs will be found within\n            the user-specified constraints using the exhaustive approach.\n            Although unlikely, the exhuastive approach could take (worst-case)\n            order n^2 time (if, for example, your data were primarily or entirely\n            one nucleotide OR you specify the minimum base ssr length to be 1 or\n            2). For many cases, the default approach is sufficient and will save\n            (possibly significant) time." << endl << endl;
	cerr << "        -h, --help" << endl << "            Show this help and exit.  All other arguments will be ignored." << endl << endl;
	cerr << "        -t, --num-threads" << endl << "            The number of threads to use. [default: 1]" << endl << endl;
	cerr << "DEPENDENCIES:" << endl << endl;
	cerr << "    None." << endl << endl;
	cerr << "NOTE:" << endl << endl;
	cerr << "    --We compiled using gcc 4.9.2.  Your compiler must support C++11." << endl << endl;
}
string FindSSRsArgs::toString() const
{
	string output = "FindSSRsArgs\n============\n";
	string temp;
	stringstream strm;
	strm << boolalpha << this->arguments_valid << " " << this->ehaustive << " ";
	strm >> temp;
	//output = output + "Valid Arguments=" + temp;
	output += "Valid Arguments=" + temp;
	strm >> temp;
	//output = output + "\nExhaustive=" + temp;
	//output = output + "\nUsage Statement=" + this->usage_statement;
	//output = output + "\nSpecies 1 Fasta File Name=" + this->species_1_fasta_file_name;
	//output = output + "\nEnumerated SSRs[ ";
	output += "\nExhaustive=" + temp;
	output += "\nUsage Statement=" + this->usage_statement;
	output += "\nSpecies 1 Fasta File Name=" + this->species_1_fasta_file_name;
	output += "\nEnumerated SSRs[ ";
	for (unordered_set<string>::iterator itr = this->enumerated_ssrs->begin(); itr != this->enumerated_ssrs->end(); ++itr)
	{
		//output = output + ", " + *itr;
		output += ", " + *itr;
	}
	//output = output + " ]\nOut File Name=" + this->out_file_name + "\n";
	output += " ]\nOut File Name=" + this->out_file_name + "\n";
	return output;
}
void FindSSRsArgs::operator=(const FindSSRsArgs &args)
{
	this->deepCopy(args);
}
