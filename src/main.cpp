#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <filesystem>

using namespace std,filesystem;

string isExist(const char* command){
  char* pathEnv = getenv("PATH");
  if(!pathEnv) return;

  string path(pathEnv);
  stringstream ss(path);

  while(!ss.eof()){
    getline(ss, path, ':');
    string fullpath = directory + "/" + command;
    if(exists(fullpath)){
      return fullpath;
    }
  }

  return "";
}

bool endsWithExtension(const string& filePath, const string extension) {
    if (extension.length() > filePath.length()) {
        return false;
    }
    
    return filePath.compare(filePath.length() - extension.length(), extension.length(), extension) == 0;
}

int main(int argc, char* argv[]) {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  map<string,int> mp;
  mp["echo"] = 1;  
  mp["exit"] = 1;
  mp["type"] = 1;

  while(true){
    cout << "$ ";

    string input;
    getline(cin, input);

    
    string command = input.substr(0,input.find(" "));
    input.erase(0,input.find(" ")+1);
    if(command == "exit"){
      return 0;
    }
    else if(command == "echo"){
      cout << input << endl;
    }
    else if(command == "type"){
      if(mp[input]){
        cout << input << " is a shell builtin\n";
      }
      else{
        string get_command_path = isExist(input);
        if(!get_command_path.empty()){
          cout << input << " is " << get_command_path << endl;
        }
        else{
          cout << command <<": not found\n";
        }
      }
    }
    else if(endsWithExtension(command,".exe")){
      string get_command_path = isExist(command);
      if(!get_command_path.empty()){
        cout << input << " is " << get_command_path << endl;
      }
      else{
        cout << command <<": not found\n";
      }
    }
    else{
      cout << input <<": command not found\n";
    }
  }
}




