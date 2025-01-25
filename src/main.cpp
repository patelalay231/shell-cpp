#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>

using namespace std;

// Function to get absolute path if a command exists in the system PATH
string getFilePath(const string& command) {
    string path_env = getenv("PATH");
    stringstream ss(path_env);
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

// Function to escape special characters in shell commands
string escapeShell(const string& input) {
    string escaped = input;
    size_t pos = 0;
    while ((pos = escaped.find_first_of("\"'\\", pos)) != string::npos) {
        escaped.insert(pos, "\\");
        pos += 2; // Move past the escaped character
    }
    return escaped;
}

int main() {
    cout << unitbuf;
    cerr << unitbuf;

    map<string, int> shell_builtins = {
        {"echo", 1},
        {"exit", 1},
        {"type", 1},
        {"pwd", 1},
        {"cd", 1}
    };

    while (true) {
        cout << "$ ";

        string input;
        getline(cin, input);

        // Extract command and arguments
        string command = input.substr(0, input.find(" "));
        string arguments = input.substr(input.find(" ") + 1);

        // Exit command
        if (command == "exit") {
            return 0;
        }
        // Echo command
        else if (command == "echo") {
            string result = arguments;
            cout << result << endl;
        }
        // Type command
        else if (command == "type") {
            if (shell_builtins[arguments]) {
                cout << arguments << " is a shell builtin\n";
            } else {
                string command_path = getFilePath(arguments);
                if (!command_path.empty()) {
                    cout << arguments << " is " << command_path << endl;
                } else {
                    cout << arguments << ": not found\n";
                }
            }
        }
        // PWD command
        else if (command == "pwd") {
            cout << filesystem::current_path().string() << endl;
        }
        // CD command
        else if (command == "cd") {
            string path = arguments;
            try {
                filesystem::current_path(path);
            } catch (const filesystem::filesystem_error& e) {
                cout << "cd: " << path << ": No such file or directory\n";
            }
        }
        // Executing other commands
        else {
            string escaped_command = escapeShell(command + " " + arguments);
            int ret_code = system(escaped_command.c_str());
            if (ret_code != 0) {
                cout << "Error: Failed to execute command '" << escaped_command << "'\n";
            }
        }
    }
}
