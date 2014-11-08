#include "FAT.h"
using namespace std;

// void getBinFDEntry(char* _output, bool _free, char* _name, bool _dir, char* _cDate, char* _cluster, int _size, char* _reserved){
// 	int cont = 0;
// 	//STORE IF IS FREE
// 	string tmpFREE = _free ? "0x000000" : "0x111111";
// 	for( int i = 0; i < 8; i++ ){
// 		_output[i] = tmpFREE[i];
// 	}

// 	//STORE NAME
// 	for( int i = 8; i < 88; i++ ){
// 		_output[i] = _name[cont];
// 		cont++;
// 	}

// 	//STORE IF IS A DIRECTORY
// 	string tmpDIR = _dir ? "0x000010" : "0x000020";
// 	cont = 88;
// 	for( int i = 0; i < 8; i++ ){
// 		_output[cont] = tmpDIR[i];
// 		cont++;
// 	}

// 	//STORE CREATION DATE
// 	cont = 0;
// 	for( int i = 96; i < 160; i++ ){
// 		_output[i] = _cDate[cont];
// 		cont++;
// 	}

// 	//STORE CLUSTER
// 	cont = 0;
// 	for( int i = 160; i < 176; i++ ){
// 		_output[i] = _cluster[cont];
// 		cont++;
// 	}

// 	//STORE SIZE
// 	cont = 176;
// 	unsigned int mask = 1 << (sizeof(int)*8-1);
// 	for( int i = 0; i < sizeof(int)*8; i++ ){
// 		if( (_size & mask) == 0 )
// 			_output[cont] = '0';
// 		else
// 			_output[cont] = '1';
// 		mask >>= 1;
// 		cont++;
// 	}

// 	//STORE RESERVED
// 	cont = 0;
// 	for( int i = 208; i < 256; i++ ){
// 		_output[i] = _reserved[cont];
// 		cont++;
// 	}
// }

int main(){
	FAT* _fat = new FAT();
	cout << "search: " << _fat.searchDir("Hola") << endl;

	free( _fat );
	return 0;
}