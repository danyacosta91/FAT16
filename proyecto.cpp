#include "proyecto.h"
#include <sstream>
#include <algorithm>

void FAT::Tokenize(const string& str, vector<string>& tokens, const string& delimiters){
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos = str.find_first_of(delimiters, lastPos);
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
	initialize();
	string line;
	ifstream _file("OS.fat");
	if( _file.is_open() ){
		getline(_file, line);
		_file.close();
		_regionsTXT.clear();
		Tokenize(line, _regionsTXT, "$");
		_lineTXT.clear();
		Tokenize(_regionsTXT[0], _lineTXT, ";");
		for( int i = 0; i < 512; i++ ){
			_rowTXT.clear();
			Tokenize(_lineTXT[i], _rowTXT, ",");
			if( strcmp( _rowTXT[0].c_str(), "0x000000" ) == 0 )
				_root[i]._free = true;
			else{
				_root[i]._free = false;
				strcpy( _root[i].name, B_to_A( _rowTXT[1] ).c_str() );
				if( strcmp(_rowTXT[2].c_str(), "0x000010" ) == 0 )
					_root[i]._dir = true;
				else
					_root[i]._dir = false;
				_root[i]._cDate = atol( B_to_A( _rowTXT[3] ).c_str() );
				_root[i]._cluster = static_cast<short>(atoi(B_to_A( _rowTXT[4]).c_str()));
				_root[i]._size = bitset<32>(_rowTXT[5]).to_ulong();
				strcpy( _root[i].reserved, B_to_A( _rowTXT[6] ).c_str() );
			}
		}
		_lineTXT.clear();
		Tokenize(_regionsTXT[1], _lineTXT, ",");
		for( int i = 0; i < 65535; i++ )
			_FAT[i] = static_cast<short>(atoi(_lineTXT[i].c_str()));
		_lineTXT.clear();
		Tokenize(_regionsTXT[3], _lineTXT, ";");
		int cont = 0;
		for( auto it = _lineTXT.begin(); it != _lineTXT.end(); it++ ){
			if( strcmp( (*it).substr(0,2).c_str(), "0x" ) == 0 ){
				_rowTXT.clear();
				Tokenize( (*it), _rowTXT, "#");
				for( int i = 0; i < 512; i++ ){
					_regionsTXT.clear();
					Tokenize( line, _rowTXT, ",");
					if( strcmp( _regionsTXT[0].c_str(), "0x000000" ) == 0 )
						_dataRegion[cont].entries[i]._free = true;
					else{
						_dataRegion[cont].entries[i]._free = false;
						strcpy( _dataRegion[cont].entries[i].name, B_to_A( _regionsTXT[1] ).c_str() );
						if( strcmp(_regionsTXT[2].c_str(), "0x000010" ) == 0 )
							_dataRegion[cont].entries[i]._dir = true;
						else
							_dataRegion[cont].entries[i]._dir = false;
						_dataRegion[cont].entries[i]._cDate = atol( B_to_A( _regionsTXT[3] ).c_str() );
						_dataRegion[cont].entries[i]._cluster = static_cast<short>( atoi(B_to_A(_regionsTXT[4]).c_str()) );
						_dataRegion[cont].entries[i]._size = bitset<32>(_regionsTXT[5]).to_ulong();
						strcpy( _dataRegion[cont].entries[i].reserved, B_to_A( _regionsTXT[6] ).c_str() );
					}
				}
			}else if( strcmp( (*it).substr(0,8).c_str(), "00000000" ) == 0 ){
			}else
				strcpy( _dataRegion[cont].buffer, (*it).c_str() );
			cont++;
		}
	}
	checkFreeIndex();
}

FAT::~FAT(){

}

