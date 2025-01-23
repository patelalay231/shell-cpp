#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>

using namespace std;

void isExist(const char* command) {
    char* pathEnv = getenv("PATH");
    if (!pathEnv) return;

    string path(pathEnv);
    stringstream ss(path);
    string directory;

    while (getline(ss, directory, ':')) {
        string fullPath = directory + "/" + command;
        struct stat sb;
        cout << fullPath << endl;
        if (stat(fullPath.c_str(), &sb) == 0)
            cout << "The path is valid!";
            break;
      }
    
    std::cout << "Command not found in PATH" << std::endl;
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

    
    string command = input.substr(0,4);
    if(command == "exit"){
      return 0;
    }
    else if(command == "echo"){
      cout << input.substr(5,input.length()) << endl;
    }
    else if(command == "type"){
      string check_command = input.substr(5,input.length());
      isExist(check_command.c_str());
    }
    else{
      cout << input <<": command not found\n";
    }
  }
}




