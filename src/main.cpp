#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>

using namespace std;

// Function to check if a command exists in the system PATH
string getFilePath(const string& command) {
    string path_env = getenv("PATH");
    stringstream ss(path_env);
    string path;
    
    // Iterate through all directories in the PATH
    while (getline(ss, path, ':')) {
        string abs_path = path + "/" + command;
        struct stat sb;
        
        // Check if the command exists in the current directory
        if (stat(abs_path.c_str(), &sb) == 0) {
            return abs_path;  // Return the absolute path if the command exists
        }
    }
    return "";  // Return empty string if command is not found
}

int main() {
    // Ensure the output is flushed immediately after each statement
    cout << unitbuf;
    cerr << unitbuf;

    // Map for built-in shell commands
    map<string, int> shell_builtins = {
        {"echo", 1},
        {"exit", 1},
        {"type", 1},
        {"pwd", 1},
        {"cd" , 1}
    };

    // Start a loop to simulate the shell
    while (true) {
        cout << "$ ";  // Prompt the user for input

        string input;
        getline(cin, input);  // Get the user input

        // Extract the command and arguments
        string command = input.substr(0, input.find(" "));
        input.erase(0, input.find(" ") + 1);

        // Handle the "exit" command to break the loop and terminate the program
        if (command == "exit") {
            return 0;
        }
        // Handle the "echo" command 
        else if (command == "echo") {
          string result = "";
          int i = 0;

          while (i < input.length()){
            string temp = "";
            // handling single-quote message
            if(input[i] == '\''){
              i++;
              while(i < input.length()){
                temp += input[i++];
                if(i > 0 && input[i] == '\'' && input[i-1] != '\\'){
                  break;
                }
              }
              if (i < input.length() && input[i] == '\'') {
                i++; // Skip the closing single quote
              }
              cout << temp;
            }
            // hadling double quote message
            else if(input[i] == '\"'){
              i++;
              while(i<input.length() && input[i] != '\"'){
                if(i+1 < input.length() && input[i] == '\\'){
                  temp += input[i+1];
                  i+=2;
                }
                else{
                  temp += input[i++];
                }
              }
              if(i < input.length() && input[i] == '\"'){
                i++; // Skip the last cloing double quote
              }
              cout << temp;
            }
            // handling without single-quote message
            else{
              while(i < input.length() && input[i] != ' ' && input[i] != '\''){
                if(i+1 < input.length() && input[i] == '\\'){
                  temp += input[i+1];
                  i+=2;
                }
                else{
                  temp += input[i++];
                }
              }
              // Skipping the spaces
              while(i < input.length() && input[i] == ' '){
                i++;
              }
              cout << temp << " ";
            }
          }
            cout << endl;
        }
        // Handle the "type" command
        else if (command == "type") {
            if (shell_builtins[input]) {
                cout << input << " is a shell builtin\n";
            } else {
                string command_path = getFilePath(input);
                if (!command_path.empty()) {
                    cout << input << " is " << command_path << endl;
                } else {
                    cout << input << ": not found\n";
                }
            }
        }
        // Handle the "pwd" command
        else if(command == "pwd"){
          cout << filesystem::current_path().string() << endl;
        }
        // Handle the "cd" command
        else if(command == "cd"){
          string path = input;
          // Navigating in absoulute path
          if(path[0] == '/'){
            try{
              filesystem::current_path(path);
            }catch(const filesystem::filesystem_error& e){
              cout << "cd: " << path << ": No such file or directory\n";
            }
          }
          // Navigating in relative path ./, ../, ./dir
          else if(path[0] == '.'){
            try{
              path = filesystem::current_path().string() + '/' + path; 
              string cwd = filesystem::canonical(path);
              filesystem::current_path(cwd);
            }catch(const filesystem::filesystem_error& e){
              cout << "cd: " << path << ": No such file or directory\n";
            }
          }
          else{
            string home_dir_path = getenv("HOME");
            filesystem::current_path(home_dir_path);
          }
        }
        // For other commands, try to find their path and execute them
        else {
            string command_path = getFilePath(command);
            if (!command_path.empty()) {
                // Execute the command with arguments
                string full_command = command + ' ' + input;
                system(full_command.c_str());
            } else {
                cout << command << ": not found\n";
            }
        }
    }
}