bool FAT::mkdir(string dirName){
	if (strlen(dirName.c_str()) != 0 && strlen(dirName.c_str()) < 10){ // Validacion de cadena
		if (searchDir(dirName) == false){ //Validacion de directorio existente en la carpeta actual
			d_entry newDir;
			newDir._free = false;
			strcpy(newDir.name, dirName.c_str());
			newDir._dir = true;
			newDir._cDate = time(0);
			newDir._cluster = freeIndex.front();						
			newDir._size = 4096;	
			if (currentDir == NULL){ //Se encuentra en la root
				for (int i = 0; i < 512; i++){
					if (_root[i]._free == true){
						_root[i] = newDir;

						_dataRegion[freeIndex.front()].entries = new d_entry[128];
						d_entry x;
						x._free = true;
						string c = "NULL";
						strcpy(x.name, c.c_str());
						x._dir = false;
						x._cDate = 0;
						x._cluster = 0xFFFF;	
						x._size =  0xFFFFFFFF;
						for (int i = 0; i < 128; i++){
							_dataRegion[freeIndex.front()].entries[i] = x;
						}
						freeIndex.pop();
						return true;
					}
				}
				return false;
			} else { //Si se encuentra en el Data Region
				if (_FAT[currentDir->_cluster] == 0xFFFF){
					unsigned short currentIndex = currentDir->_cluster; 
					for (int i = 0; i < 128; i++){
						if (_dataRegion[currentDir->_cluster].entries[i]._free == true){
							_dataRegion[currentDir->_cluster].entries[i] = newDir;
							_dataRegion[freeIndex.front()].entries = new d_entry[128];
							d_entry x;
							x._free = true;
							string c = "NULL";
							strcpy(x.name, c.c_str());
							x._dir = false;
							x._cDate = 0;
							x._cluster = 0xFFFF;	
							x._size =  0xFFFFFFFF;
							for (int i = 1; i < 128; i++){
								_dataRegion[freeIndex.front()].entries[i] = x;
							}
							_dataRegion[freeIndex.front()].entries[0] = *currentDir;
							freeIndex.pop();
							return true;
						}
					}
					freeIndex.pop();
					currentIndex = freeIndex.front();
					_FAT[currentDir->_cluster] = currentIndex;

					
					_dataRegion[currentIndex].entries[0] = newDir;

					_dataRegion[newDir._cluster].entries = new d_entry[128];
					d_entry x;
					x._free = true;
					string c = "NULL";
					strcpy(x.name, c.c_str());
					x._dir = false;
					x._cDate = 0;
					x._cluster = 0xFFFF;	
					x._size =  0xFFFFFFFF;
					for (int i = 1; i < 128; i++){
						_dataRegion[freeIndex.front()].entries[i] = x;
					}
					_dataRegion[newDir._cluster].entries[0] = *currentDir;
					freeIndex.pop();
					return true;
				} else {
					unsigned short currentIndex = currentDir->_cluster;
					while (_FAT[currentIndex] != 0xFFFF){
						currentIndex = _FAT[currentIndex];
					}
					for (int i = 0; i < 128; i++){
						if(_dataRegion[currentIndex].entries[i]._free){
							_dataRegion[currentIndex].entries[i] = newDir;

							_dataRegion[newDir._cluster].entries = new d_entry[128];
							d_entry x;
							x._free = true;
							string c = "NULL";
							strcpy(x.name, c.c_str());
							x._dir = false;
							x._cDate = 0;
							x._cluster = 0xFFFF;	
							x._size =  0xFFFFFFFF;
							for (int i = 1; i < 128; i++){
								_dataRegion[freeIndex.front()].entries[i] = x;
							}
							_dataRegion[freeIndex.front()].entries[0] = *currentDir;
							freeIndex.pop();							
							return true;
						}
					}
					freeIndex.pop();
					currentIndex = freeIndex.front();
					_FAT[currentDir->_cluster] = currentIndex;

					
					_dataRegion[currentIndex].entries[0] = newDir;

					_dataRegion[newDir._cluster].entries = new d_entry[128];
					d_entry x;
					x._free = true;
					string c = "NULL";
					strcpy(x.name, c.c_str());
					x._dir = false;
					x._cDate = 0;
					x._cluster = 0xFFFF;	
					x._size =  0xFFFFFFFF;
					for (int i = 1; i < 128; i++){
						_dataRegion[freeIndex.front()].entries[i] = x;
					}
					_dataRegion[newDir._cluster].entries[0] = *currentDir;
					freeIndex.pop();
					return true;
				}
			}
		} else{
			return false;
		}
	} else {
		return false;
	}
}


/*bool FAT::rmdir(string dirName){
	if (strlen(dirName.c_str()) != 0 && strlen(dirName.c_str()) < 10){
		if (currentDir == NULL){ // Si esta en el root
			d_entry* tmp;
			for (int i = 0; i < 512; i++){
				if (strcmp(_root[i].name,dirName.c_str()) == 0 ){
					tmp = &_root[i];
					break;
				}
			}
			if(_FAT[tmp->_cluster] == 0xFFFF){
				bool emptyDir = true;
				for (int i = 0; i < 128; i++){
					if(!_dataRegion[tmp->_cluster].entries[i]._free){
						emptyDir = false;
						return false;
					}
				}
				_dataRegion[tmp->_cluster].entries = NULL;
				tmp->_free = true;
			}
		} else { //Si esta en otro nivel
			d_entry* tmp = NULL;
			unsigned short currentIndex = currentDir->_cluster;
			while (_FAT[currentIndex] != 0xFFFF){
				for (int i = 0; i < 128; i++){
					if (strcmp(_dataRegion[currentIndex].entries[i].name, dirName.c_str()) == 0){
						tmp = &dataRegion[currentIndex].entries[i];
					}
				}
			}
		}
	}
}*/

