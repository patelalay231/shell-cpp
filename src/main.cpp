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

    cout << input <<": command not found\n";
  }
}
