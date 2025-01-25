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

// Tokenization function for executable commands with quotes
vector<string> tokenizeExecutable(const string& input) {
    vector<string> tokens;
    bool in_single_quote = false;
    bool in_double_quote = false;
    string current_token;

    for (char c : input) {
        if (c == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
            current_token += c;
        }
        else if (c == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
            current_token += c;
        }
        else if (c == ' ' && !in_single_quote && !in_double_quote) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        }
        else {
            current_token += c;
        }
    }

    if (!current_token.empty()) {
        tokens.push_back(current_token);
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
    
    // Handling quotes and escaping
    string final_output;
    size_t i = 0;
    while (i < output.length()) {
        if (output[i] == '\'' || output[i] == '"') {
            char quote = output[i++];
            string temp;
            while (i < output.length() && output[i] != quote) {
                if (output[i] == '\\' && i+1 < output.length() && (output[i+1] == quote || output[i+1] == '\\')) {
                    temp += output[i+1];
                    i += 2;
                } else {
                    temp += output[i++];
                }
            }
            final_output += temp;
            if (i < output.length()) i++; // skip closing quote
        } else if (output[i] == '\\' && i+1 < output.length()) {
            final_output += output[i+1];
            i += 2;
        } else {
            final_output += output[i++];
        }
    }
    
    cout << final_output << endl;
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
        } else if (command == "'exe" || command == "\"exe") {
            // Executable block handling with quote-aware tokenization
            vector<string> exe_tokens = tokenizeExecutable(input);
            if (exe_tokens.size() > 1) {
                // Skip the first token ('exe or "exe)
                string full_command = exe_tokens[1];
                system(full_command.c_str());
            }
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