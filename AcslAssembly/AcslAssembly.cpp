// AcslAssembly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
using namespace std;
//OPCODE storage for retrieval
const string valid_opcodes[] = {"LOAD", "STORE", "ADD", "SUB", "MULT", "DIV", "BG", "BE", "BL", "BU", "READ", "PRINT", "DC", "END"};

//Structure defining each line of the assembly
struct program_line {
    string label;
    string opcode;
    string loc;
};

//Intepreter class-stores variables, ACC and opcode functions
class acsl_intepreter {
    int ACC = 0;
    map<string, int> variables;
public:
    ~acsl_intepreter() {
        cout << "ACC = " << ACC << "\n";
        for (auto l : variables) {
            cout << l.first << " = " << l.second << "\n";
        }
    }
    bool execute_program(const vector<program_line>& program) {
        size_t current_line_number = 0;
        while (current_line_number < program.size()) {
            auto& current_program_line = program[current_line_number];
            if (current_program_line.opcode == "DC") {
                char* endp = nullptr;
                long value = strtol(current_program_line.loc.c_str(), &endp, 10);
                if (*endp != 0) {
                    cout << "Parsing error at line: " << current_line_number + 1 << "\n";
                    return false;
                }
                variables[current_program_line.label] = value;
                current_line_number++;
            }
            else if (current_program_line.opcode == "LOAD") {
                if (current_program_line.loc.c_str()[0] == '=') {
                    char* endp = nullptr;
                    long value = strtol(current_program_line.loc.c_str() + 1, &endp, 10);
                    if (*endp != 0) {
                        cout << "Parsing error at line: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    ACC = value;
                }
                else {
                    auto it = variables.find(current_program_line.loc);
                    if (it == variables.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    else {
                        ACC = it->second;
                    }
                }
                current_line_number++;
            }
            else if (current_program_line.opcode == "STORE") {
                auto it = variables.find(current_program_line.loc);
                if (it == variables.end()) {
                    variables[current_program_line.loc] = ACC;
                }
                else {
                    it->second = ACC;
                }
                current_line_number++;
            }
            else if (current_program_line.opcode == "BG") {
                if (ACC > 0) {
                    auto it = find_if(program.begin(), program.end(), [&current_program_line](const program_line& p) {
                        return p.label == current_program_line.loc;
                        });
                    if (it == program.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    current_line_number = it - program.begin();
                }
                else {
                    current_line_number++;
                }
            }
            else if (current_program_line.opcode == "BE") {
                if (ACC == 0) {
                    auto it = find_if(program.begin(), program.end(), [&current_program_line](const program_line& p) {
                        return p.label == current_program_line.loc;
                        });
                    if (it == program.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    current_line_number = it - program.begin();
                    /*int i = 0;
                    for (auto& line : program) {
                        if (line.label == current_program_line.loc) {
                            current_line_number = i;
                        }
                        else {
                            i++;
                        }
                    }
                    if (i == program.size()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }*/
                }
                else {
                    current_line_number++;
                }
            }
            else if (current_program_line.opcode == "BL") {
                if (ACC < 0) {
                    auto it = find_if(program.begin(), program.end(), [&current_program_line](const program_line& p) {
                        return p.label == current_program_line.loc;
                        });
                    if (it == program.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    current_line_number = it - program.begin();
                }
                else {
                    current_line_number++;
                }
            }
            else if (current_program_line.opcode == "BU") {
                auto it = find_if(program.begin(), program.end(), [&current_program_line](const program_line& p) {
                    return p.label == current_program_line.loc;
                    });
                if (it == program.end()) {
                    cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                    return false;
                }
                current_line_number = it - program.begin();
            }
            else if (current_program_line.opcode == "READ") {
                int input;
                cout << "Enter an integer to READ:\n";
                try
                {
                    cin >> input;
                }
                catch (const std::exception&)
                {
                    cout << "Not an integer input\n";
                }
                variables[current_program_line.loc] = input;
                current_line_number++;
            }
            else if (current_program_line.opcode == "PRINT") {
                if (current_program_line.loc.c_str()[0] == '=') {
                    char* endp = nullptr;
                    long value = strtol(current_program_line.loc.c_str() + 1, &endp, 10);
                    if (*endp != 0) {
                        cout << "Parsing error at line: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    cout << value << "\n";
                    current_line_number++;
                }
                else {
                    auto it = variables.find(current_program_line.loc);
                    if (it == variables.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    else {
                        cout << it->second << "\n";
                        current_line_number++;
                    }
                }
            }
            else if (current_program_line.opcode == "MULT") {
                if (current_program_line.loc.c_str()[0] == '=') {
                    char* endp = nullptr;
                    long value = strtol(current_program_line.loc.c_str() + 1, &endp, 10);
                    if (*endp != 0) {
                        cout << "Parsing error at line: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    ACC = ACC * value;
                    current_line_number++;
                }
                else {
                    auto it = variables.find(current_program_line.loc);
                    if (it == variables.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    else {
                        ACC = ACC * (it->second);
                        current_line_number++;
                    }
                }
            }
            else if (current_program_line.opcode == "ADD") {
                if (current_program_line.loc.c_str()[0] == '=') {
                    char* endp = nullptr;
                    long value = strtol(current_program_line.loc.c_str() + 1, &endp, 10);
                    if (*endp != 0) {
                        cout << "Parsing error at line: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    ACC = ACC + value;
                    current_line_number++;
                }
                else {
                    auto it = variables.find(current_program_line.loc);
                    if (it == variables.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    else {
                        ACC = ACC + (it->second);
                        current_line_number++;
                    }
                }
            }
            else if (current_program_line.opcode == "SUB") {
                if (current_program_line.loc.c_str()[0] == '=') {
                    char* endp = nullptr;
                    long value = strtol(current_program_line.loc.c_str() + 1, &endp, 10);
                    if (*endp != 0) {
                        cout << "Parsing error at line: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    ACC = ACC - value;
                    current_line_number++;
                }
                else {
                    auto it = variables.find(current_program_line.loc);
                    if (it == variables.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    else {
                        ACC = ACC - (it->second);
                        current_line_number++;
                    }
                }
            }
            else if (current_program_line.opcode == "DIV") {
                if (current_program_line.loc.c_str()[0] == '=') {
                    char* endp = nullptr;
                    long value = strtol(current_program_line.loc.c_str() + 1, &endp, 10);
                    if (*endp != 0) {
                        cout << "Parsing error at line: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    ACC = ACC / value;
                    current_line_number++;
                }
                else {
                    auto it = variables.find(current_program_line.loc);
                    if (it == variables.end()) {
                        cout << "error: unrecognized variable at line number: " << current_line_number + 1 << "\n";
                        return false;
                    }
                    else {
                        ACC = ACC / (it->second);
                        current_line_number++;
                    }
                }
            }
            else if (current_program_line.opcode == "END") {
                current_line_number++;
                return true;
            }
        }
        return true;
    }
};


//Next-non-white-space algorithm for parsing
const char* next_non_white_space(const char* s) {
    while ((*s == ' ' || *s == '\t') && *s != '\0') {
        s++;
    }
    return s;
}

//Next-white-space algorithm for parsing
const char* next_white_space(const char* s) {
    while (*s != ' ' && *s != '\t' && *s != '\0') {
        s++;
    }
    return s;
}

//tokenize function utilizing above two functions and pointer arithmetic
vector<string> tokenize(string& s) {
    vector<string> output;
    const char* it = s.c_str();
    while (*it != 0) {
        const char* n = next_non_white_space(it);
        if (*n == 0) {
            break;
        }
        it = next_white_space(n);
        string token(n, it - n);
        output.push_back(token);
    }
    return output;

}

int main(int argc, char** argv)
{
    //Command line call
    if (argc == 1) {
        cout << "Provide a file name\n";
        return 0;
    }
    ifstream fs(argv[1], fstream::in);
    if (!fs) {
        cout << "Unable to open file\n";
        return -1;
    }
    vector<program_line> program;
    string line;
    acsl_intepreter intepreter;
    set<string> opcode_set(valid_opcodes, valid_opcodes + (sizeof(valid_opcodes) / sizeof(valid_opcodes[0])));
    //Loop to create a vector of assebly lines
    while (fs.good()) {
        getline(fs, line);
        vector<string> tokens = tokenize(line);
        if (tokens.size() == 3) {
            program.push_back(program_line{ tokens[0], tokens[1], tokens[2] });
        }
        else if (tokens.size() == 2) {
            if (opcode_set.find(tokens[0]) != opcode_set.end()) {
                program.push_back(program_line{ "", tokens[0], tokens[1] });
            }
            else if (opcode_set.find(tokens[1]) != opcode_set.end()) {
                program.push_back(program_line{ tokens[0], tokens[1], ""});
            }
            else {
                cout << "invalid token at line: " << program.size() + 1 << "\n";
            }
        }
        else if (tokens.size() == 1) {
            program.push_back(program_line{ "", tokens[0], ""});
        }
        else {
            cout << "invalid-too few or too many tokens at line: " << program.size() + 1 << "\n";
        }
    }
    intepreter.execute_program(program);
}

