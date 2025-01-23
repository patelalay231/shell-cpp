#include <iostream>
#include <string>
using namespace std;
int main() {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  while(true){
    cout << "$ ";

    string input;
    getline(cin, input);

    // exist 0 - program terminates with code/status 0.
    string command = input.substr(0,4);
    if(command == "exit"){
      return 0;
    }
    else if(command == "echo"){
      cout << input.substr(5,input.length()) << endl;
    }
    else{
      cout << input <<": command not found\n";
    }
  }
}
