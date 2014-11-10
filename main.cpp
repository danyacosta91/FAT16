#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

string _currentDir;
string _PWD = "~/";

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
    string _line = "", _tmp = "", _get = _PWD + ":Mi_sh>";
    _currentDir = "/";
    vector<string> _TComandos;
    vector<string> _args;
    _TComandos.push_back("ls");_TComandos.push_back("ls -l");_TComandos.push_back("cat");_TComandos.push_back("cat > ");
    _TComandos.push_back("rmdir");_TComandos.push_back("delete");_TComandos.push_back("cd");_TComandos.push_back("cd ..");
    _TComandos.push_back("--help");_TComandos.push_back("mkdir");_TComandos.push_back("exit");

	do{
		erase();
        printw(_get.c_str());
        tecla = getch();
		if( tecla == 9 ){
			_tmp = sePuedeCompletar(_line, _TComandos);
			if( strcmp( _tmp.c_str(), "") != 0 ){
			    erase();
			    _line = _tmp;
			    _get = _PWD + (strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir)
			    + ":Mi_sh>" + _tmp;
			}
		}else if( tecla == 10 ){
			_args.clear();
			Tokenize( _line, _args, " " );
			if( strcmp( _args[0].c_str(), "exit" ) != 0 ){
				if( strcmp( _args[0].c_str(), "--help" ) == 0 ){
					erase();
					_get = "Command List:\n1) cat <filename> or cat > <filename>\n2)";
					_get += " cd <dirName> or cd or cd ..\n3) delete <filename/dirName>\n4) ls or ls -l";
					_get += "\n5) mkdir <dirName>\n6) rmdir <dirName>\n(tab key for autocomplete)\nMi_sh>";
				}else if( strcmp( _args[0].c_str(), "ls" ) == 0 ){
					if( _args.size() > 1 ){
						//FAT de ls -l
					}else{
						//FAT de ls
					}
				}else if( strcmp( _args[0].c_str(), "cat" ) == 0 ){
					if( _args.size() > 1 ){
						//FAT de cat
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "rmdir" ) == 0 ){
					if( _args.size() > 1 ){
						//FAT de rmdir
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "mkdir" ) == 0 ){
					if( _args.size() > 1 ){
						//FAT de mkdir
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "delete" ) == 0 ){
					if( _args.size() > 1 ){
						//FAT de cat
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "cd" ) == 0 ){
					if( _args.size() > 1 ){
						//FAT de cat
						if( strcmp( _args[1].c_str(), ".." ) == 0 ){
						}else{
							if( strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 )
								_currentDir = _args[1] + "/";
							else
								_currentDir += _args[1] + "/";
							_get = _PWD + _currentDir + ":Mi_sh>";
						}
					}else{
						_currentDir = _PWD;
						_get = _PWD + ":Mi_sh>";
					}
				}else
					_get = "Command not supported!\nType: --help for options.\n" + _PWD + 
					(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
				_line = "";
			}
		}else if( tecla == 127 ){
			if( _line.length() > 0 ){
			    _line = _line.substr( 0, _line.length()-1 );
			    _get += "\n" + _PWD + (strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? 
			    "" : _currentDir) + ":Mi_sh>" + _line;
			}
        }else{
        	_line += tecla;
        	_get += tecla;
        }
	}while( strcmp(_line.c_str(), "exit") != 0 );

	endwin ();
	return 0;
}