bool FAT::searchDir(string dirName){
	if (strlen(dirName.c_str()) != 0 && strlen(dirName.c_str()) < 10){
		if (currentDir == NULL){ // Si se encuentra buscando en el root
			for (int i = 0; i < 512; i++){ //Revisa las 512 entradas del root
				if (strcmp(_root[i].name, dirName.c_str()) == 0 ){ // Si el nombre de la d_entry es el mismo que el dirName dado
					return true;
				} 
			}
			return false;
		} else { //Si se encuentra buscando en el directorio Actual
			d_entry temp = *currentDir;
			unsigned short blockIndex = temp._cluster;
			if (_FAT[blockIndex] == 0xFFFF ){ //si es bloque unico
				for (int i = 0; i < 128; i++){
					d_entry* tmpEntry = &_dataRegion[blockIndex].entries[i]; 
					if (strcmp(tmpEntry->name, dirName.c_str()) == 0){
						return true;
					}
				}
				return false;
			} else { //si son varios clusters
				unsigned short currentIndex = blockIndex;
				while (_FAT[currentIndex] != 0xFFFF){ //mientras no se encuentre el bloque final del directorio
					for (int i = 0; i < 128; i++){
						d_entry tmpEntry = _dataRegion[currentIndex].entries[i]; 
						if (strcmp(tmpEntry.name, dirName.c_str()) == 0){
							return true;
						}
					}
					currentIndex = _FAT[blockIndex];
				}
				for (int i = 0; i < 128; i++){
					d_entry tmpEntry = _dataRegion[currentIndex].entries[i]; 
					if (strcmp(tmpEntry.name, dirName.c_str()) == 0){
						return true;
					}
				}
				return false;
			}			
		}
	} else { //si la cadena no pasa los requisitos
		return false;
	}
}

void FAT::print(string dirName){
	d_entry* d = retrieveDir(dirName);
	cout << d->_cDate << endl;
}

void FAT::changeDirectory(string dirName){
	if (strcmp(dirName.c_str(), "NULL") == 0){
		parentDir = NULL;
		currentDir = NULL;
	} else if (strcmp(dirName.c_str(), "..") == 0){
		if (parentDir == NULL){
			currentDir == NULL;
		} else {

		}
	} else{
		parentDir = currentDir;
		currentDir = retrieveDir(dirName);
	}
}

/*string FAT::cat(string fileName){
	if (currentDir == NULL){
		d_entry* workingFile = retrieveDir(fileName);
		unsigned short currentIndex = 2;
		while (_FAT[currentIndex] != 0xFFFF){ //mientras no se encuentre el bloque final del directorio
			for (int i = 0; i < 128; i++){
				if (!_dataRegion[currentIndex].entries[i]._free){
					d_entry* tmpEntry = &_dataRegion[currentIndex].entries[i]; 
					ss << tmpEntry->name << "," << tmpEntry->_dir << "," << formatDate(tmpEntry->_cDate) << "," << tmpEntry->_size << ",";
				}
			}
			currentIndex = _FAT[currentIndex];
		}
		for (int i = 0; i < 128; i++){
			d_entry tmpEntry = _dataRegion[currentIndex].entries[i]; 
			if (!_dataRegion[currentIndex].entries[i]._free){
				d_entry* tmpEntry = &_dataRegion[currentIndex].entries[i]; 
				ss << tmpEntry->name << "," << tmpEntry->_dir << "," << formatDate(tmpEntry->_cDate) << "," << tmpEntry->_size << ",";
			}
		}
	}
}*/

void FAT::wCat(string fileName){

}

