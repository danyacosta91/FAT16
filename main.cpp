#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

string _commando;
string _rutaMKDIR;
string _PWD;

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters){
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos){
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

string sePuedeCompletar(string line, vector<string>& tokens){
    int cont = 0;
    string val = "";
    bool _bandera = true;
    for( auto it = tokens.begin(); it != tokens.end(); it++ ){
        val = *it;
        cont = 0;
        for( int i = 0; i < val.length(); i++ ){
            if( cont < line.length() && line.at(cont) == val.at(i) ){
            	if( cont != i ){
            		cont = 0;
            		break;
            	}
                cont++;
            }
        }
        if( cont > 0 && (cont/val.length()-1)*100 >= 50 )
            return val;
    }
    return "";
}

int main(){
	initscr();
	char tecla;
    string _line = "", _tmp = "", _get = "Mi_sh>";
    vector<string> _TComandos;
    _TComandos.push_back("ls");_TComandos.push_back("cat");_TComandos.push_back("mkdir");
    _TComandos.push_back("rmdir");_TComandos.push_back("delete");_TComandos.push_back("cd");
    _TComandos.push_back("-l");_TComandos.push_back("help");

	do{
		erase();
        printw(_get.c_str());
        tecla = getch();
         if( tecla == 9 ){
			_tmp = sePuedeCompletar(_line, _TComandos);
            if( strcmp( _tmp.c_str(), "") != 0 ){
                erase();
                _line = _tmp;
                _get = "Mi_sh>" + _tmp;
            }
         }else if( tecla == 10 ){

         }else if( tecla == 127 ){
            if( _line.length() > 0 ){
                _line = _line.substr( 0, _line.length()-1 );
                _get += "\nMi_sh>" + _line;
            }
        }else{
        	_line += tecla;
        	_get += tecla;
        }
	}while( strcmp(_line.c_str(), "exit") != 0 );

	endwin ();
	return 0;
}