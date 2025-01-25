#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <vector>

using namespace std;

// Function to split input into tokens
vector<string> tokenize(string input)
{
    const char kSingleQuote = '\'';
    const char kDoubleQuote = '"';
    bool inSingleQuotes = false;
    bool inDoubleQuotes = false;
    bool escapeLiteral = false;
    vector<string> tokens;
    string nextToken = "";
    input.push_back(' '); // we do this so our for loop properly pushs back the last token to the token list
    for (auto c : input)
    {
        if (c == kSingleQuote && !escapeLiteral && !inDoubleQuotes)
        {
            inSingleQuotes = !inSingleQuotes;
        }
        else if (c == kDoubleQuote && !escapeLiteral && !inSingleQuotes)
        {
            inDoubleQuotes = !inDoubleQuotes;
        }
        else if (c == '\\' && !escapeLiteral && !inSingleQuotes)
        {
            escapeLiteral = true;
        }
        else if (c == ' ' && !escapeLiteral && !inSingleQuotes && !inDoubleQuotes)
        {
            if (nextToken != "") // skip on spaces
                tokens.push_back(nextToken);
            nextToken = "";
        }
        else
        {
            nextToken.push_back(c);
            if (escapeLiteral)
                escapeLiteral = false;
        }
    }
    return tokens;
}

// Function to get relative path if a command exists in the system PATH
string getFilePath(const string &command)
{
    string path_env = getenv("PATH");
    stringstream ss(path_env);
    string path;

    // Iterate through all directories in the PATH
    while (getline(ss, path, ':'))
    {
        string abs_path = path + "/" + command;
        struct stat sb;

        // Check if the command exists in the current directory
        if (stat(abs_path.c_str(), &sb) == 0)
        {
            return abs_path; // Return the absolute path if the command exists
        }
    }
    return ""; // Return empty string if command is not found
}

// Function to handle echo command
void handleEcho(const string &input)
{
    vector<string> tokens = tokenize(input);
    string result;
    string fileName;
    bool redirectToFile = false;
    bool redirectToError = false;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        string token = tokens[i];

        // Handle single-quoted strings
        if (token.front() == '\'' && token.back() == '\'')
        {
            result += token.substr(1, token.size() - 2);
        }
        // Handle double-quoted strings
        else if (token.front() == '"' && token.back() == '"')
        {
            result += token.substr(1, token.size() - 2);
        }
        // Handle file redirection (1> or 2>)
        else if (token == "1>" || token == ">")
        {
            fileName = tokens[++i];
            ofstream outputFile(fileName);
            if (outputFile.is_open())
            {
                outputFile << result << endl;
                outputFile.close();
            }
            return;
        }
        else if (token == "2>")
        {
            fileName = tokens[++i];
            ofstream outputFile(fileName);
            if (outputFile.is_open())
            {
                cerr << result << endl;
                outputFile.close();
            }
            return;
        }
        else if(token == "1>>")
        {
            fileName = tokens[++i];
            ofstream outputFile(fileName,ios::app);
            if (outputFile.is_open())
            {
                outputFile << result << endl;
                outputFile.close();
            }
            return;
        }
        else if(token == "2>>")
        {
            fileName = tokens[++i];
            ofstream outputFile(fileName,ios::app);
            if (outputFile.is_open())
            {
                cerr << result << endl;
                outputFile.close();
            }
            return;
        }
        else
        {
            result += token + " ";
        }
    }

    if (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }
    cout << result << endl;
}

int main()
{
    // Ensure the output is flushed immediately after each statement
    cout << unitbuf;
    cerr << unitbuf;

    // Map for built-in shell commands
    map<string, int> shell_builtins = {
        {"echo", 1},
        {"exit", 1},
        {"type", 1},
        {"pwd", 1},
        {"cd", 1}};

    // Start a loop to simulate the shell
    while (true)
    {
        cout << "$ "; // Prompt the user for input

        string input;
        getline(cin, input); // Get the user input

        // Extract the command and arguments
        string command = input.substr(0, input.find(" "));
        if (command != "\'exe" && command != "\"exe")
            input.erase(0, input.find(" ") + 1);
        // Handle the "exit" command to break the loop and terminate the program
        if (command == "exit")
        {
            return 0;
        }
        // Handle the "echo" command
        else if (command == "echo")
        {
            handleEcho(input);
        }
        // Handle the "type" command
        else if (command == "type")
        {
            if (shell_builtins[input])
            {
                cout << input << " is a shell builtin\n";
            }
            else
            {
                string command_path = getFilePath(input);
                if (!command_path.empty())
                {
                    cout << input << " is " << command_path << endl;
                }
                else
                {
                    cout << input << ": not found\n";
                }
            }
        }
        // Handle the "pwd" command
        else if (command == "pwd")
        {
            cout << filesystem::current_path().string() << endl;
        }
        // Handle the "cd" command
        else if (command == "cd")
        {
            string path = input;

            // Navigating in absoulute path
            if (path[0] == '/')
            {
                try
                {
                    filesystem::current_path(path);
                }
                catch (const filesystem::filesystem_error &e)
                {
                    cout << "cd: " << path << ": No such file or directory\n";
                }
            }
            // Navigating in relative path ./, ../, ./dir
            else if (path[0] == '.')
            {
                try
                {
                    path = filesystem::current_path().string() + '/' + path;
                    string cwd = filesystem::canonical(path);
                    filesystem::current_path(cwd);
                }
                catch (const filesystem::filesystem_error &e)
                {
                    cout << "cd: " << path << ": No such file or directory\n";
                }
            }
            else
            {
                string home_dir_path = getenv("HOME");
                filesystem::current_path(home_dir_path);
            }
        }
        // Handle executable
        else if (command == "\'exe" || command == "\"exe")
        {
            system(input.c_str());
        }
        // For other commands, try to find their path and execute them.s
        else
        {
            string command_path = getFilePath(command);
            if (!command_path.empty())
            {
                // Execute the command with arguments
                string full_command = command + ' ' + input;
                system(full_command.c_str());
            }
            else
            {
                cout << command << ": not found\n";
            }
        }
    }
}