FAT::d_entry* FAT::retrieveDir(string dirName){
	if (strlen(dirName.c_str()) != 0 && strlen(dirName.c_str()) < 10){
		if (currentDir == NULL){ // Si se encuentra buscando en el root
			for (int i = 0; i < 512; i++){ //Revisa las 512 entradas del root
				if (strcmp(_root[i].name, dirName.c_str()) == 0 ){ // Si el nombre de la d_entry es el mismo que el dirName dado
					d_entry* rootDir = &_root[i];
					return rootDir;
				} 
			}
			return NULL;
		} else { //Si se encuentra buscando en el directorio Actual
			d_entry* temp = currentDir;
			unsigned short blockIndex = temp->_cluster;
			if (_FAT[blockIndex] == 0xFFFF ){ //si es bloque unico
				for (int i = 0; i < 128; i++){
					d_entry* tmpEntry = &_dataRegion[blockIndex].entries[i]; 
					if (strcmp(tmpEntry->name, dirName.c_str()) == 0){
						return tmpEntry;
					}
				}
				return NULL;
			} else { //si son varios clusters
				unsigned short currentIndex = blockIndex;
				while (_FAT[currentIndex] != 0xFFFF){ //mientras no se encuentre el bloque final del directorio
					for (int i = 0; i < 128; i++){
						d_entry* tmpEntry = &_dataRegion[currentIndex].entries[i]; 
						if (strcmp(tmpEntry->name, dirName.c_str()) == 0){
							return tmpEntry;
						}
					}
					currentIndex = _FAT[blockIndex];
				}
				for (int i = 0; i < 128; i++){
					d_entry* tmpEntry = &_dataRegion[currentIndex].entries[i]; 
					if (strcmp(tmpEntry->name, dirName.c_str()) == 0){
						return tmpEntry;
					}
				}
				return NULL;
			}			
		}
	} else { //si la cadena no pasa los requisitos
		return NULL;
	}
}

int FAT::getblockSize() {
	return 0;
}

bool FAT::searchFile(string fileName){
	if (strlen(fileName.c_str()) != 0 && strlen(fileName.c_str()) < 10){
		d_entry temp = *currentDir;
		unsigned short blockIndex = temp._cluster;

	} else {
		return false;
	}
}

void FAT::checkFreeIndex(){
	for (int i = 0; i < 65536; i++){
		if (_FAT[i] == 0xFFFF){
			if (_dataRegion[i].entries == NULL && _dataRegion[i].buffer == NULL){	
				bool exists = false;
				for (int j = 0; j < 65536; j++){
					if (_FAT[j] == i){
						exists = true;
					}
				}
				if (!exists){ 
					freeIndex.push(i);
				}
			}
		}
	}
}

string FAT::formatDate(time_t theTime){
	struct tm *aTime = localtime(&theTime);
	string c(asctime(aTime));
	c.erase(remove(--c.end(),c.end(), '\n'), c.end());
	return c;
}

string FAT::listDirectory() {
	if (currentDir == NULL){
		stringstream ss;
		ss << "Nombre," << "Tipo," << "Fecha de Creacion," << "Tamaño de Archivo,";
		for (int i = 0; i < 512; i++){
			if (!_root[i]._free){
				d_entry* tmpEntry = &_root[i]; 
				ss << tmpEntry->name << "," << tmpEntry->_dir << "," << formatDate(tmpEntry->_cDate) << "," << tmpEntry->_size << ",";
			}
		}
		string c = ss.str();
		c.erase(remove(--c.end(),c.end(), ','), c.end());
		return c;
	} else {
		stringstream ss;
		ss << "Nombre," << "Tipo," << "Fecha de Creacion," << "Tamaño de Archivo,";
		d_entry* workingDir = currentDir;
		unsigned short currentIndex = workingDir->_cluster;
		while (_FAT[currentIndex] != 0xFFFF){ //mientras no se encuentre el bloque final del directorio
			for (int i = 0; i < 128; i++){
				if (!_dataRegion[currentIndex].entries[i]._free){
						d_entry* tmpEntry = &_dataRegion[currentIndex].entries[i]; 
						ss << tmpEntry->name << "," << tmpEntry->_dir << "," << formatDate(tmpEntry->_cDate) << "," << tmpEntry->_size << ",";
					
				}
			}
			currentIndex = _FAT[currentIndex];
		}
		for (int i = 0; i < 128; i++){
			d_entry tmpEntry = _dataRegion[currentIndex].entries[i]; 
			if (!_dataRegion[currentIndex].entries[i]._free){
					d_entry* tmpEntry = &_dataRegion[currentIndex].entries[i]; 
					ss << tmpEntry->name << "," << tmpEntry->_dir << "," << formatDate(tmpEntry->_cDate) << "," << tmpEntry->_size << ",";
			}
		}
		string c = ss.str();
		c.erase(remove(--c.end(),c.end(), ','), c.end());
		return c;
	}
}

