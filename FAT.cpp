#include "FAT.h"

void FAT::Tokenize(const string& str, vector<string>& tokens, const string& delimiters){
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos){
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

FAT::FAT(){
	vector<string> _regionsTXT;
	vector<string> _lineTXT;
	vector<string> _rowTXT;
	vector<string>::iterator it;
	_root = new d_entry[512];
	_fat = new short[65536];
	_data = new cluster[65468];
	string line;
	ifstream _file("OS.fat");
	if( _file.is_open() ){
		getline(_file, line);
		_file.close();
		_regionsTXT.clear();
		Tokenize(line, _regionsTXT, "$");
		_lineTXT.clear();
		Tokenize(_regionsTXT[0], _lineTXT, ";");
		int cont = 0;
		for( it = _lineTXT.begin(); it < _lineTXT.end(); it++ ){
			d_entry tmp;
			_rowTXT.clear();
			Tokenize((*it), _rowTXT, ",");
			if( strcmp( _rowTXT[0].c_str(), "0x000000" ) == 0 )
				tmp._free = true;
			else{
				tmp._free = false;
				tmp._name = B_to_A( _rowTXT[1] );
				if( strcmp(_rowTXT[2].c_str(), "0x000010" ) == 0 )
					tmp._dir = true;
				else
					tmp._dir = false;
				tmp._cDate = B_to_A( _rowTXT[3] );
				tmp._cluster = B_to_A( _rowTXT[4] );
				tmp._size = bitset<32>(_rowTXT[5]).to_ulong();
				tmp._reserved = B_to_A( _rowTXT[6] );
			}
			_root[cont] = tmp;
			cont++;
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
	// delete _root;
	// delete _fat;
	// delete _data;
}

string FAT::strToBin(string value){
	string ret = "";
	for (std::size_t i = 0; i < value.size(); ++i){
		bitset<8> b(value[i]);
		ret += b.to_string();
	}
	return ret;
}

void FAT::save(){
	string _output = "";
	//STORE ROOT SECTION
	for( int i = 0; i < 512; i++ ){
		if( _root[i]._free ){
			_output += "0x000000";
		}else{
			//STORE IF IS FREE
			_output += _root[i]._free ? "0x000000" : "0x111111";
			_output += ",";

			//STORE NAME
			_output += strToBin(_root[i]._name);
			_output += ",";

			//STORE IF IS A DIRECTORY
			_output += _root[i]._dir ? "0x000010" : "0x000020";
			_output += ",";

			//STORE CREATION DATE
			_output += strToBin(_root[i]._cDate);
			_output += ",";

			//STORE CLUSTER
			_output += strToBin(_root[i]._cluster);
			_output += ",";

			//STORE SIZE
			unsigned int mask = 1 << (sizeof(int)*8-1);
			for( int j = 0; j < sizeof(int)*8; j++ ){
				if( (_root[i]._size & mask) == 0 )
					_output += '0';
				else
					_output += '1';
				mask >>= 1;
			}
			_output += ",";

			//STORE RESERVED
			_output += strToBin(_root[i]._reserved);
		}
		_output += ";";
	}
	_output += "$";

	//WRITE IN FILE
	remove("OS.fat");
	ofstream _file;
	_file.open( "OS.fat", ofstream::out );
	_file << _output;
	_file.close();
}

bool FAT::searchDir(string dirName){
	for( int i = 0; i < 512; i++ ){
		if( _root[i]._free == false && _root[i]._dir == true ){
			string name( _root[i]._name );
			if( strcmp( dirName.c_str(), name.c_str() ) == 0 )
				return true;
		}
	}
	return false;
}

bool FAT::searchFile(string fileName){
	for( int i = 0; i < 512; i++ ){
		if( _root[i]._free == false && _root[i]._dir == false && strcmp( fileName.c_str(), _root[i]._name.c_str() ) == 0 )
			return true;
	}
	return false;
}

string FAT::B_to_A(string input){
	int size= input.size();
	int bin[8];
	int tempChar = 0;
	char character;
	string output = "";

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
		output += character;
	}
	return output;
}

bool FAT::mkDir(string dirName, string dir){
	if( searchDir(dirName) == false ){
		if( strcmp(dir.c_str(), "/") == 0 ){
			for (int i = 0; i < 512; ++i){
				if( _root[i]._free ){
					time_t theTime = time(NULL);
					struct tm *aTime = localtime(&theTime);

					int day = aTime->tm_mday;
					int month = aTime->tm_mon + 1;
					int year = aTime->tm_year + 1900;

					stringstream ss;
					if( day < 10 )
						ss << "0" << day;
					else if( day >= 10 )
						ss << day;

					if( month < 10 )
						ss << "0" << month;
					else if( month >= 10 )
						ss << month;
					ss << year;

					_root[i]._free = false;
					_root[i]._dir = true;
					_root[i]._name = dirName;
					_root[i]._cDate = ss.str();
					cluster tmp;
					_data[i] = tmp;
					ss.str("");
					ss << i;
					_root[i]._cluster = ss.str();
					_root[i]._reserved = "00000000";
					return true;
				}
			}
		}else{

		}
	}
	return false;
}