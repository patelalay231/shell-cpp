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

// void isExist(const char* command) {
//     char* pathEnv = getenv("PATH");
//     if (!pathEnv) return;

//     string path(pathEnv);
//     stringstream ss(path);
//     string directory;

//     while (getline(ss, directory, ':')) {
//         string fullPath = directory + "/" + command;
//         struct stat sb;
//         if (stat(fullPath.c_str(), &sb) == 0){
//             cout << command << " is " << fullPath << endl;
//             return;
//         }
//       }
    
//     cout << command <<": not found\n";
// }

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

    
    string command = input.substr(0,4);
    if(command == "exit"){
      return 0;
    }
    else if(command == "echo"){
      cout << input.substr(5,input.length()) << endl;
    }
    else if(command == "type"){
      string check_command = input.substr(5,input.length());
      if(mp[check_command]){
        cout << check_command << " is a shell builtin\n";
      }
      else{
        string get_command_path = isExist(check_command);
        if(get_command_path){
          cout << check_command << " is " << fullPath << endl;
        }
        else{
          cout << command <<": not found\n";
        }
      }
    }
    else{
      cout << input <<": command not found\n";
    }
  }
}




