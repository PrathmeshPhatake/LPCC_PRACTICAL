#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

// Structure to store macro definition
struct Macro {
    string name;
    vector<string> parameters;
    vector<string> instructions;
};

// Class for the macro processor
class MacroProcessor {
private:
    // Stores macro definitions (name -> macro)
    map<string, Macro> macroTable;
    // Stores the arguments passed to a macro during expansion
    map<string, string> argumentTable;
    // Pass 1 output (MDT - Macro Definition Table)
    vector<string> intermediateCode;
    // Input program
    vector<string> inputProgram;
    // Final expanded output
    vector<string> expandedCode;

public:
    void readInput(const string& input) {
        istringstream iss(input);
        string line;
        while (getline(iss, line)) {
            if (!line.empty()) {
                inputProgram.push_back(line);
            }
        }
    }

    // First pass - Build the macro table
    void pass1() {
        bool inMacroDefinition = false;
        Macro currentMacro;

        for (string& line : inputProgram) {
            istringstream iss(line);
            string firstToken;
            iss >> firstToken;

            if (firstToken == "MACRO") {
                inMacroDefinition = true;
                string macroName, restOfLine;
                iss >> macroName;
                getline(iss, restOfLine);

                currentMacro = Macro();
                currentMacro.name = macroName;

                // Extract parameters
                istringstream paramStream(restOfLine);
                string param;
                while(paramStream >> param) {
                    // Remove commas
                    if (param.back() == ',') {
                        param.pop_back();
                    }
                    currentMacro.parameters.push_back(param);
                }

                intermediateCode.push_back("MACRO " + macroName + " " + restOfLine);
            }
            else if (firstToken == "MEND") {
                inMacroDefinition = false;
                currentMacro.instructions.push_back(line);
                macroTable[currentMacro.name] = currentMacro;
                intermediateCode.push_back("MEND");
            }
            else if (inMacroDefinition) {
                currentMacro.instructions.push_back(line);
                intermediateCode.push_back(line);
            }
            else {
                // Regular instruction, just pass through to intermediate code
                intermediateCode.push_back(line);
            }
        }
    }

    // Second pass - Expand the macros
    void pass2() {
        for (size_t i = 0; i < intermediateCode.size(); i++) {
            string line = intermediateCode[i];
            istringstream iss(line);
            string firstToken;
            iss >> firstToken;

            // Skip macro definitions in the expansion phase
            if (firstToken == "MACRO") {
                // Skip all lines until MEND
                while (i < intermediateCode.size() && intermediateCode[i] != "MEND") {
                    i++;
                }
                continue;
            }

            // Check if the line calls a macro
            if (macroTable.find(firstToken) != macroTable.end()) {
                // It's a macro call, expand it
                expandMacro(line,line);
            }
            else {
                // Regular instruction, add to expanded code
                expandedCode.push_back(line);
            }
        }
    }

    void expandMacro(const string& line, const string& originalCall = "") {
    istringstream iss(line);
    string macroName;
    iss >> macroName;

    Macro& macro = macroTable[macroName];
    map<string, string> savedArgumentTable = argumentTable;

    string restOfLine;
    getline(iss, restOfLine);

    vector<string> args;
    string currentArg;
    for (char c : restOfLine) {
        if (c == ',') {
            if (!currentArg.empty()) {
                currentArg.erase(0, currentArg.find_first_not_of(" \t"));
                currentArg.erase(currentArg.find_last_not_of(" \t") + 1);
                args.push_back(currentArg);
                currentArg.clear();
            }
        } else {
            currentArg += c;
        }
    }
    if (!currentArg.empty()) {
        currentArg.erase(0, currentArg.find_first_not_of(" \t"));
        currentArg.erase(currentArg.find_last_not_of(" \t") + 1);
        args.push_back(currentArg);
    }

    for (size_t i = 0; i < min(args.size(), macro.parameters.size()); i++) {
        argumentTable[macro.parameters[i]] = args[i];
    }

    vector<string> expandedLines;
    for (const string& instr : macro.instructions) {
        if (instr == "MEND" || instr.find("MACRO") == 0) continue;

        string instruction = instr;
        for (const auto& param : macro.parameters) {
            size_t pos = 0;
            while ((pos = instruction.find(param, pos)) != string::npos) {
                instruction.replace(pos, param.length(), argumentTable[param]);
                pos += argumentTable[param].length();
            }
        }

        istringstream expandedIss(instruction);
        string expandedFirstToken;
        expandedIss >> expandedFirstToken;

        if (macroTable.find(expandedFirstToken) != macroTable.end()) {
            // Recursively expand and add call as the annotation
            expandMacro(instruction, instruction);
        } else {
            expandedLines.push_back(instruction);
        }
    }

    // Append macro call as a comment to the last expanded line
    for (size_t i = 0; i < expandedLines.size(); ++i) {
        string lineToAdd = expandedLines[i];
        if (i == expandedLines.size() - 1 && !line.empty()) {
            lineToAdd += "        ; " + (originalCall.empty() ? line : originalCall);
        }
        expandedCode.push_back(lineToAdd);
    }

    argumentTable = savedArgumentTable;
}


    // Print the intermediate code (MDT)
    void printIntermediateCode() {
        cout << "\nIntermediate Code (Pass 1 Output - MDT):\n";
        cout << "------------------------------------------\n";
        for (const string& line : intermediateCode) {
            cout << line << endl;
        }
    }

    // Print the expanded code (Pass 2 Output)
    void printExpandedCode() {
        cout << "\nExpanded Code (Pass 2 Output):\n";
        cout << "------------------------------------------\n";
        for (const string& line : expandedCode) {
            cout << line << endl;
        }
    }

    // Print the macro table
    void printMacroTable() {
        cout << "\nMacro Table:\n";
        cout << "------------------------------------------\n";
        for (const auto& entry : macroTable) {
            cout << "Macro Name: " << entry.first << endl;
            cout << "Parameters: ";
            for (const string& param : entry.second.parameters) {
                cout << param << " ";
            }
            cout << endl;
            cout << "Instructions:" << endl;
            for (const string& instr : entry.second.instructions) {
                cout << "  " << instr << endl;
            }
            cout << "------------------------------------------\n";
        }
    }
};

int main() {
    // Sample input from the problem statement
    string input = 
        "LOAD J\n"
        "STORE M\n"
        "MACRO EST\n"
        "LOAD e\n"
        "ADD d\n"
        "MEND\n"
        "LOAD S\n"
        "MACRO SUB4 ABC\n"
        "LOAD U\n"
        "STORE ABC\n"
        "MEND\n"
        "LOAD P\n"
        "ADD V\n"
        "MACRO ADD7 P4, P5, P6\n"
        "LOAD P5\n"
        "SUB4 XYZ\n"
        "SUB 8\n"
        "SUB 2\n"
        "STORE P4\n"
        "STORE P6\n"
        "MEND\n"
        "EST\n"
        "ADD7 C4, C5, C6\n"
        "SUB4 z\n"
        "END";

    MacroProcessor processor;
    processor.readInput(input);
    
    // First pass - build macro table
    processor.pass1();
    
    // Print intermediate code after pass 1
    processor.printMacroTable();
    processor.printIntermediateCode();
    
    // Second pass - expand macros
    processor.pass2();
    
    // Print expanded code
    processor.printExpandedCode();
    
    return 0;
}