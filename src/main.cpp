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
    if(input == "exit 0"){
      return 0;
    }

    cout << input <<": command not found\n";
  }
}
