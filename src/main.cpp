#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <sys/stat.h>

using namespace std;

// Tokenization function
vector<string> tokenize(const string& input) {
    vector<string> tokens;
    istringstream iss(input);
    string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Map for built-in shell commands
map<string, int> shell_builtins = {
    {"echo", 1},
    {"exit", 1},
    {"type", 1},
    {"pwd", 1},
    {"cd", 1},
};

// Function to get relative path if a command exists in the system PATH
string getFilePath(const string& command) {
    string path_env = getenv("PATH");
    istringstream ss(path_env);
    string path;
    
    while (getline(ss, path, ':')) {
        string abs_path = path + "/" + command;
        struct stat sb;
        
        if (stat(abs_path.c_str(), &sb) == 0) {
            return abs_path;
        }
    }
    return "";
}

// Function to handle echo command with tokenization
void handleEcho(const vector<string>& tokens) {
    if (tokens.size() <= 1) {
        cout << endl;
        return;
    }
    
    string output;
    for (size_t i = 1; i < tokens.size(); ++i) {
        output += tokens[i] + " ";
    }
    
    // Remove trailing space
    if (!output.empty()) {
        output.pop_back();
    }
    
    cout << output << endl;
}

// Function to handle type command with tokenization
void handleType(const vector<string>& tokens) {
    if (tokens.size() < 2) {
        cout << "type: missing argument\n";
        return;
    }
    
    string command = tokens[1];
    
    if (shell_builtins.count(command)) {
        cout << command << " is a shell builtin\n";
    } else {
        string command_path = getFilePath(command);
        if (!command_path.empty()) {
            cout << command << " is " << command_path << endl;
        } else {
            cout << command << ": not found\n";
        }
    }
}

// Function to handle cd command with tokenization
void handleCd(const vector<string>& tokens) {
    string path = (tokens.size() > 1) ? tokens[1] : getenv("HOME");
    
    try {
        if (path[0] == '/') {
            filesystem::current_path(path);
        } else if (path[0] == '.') {
            path = filesystem::current_path().string() + '/' + path;
            string cwd = filesystem::canonical(path);
            filesystem::current_path(cwd);
        } else {
            filesystem::current_path(path);
        }
    } catch (const filesystem::filesystem_error& e) {
        cout << "cd: " << path << ": No such file or directory\n";
    }
}

int main() {
    cout << unitbuf;
    cerr << unitbuf;

    while (true) {
        cout << "$ ";
        
        string input;
        getline(cin, input);
        
        vector<string> tokens = tokenize(input);
        
        if (tokens.empty()) continue;
        
        string command = tokens[0];
        
        if (command == "exit") {
            break;
        } else if (command == "echo") {
            handleEcho(tokens);
        } else if (command == "type") {
            handleType(tokens);
        } else if (command == "pwd") {
            cout << filesystem::current_path().string() << endl;
        } else if (command == "cd") {
            handleCd(tokens);
        } else {
            string command_path = getFilePath(command);
            if (!command_path.empty()) {
                // Reconstruct command with all tokens
                string full_command;
                for (const auto& token : tokens) {
                    full_command += token + " ";
                }
                system(full_command.c_str());
            } else {
                cout << command << ": not found\n";
            }
        }
    }
    
    return 0;
}