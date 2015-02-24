/*
	Brandon Pickett
	SingleResult.cpp
*/

#include "../include/SingleResult.h"

using namespace std;

SingleResult::SingleResult()
{
	k = 0;
	r = 0;
	p = 0;
}
SingleResult::SingleResult(uint32_t k, uint32_t r, uint32_t p)
{
	this->k = k;
	this->r = r;
	this->p = p;
}
SingleResult::~SingleResult()
{

}
uint32_t SingleResult::getK() const
{
	return k;
}
uint32_t SingleResult::getR() const
{
	return r;
}
uint32_t SingleResult::getP() const
{
	return p;
}
void SingleResult::setK(uint32_t k)
{
	this->k = k;
}
void SingleResult::setR(uint32_t r)
{
	this->r = r;
}
void SingleResult::setP(uint32_t p)
{
	this->p = p;
}
void SingleResult::setAll(uint32_t k, uint32_t r, uint32_t p)
{
	this->k = k;
	this->r = r;
	this->p = p;
}
void SingleResult::writeToFile(const string &header, const string &sequence, string &output) const
{
	string temp;
	stringstream strm;
	strm << (this->r + 1) << " " << this->p;
	strm >> temp;
	output = output + header.substr(1,header.size() - 1) + "\t" + sequence.substr(this->p,this->k) + "\t" + temp; // header\tSSR\tRepeats
	strm >> temp;
	output = output + "\t" + temp; // \tPosition
}
string SingleResult::toString() const
{
	string output = "SingleResult[ k=";
	string temp;
	stringstream strm;
	strm << k << " " << r << " " << p;
	strm >> temp;
	output = output + temp + ", r=";
	strm >> temp;
	output = output + temp + ", p=";
	strm >> temp;
	output = output + temp + " ]";
	return output;
}
bool SingleResult::operator<(const SingleResult &s) const
{
	if (this->getP() < s.getP())
	{
		return true;
	}
	else if (this->getP() > s.getP())
	{
		return false;
	}
	else // p's are equal, choose the one with more total nucleotides
	{
		if ( (this->getK() * (this->getR() + 1)) > (s.getK() * (s.getR() + 1)) )
		{
			return true;
		}
		else if ( (this->getK() * (this->getR() + 1)) < (s.getK() * (s.getR() + 1)) )
		{
			return false;
		}
		else
		{
			if (this->getK() < s.getK()) // p's and (k*(r+1))'s are equal, choose the smaller kmer
			{
				return true;
			}
			else if (this->getK() > s.getK())
			{
				return false;
			}
			else
			{
				string temp;
				stringstream strm;
				strm << this->getP() << " " << this->getK() << " " << this->getR() << " " << s.getK() << " " << s.getR();
				strm >> temp;

				cerr << "\033[0;31m" << "You should never get here!!!" << endl;
				cerr << this->toString() << endl;
				cerr << s.toString() << endl;
				cerr << "p= " << temp << "\t";
				cerr << "k1=" << temp << ",r1=" << temp << "\t";
				cerr << "k2=" << temp << ",r2=" << temp << "\033[0m" << endl;
				exit(1);
			}
		}	
	}
}
