#include "proyecto.h"

FAT::FAT(){
	initialize();
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
							freeIndex.pop();
							return true;
						}
					}
					freeIndex.pop();
					currentIndex = freeIndex.front();
					_FAT[currentDir->_cluster] = currentIndex;

					
					_dataRegion[currentIndex].entries[0] = newDir;

					_dataRegion[newDir._cluster].entries = new d_entry[128];
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
							freeIndex.pop();
							return true;
						}
					}
					freeIndex.pop();
					currentIndex = freeIndex.front();
					_FAT[currentDir->_cluster] = currentIndex;

					
					_dataRegion[currentIndex].entries[0] = newDir;

					_dataRegion[newDir._cluster].entries = new d_entry[128];
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
					d_entry tmpEntry = _dataRegion[blockIndex].entries[i]; 
					if (strcmp(tmpEntry.name, dirName.c_str()) == 0){
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

int FAT::getblockSize() {
	return 0;
}

bool FAT::searchFile(string fileName){
	if (strlen(fileName.c_str()) != 0 && strlen(fileName.c_str()) > 10){
		d_entry temp = *currentDir;
		unsigned short blockIndex = temp._cluster;

	} else {
		return false;
	}
}

void FAT::checkFreeIndex(){
	for (int i = 0; i < 65535; i++){
		if (_FAT[i] == 0xFFFF){
			if (_dataRegion[i].entries == NULL && _dataRegion[i].buffer == NULL){	
				bool exists = false;
				for (int j = 0; j < 65535; j++){
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
	for (int i = 0; i < 65535; i++){
		_FAT[i] = 0xFFFF;
	}
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
			string rese( _root[i]._reserved );
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
				for( int j = 0; j < sizeof(int)*8; j++ ){
					if( (_dataRegion[i].entries[j]._size & mask) == 0 )
						_output += '0';
					else
						_output += '1';
					mask >>= 1;
				}
				_output += ",";

				//STORE RESERVED
				string rese( _dataRegion[i].entries[j]._reserved );
				_output += strToBin( rese );
			}
			_output += "#";
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