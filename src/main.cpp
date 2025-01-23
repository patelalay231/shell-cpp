#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <filesystem>

using namespace std;

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

void isExist(const std::string& command) {
  char* pathEnv = getenv("PATH");
  if (!pathEnv) return false;

  std::string path(pathEnv);
  size_t pos = 0;
  while ((pos = path.find(':')) != std::string::npos) {
      std::string dir = path.substr(0, pos);
      std::string fullPath = dir + "/" + command;
      
      if (std::filesystem::exists(fullPath) && 
          std::filesystem::is_regular_file(fullPath) && 
          std::filesystem::status(fullPath).permissions() & 
          std::filesystem::perms::owner_exec) {
          cout << "yes";
          return true;
      }
      
      path.erase(0, pos + 1);
  }
}


bool fileExists(const std::string& filename) {
    return std::filesystem::exists(filename) && 
           std::filesystem::is_regular_file(filename) && 
           std::filesystem::status(filename).permissions() & 
           std::filesystem::perms::owner_exec;
}