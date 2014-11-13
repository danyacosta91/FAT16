#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "proyecto.h"
using namespace std;

string _currentDir;
string _PWD = "~/";
FAT* _fat;

struct d_entry{
	bool _free;
	char name[10];
	bool _dir;
	time_t _cDate;
	unsigned short _cluster;
	char reserved[6];
	unsigned int _size;		
};

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
	_fat = new FAT();
	initscr();
	start_color();
   init_pair(2, COLOR_BLACK, COLOR_RED );
   init_pair(1, COLOR_BLACK, COLOR_WHITE );
	init_pair(3, COLOR_BLACK, COLOR_BLUE );
	char tecla;
    string _line = "", _tmp = "", _get = _PWD + ":Mi_sh>";
    _currentDir = "/";
    vector<string> _TComandos;
    vector<string> _args;
    _TComandos.push_back("ls");_TComandos.push_back("ls -l");
    _TComandos.push_back("cat");_TComandos.push_back("cat > ");
    _TComandos.push_back("rmdir");_TComandos.push_back("delete");
    _TComandos.push_back("cd");_TComandos.push_back("cd ..");
    _TComandos.push_back("--help");_TComandos.push_back("mkdir");
	 _TComandos.push_back("exit");_TComandos.push_back("clear");

	do{
		attron( COLOR_PAIR(1) | A_BOLD );
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
					_get += "\n5) mkdir <dirName>\n6) rmdir <dirName>\n7) clear\n(tab key for autocomplete)\nMi_sh>";
				}else if( strcmp( _args[0].c_str(), "clear" ) == 0 ){
					erase();
					_get = _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
				}else if( strcmp( _args[0].c_str(), "ls" ) == 0 ){
					vector<d_entry> _returnValues = _fat->listDirectory();
					stringstream ss;
					if( _args.size() > 1 ){
						attron( COLOR_PAIR(1) | A_BOLD );
						printw( "NAME\tTYPE\tDATE\tSIZE\n" );
						for( auto it = _returnValues.begin(); it != _returnValues.end(); it++ ){
							if( (*it)._dir )
								attron( COLOR_PAIR(2) | A_BOLD );
							else
								attron( COLOR_PAIR(1) | A_BOLD );
							ss << (*it).name << "\t" << (*it)._dir == true ? "DIR\t" : "FILE\t" << (*it)._cDate <<  "\t" << (*it)._size << "\n";
							printw( ss.str().c_str() );
							ss.str("");
						}
						_get = _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}else{
						for( auto it = _returnValues.begin(); it != _returnValues.end(); it++ ){
							if( (*it)._dir )
								attron( COLOR_PAIR(2) | A_BOLD );
							else
								attron( COLOR_PAIR(1) | A_BOLD );
							ss << (*it).name << "\t";
							printw( ss.str().c_str() );
							ss.str("");
						}
						_get = _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "cat" ) == 0 ){
					if( _args.size() > 1 ){
						_fat->save();
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "rmdir" ) == 0 ){
					if( _args.size() > 1 ){
						if( _fat->searchDir(_args[1]) ){
							_fat->save();
						}else{
							erase();
							_get = "Error: Name of dir exist!\nType: --help for options.\n" + _PWD + 
							(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
						}
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "mkdir" ) == 0 ){
					if( _args.size() > 1 ){
						if( _fat->searchDir( _args[1] ) == false ){
							_fat->mkdir( _args[1] );
							erase();
							_get = _PWD + 
							(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
							_fat->save();
						}
						else{
							erase();
							_get = "Error: Name of dir exist!\nType: --help for options.\n" + _PWD + 
							(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
						}
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "delete" ) == 0 ){
					if( _args.size() > 1 ){
						_fat->save();
					}else{
						erase();
						_get = "Not enough arguments!\nType: --help for options.\n" + _PWD + 
						(strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
					}
				}else if( strcmp( _args[0].c_str(), "cd" ) == 0 ){
					if( _args.size() > 1 ){
						if( strcmp( _args[1].c_str(), ".." ) == 0 ){
							_args.clear();
							Tokenize( _currentDir, _args, "/" );
							if( _args.size()-1 > 1 ){
								_currentDir = "";
								for( auto it = 0; it < _args.size()-1; it++ ){
									if( it < _args.size()-2 )
										_currentDir += _args[it] + "/";
									else
										_currentDir += _args[it];
								}
							}else{
								_currentDir = _PWD;
								_get = _PWD + ":Mi_sh>";
							}
							_fat->changeDirectory( _args[1] );
						}else{
              if( _fat->searchDir( _args[1] ) ){
                if( strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 )
                  _currentDir = _args[1] + "/";
                else
                  _currentDir += _args[1] + "/";
                _get = _PWD + _currentDir + ":Mi_sh>";
					 _fat->changeDirectory( "null" );
              }else{
                _get = "Error: Dir not found!\n" + _PWD + 
					      (strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? "" : _currentDir) + ":Mi_sh>";
              }
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
			    _get = _PWD + (strcmp( _currentDir.c_str(), _PWD.c_str() ) == 0 ? 
			    "" : _currentDir) + ":Mi_sh>" + _line;
			}
        }else{
        	_line += tecla;
        	_get += tecla;
        }
	}while( strcmp(_line.c_str(), "exit") != 0 );

	endwin ();
	delete _fat;
	return 0;
}