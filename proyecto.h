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
#include <ctime>
#include <queue>
using namespace std;

class FAT
{
private:
	#pragma pack(1)
	struct d_entry{
		bool _free;
		char name[10];
		bool _dir;
		time_t _cDate;
		unsigned short _cluster;
		char reserved[6];
		unsigned int _size;		
	};
	struct _cluster {
		d_entry* entries;
		char* buffer;
	};
	d_entry _root[512];
	unsigned short _FAT[65535];
	_cluster _dataRegion[65535];	
	d_entry* currentDir;
	d_entry* parentDir;
	queue<int> freeIndex;
	string strToBin(string);
	string B_to_A(string);
	void Tokenize(const string&, vector<string>&, const string&);
public:
	FAT();
	~FAT();
	bool mkdir(string); 
	bool searchDir(string);
	bool searchFile(string);
	void checkFreeIndex();
	void changeDirectory(string);
	void print(string);
	string formatDate(time_t);
	string cat(string);
	void wCat(string);
	string listDirectory();
	d_entry* retrieveDir(string);
	int getblockSize();
	void initialize();
	void save();
};

#endif