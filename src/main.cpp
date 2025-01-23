#include <iostream>
#include <string>
#include <map>
using namespace std;
int main() {
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
        cout << check_command <<": not found\n";
      }
    }
    else{
      cout << input <<": command not found\n";
    }
  }
}
