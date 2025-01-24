#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
using namespace std;

string isExist(string command){
    string path_env = getenv("PATH");
    stringstream ss(path_env);
    string path;
    
    while(!ss.eof()){
        getline(ss, path, ':');
        string abs_path = path + "/" + command;
        struct stat sb;
        if(stat(abs_path.c_str(),&sb) == 0){
            return abs_path;
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
    else{
      string get_command_path = isExist(command);
      if(!get_command_path.empty()){
        cout << input << " is " << get_command_path << endl;
      }
      else{
        cout << command <<": not found\n";
      }
      // cout << input <<": command not found\n";
    }
  }
}




