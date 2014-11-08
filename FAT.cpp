#include "FAT.h"

FAT::FAT(){
	_root = new d_entry[512];
	string line;
	ifstream _file("OS.fat");
	if( _file.is_open() ){
		getline(_file, line);
		_file.close();
		int cont = 0, cont2 = 0;
		string tmp2;
		for( int i = 0; i < 512; i++ ){
			d_entry tmp;
			tmp2 = line.substr( cont, 256 );
			cont += 256;
			if( strcmp( tmp2.substr( 0, 8 ).c_str(), "0x000000" ) == 0 )
				tmp._free = true;
			else{
				tmp._free = false;
				tmp._name = new char[10];
				B_to_A( tmp2.substr( 8, 80 ).c_str(), tmp._name );
				if( strcmp( tmp2.substr( 88, 8 ).c_str(), "0x000010" ) == 0 )
					tmp._dir = true;
				else
					tmp._dir = false;
				tmp._cDate = new char[8];
				B_to_A( tmp2.substr( 96, 64 ).c_str(), tmp._cDate );
				tmp._cluster = new char[2];
				B_to_A( tmp2.substr( 160, 16 ).c_str(), tmp._cluster );
				tmp._size = bitset<32>(tmp2.substr( 176, 32 )).to_ulong();
				tmp._reserved = new char[6];
				B_to_A( tmp2.substr( 208, 48 ).c_str(), tmp._reserved );
			}
			_root[i] = tmp;
		}
	}else{
		for(int i = 0; i < 512; i++){
			d_entry tmp;
			tmp._free = true;
			_root[i] = tmp;
		}
	}
}

FAT::~FAT(){
	save();
	free( _root );
}

void FAT::save(){
	string _output = "";
	//STORE ROOT SECTION
	for( int i = 0; i < 512; i++ ){
		if( _root[i]._free ){
			_output += "0x000000";
			for( int j = 0; j < 248; j++ )
				_output += "0";
		}else{
			int cont = 0;
			//STORE IF IS FREE
			_output += _root[i]._free ? "0x000000" : "0x111111";

			//STORE NAME
			for( int i = 8; i < 88; i++ ){
				_output += _root[i]._name[cont];
				cont++;
			}

			//STORE IF IS A DIRECTORY
			_output += _root[i]._dir ? "0x000010" : "0x000020";

			//STORE CREATION DATE
			cont = 0;
			for( int i = 96; i < 160; i++ ){
				_output += _root[i]._cDate[cont];
				cont++;
			}

			//STORE CLUSTER
			cont = 0;
			for( int i = 160; i < 176; i++ ){
				_output += _root[i]._cluster[cont];
				cont++;
			}

			//STORE SIZE
			unsigned int mask = 1 << (sizeof(int)*8-1);
			for( int i = 0; i < sizeof(int)*8; i++ ){
				if( (_root[i]._size & mask) == 0 )
					_output += '0';
				else
					_output += '1';
				mask >>= 1;
			}

			//STORE RESERVED
			cont = 0;
			for( int i = 208; i < 256; i++ ){
				_output += _root[i]._reserved[cont];
				cont++;
			}
		}
	}

	//WRITE IN FILE
	ofstream _file;
	_file.open( "OS.fat", ofstream::out );
	_file << _output;
	_file.close();
}

bool FAT::searchDir(string dirName){
	for( int i = 0; i < 512; i++ ){
		if( _root[i]._free == false && _root[i]._dir == true && strcmp( dirName.c_str(), _root[i]._name ) == 0 )
			return true;
	}
	return false;
}

void FAT::B_to_A(const char* input, char* output){
	int size=strlen(input);
	int bin[8];
	int tempChar = 0;
	char character;

	int a = 0;
	for(int b=0; b<size/8; b++){
		for(int c=0; c<8; c++){
			bin[c]=(int)input[a]-48;
			a++;
		}
		int power[8];
		int count = 7;
		for(int d=0; d<8; d++){
			power[d]=count;
			count--;
		}
		for(int e=0;e<8;e++){
			double r=bin[e];
			double s=power[e];
			tempChar += r* pow(2,s);
		}

		character = tempChar;
		tempChar = 0;
		output[b] = character;
	}
}