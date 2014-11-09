#ifndef FAT_H
#define FAT_H
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cmath>
#include <bitset>
#include <string.h>
#include <vector>
using namespace std;

class FAT
{
private:
	struct d_entry{
		bool _free;
		string _name;
		bool _dir;
		string _cDate;
		string _cluster;
		unsigned int _size;
		string _reserved;
	};

	struct cluster{
		d_entry _entry[128];
		string _data;
	};

	d_entry* _root;
	cluster* _data;
	short* _fat;
	void B_to_A(string, string);
	void Tokenize(const string&, vector<string>&, const string&);
	string strToBin(string);
public:
	FAT();
	bool searchDir(string);
	bool searchFile(string);
	bool mkDir(string, string);
	void save();
	~FAT();
};

#endif