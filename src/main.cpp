#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void isExist(string& command);
bool fileExists(const std::string& filename);

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
      isExist(check_command);
    }
    else{
      cout << input <<": command not found\n";
    }
  }
}

void isExist(string& command){
  char* pathEnv = getenv("PATH");
  string path(pathEnv);
  stringstream ss(path);
  string directory;
  
  while (getline(ss, directory, ':')) {
      string filename = directory + '\'' + command;
      if(fileExists(filename)){
        cout << "command found" << endl;
      }
      cout << directory << endl;
  }
}

bool fileExists(const string& filename) {
    std::ifstream file(filename);
    return file.good();
}