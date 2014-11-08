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
using namespace std;

class FAT
{
private:
	struct d_entry{
		bool _free;
		char* _name;
		bool _dir;
		char* _cDate;
		char* _cluster;
		unsigned int _size;
		char* _reserved;
	};

	struct cluster{
		d_entry _entry[128];
	};

	d_entry* _root;
	void B_to_A(const char*, char*);
public:
	FAT();
	bool searchDir(string);
	void save();
	~FAT();
};

#endif