void FAT::initialize(){
	d_entry x;
	x._free = true;
	string c = "NULL";
	strcpy(x.name, c.c_str());
	x._dir = false;
	x._cDate = 0;
	x._cluster = 0xFFFF;	
	x._size =  0xFFFFFFFF;
	strcpy(x.reserved, c.c_str());
	for (int i = 0; i < 512; i++){
		_root[i] = x;
	}
	for (int i = 0; i < 65536; i++){
		_FAT[i] = 0xFFFF;
	}
	currentDir = NULL;
	parentDir = NULL;
}

string FAT::strToBin(string value){
	string ret = "";
	for (std::size_t i = 0; i < value.size(); ++i){
		bitset<8> b(value[i]);
		ret += b.to_string();
	}
	return ret;
}

void FAT::save() {
	string _output = "";
	stringstream ss;
	//STORE ROOT SECTION
	for( int i = 0; i < 512; i++ ){
		if( _root[i]._free ){
			_output += "0x000000";
		}else{
			//STORE IF IS FREE
			_output += _root[i]._free ? "0x000000" : "0x111111";
			_output += ",";

			//STORE NAME
			string str( _root[i].name );
			_output += strToBin(str);
			_output += ",";

			//STORE IF IS A DIRECTORY
			_output += _root[i]._dir ? "0x000010" : "0x000020";
			_output += ",";

			//STORE CREATION DATE
			ss << _root[i]._cDate;
			_output += strToBin( ss.str() );
			_output += ",";

			//STORE CLUSTER
			ss.str("");
			ss << _root[i]._cluster;
			_output += strToBin( ss.str() );
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
			string rese( _root[i].reserved );
			_output += strToBin( rese );
		}
		_output += ";";
	}
	_output += "$";
	
	//STORE FAT SECTION
	for( int i = 0; i < 65535; i++ ){
		ss.str("");
		ss << _FAT[i];
		_output += strToBin( ss.str() );
		_output += ",";
	}
	_output += "$";
	for( int i = 0; i < 65535; i++ ){
		ss.str("");
		ss << _FAT[i];
		_output += strToBin( ss.str() );
		_output += ",";
	}
	_output += "$";

	//STORE DATA REGION
	for( int i = 0; i < 65535; i++ ){
		if( _dataRegion[i].entries == NULL && _dataRegion[i].buffer == NULL ){
			for( int j = 0; j < 32768; j++ )
				_output += "0";
		}else if( _dataRegion[i].entries == NULL && _dataRegion[i].buffer != NULL ){
			string out( _dataRegion[i].buffer );
			_output += out;
		}else if( _dataRegion[i].entries != NULL && _dataRegion[i].buffer == NULL ){
			for( int j = 0; j < 128; j++ ){
				//STORE IF IS FREE
				_output += _dataRegion[i].entries[j]._free ? "0x000000" : "0x111111";
				_output += ",";

				//STORE NAME
				string str( _dataRegion[i].entries[j].name );
				_output += strToBin(str);
				_output += ",";

				//STORE IF IS A DIRECTORY
				_output += _dataRegion[i].entries[j]._dir ? "0x000010" : "0x000020";
				_output += ",";

				//STORE CREATION DATE
				ss << _dataRegion[i].entries[j]._cDate;
				_output += strToBin( ss.str() );
				_output += ",";

				//STORE CLUSTER
				ss.str("");
				ss << _dataRegion[i].entries[j]._cluster;
				_output += strToBin( ss.str() );
				_output += ",";

				//STORE SIZE
				unsigned int mask = 1 << (sizeof(int)*8-1);
				for( int x = 0; x < sizeof(int)*8; x++ ){
					if( (_dataRegion[i].entries[j]._size & mask) == 0 )
						_output += '0';
					else
						_output += '1';
					mask >>= 1;
				}
				_output += ",";

				//STORE RESERVED
				string rese( _dataRegion[i].entries[j].reserved );
				_output += strToBin( rese );
				if( j < 127 )
					_output += "#";
			}
		}
		_output += ";";
	}
	
	//WRITE IN FILE
	remove("OS.fat");
	//ofstream _file;
	//_file.open( "OS.fat", ofstream::out );
	//_file << _output;
	//_file.close();
	FILE * pFile;
	pFile = fopen ("OS.fat", "wb");
	fwrite (_output.c_str() , sizeof(char), 268713984, pFile);
	fclose (pFile);
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