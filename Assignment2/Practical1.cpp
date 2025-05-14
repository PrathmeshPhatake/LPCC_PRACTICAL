#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// Structure to represent a macro
struct Macro {
    string name;                    // Name of the macro
    vector<string> parameters;      // Formal parameters
    vector<string> definition;      // Macro definition statements
    int index;                      // Index in MDT
};

// Structure to represent Macro Name Table (MNT)
struct MNT_Entry {
    string name;                    // Name of the macro
    int index;                      // Index in MDT where this macro starts
    int nesting_level;              // Nesting level of macro
    int num_params;                 // Number of parameters
};

// Class to represent a two-pass macro processor
class MacroProcessor {
private:
    vector<Macro> mdt;              // Macro Definition Table
    vector<MNT_Entry> mnt;          // Macro Name Table
    map<string, vector<int>> ala;   // Argument List Array (macro_name -> parameter indices)

public:
    // Function to process the input file and generate MDT
    void processMacros(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        bool insideMacro = false;
        Macro currentMacro;
        int mdtIndex = 0;

        while (getline(file, line)) {
            // Trim leading and trailing whitespace
            line = trim(line);
            
            if (line.empty()) continue;

            vector<string> tokens = tokenize(line);
            
            if (tokens.size() == 0) continue;

            // Check if line contains MACRO definition
            if (tokens[0] == "MACRO") {
                insideMacro = true;
                currentMacro = Macro();
                currentMacro.index = mdtIndex;
                
                // Parse macro name and parameters
                if (tokens.size() > 1) {
                    currentMacro.name = tokens[1];
                    
                    // Parse parameters if any
                    if (tokens.size() > 2) {
                        string paramStr = line.substr(line.find(tokens[1]) + tokens[1].length());
                        currentMacro.parameters = parseParameters(paramStr);
                    }
                }
            }
            // Check if line contains MEND (end of macro)
            else if (tokens[0] == "MEND") {
                if (insideMacro) {
                    // Add MEND to macro definition
                    currentMacro.definition.push_back(line);
                    
                    // Add macro to MDT
                    mdt.push_back(currentMacro);
                    
                    // Create MNT entry
                    MNT_Entry mntEntry;
                    mntEntry.name = currentMacro.name;
                    mntEntry.index = currentMacro.index;
                    mntEntry.num_params = currentMacro.parameters.size();
                    mntEntry.nesting_level = 0; // Set default nesting level
                    mnt.push_back(mntEntry);
                    
                    // Update MDT index
                    mdtIndex += currentMacro.definition.size();
                    
                    insideMacro = false;
                }
            }
            // Inside a macro definition
            else if (insideMacro) {
                currentMacro.definition.push_back(line);
            }
            // Check if line contains a macro call
            else {
                // For simplicity, we're not processing macro calls here
                // This would be part of the second pass
            }
        }

        // Process macro definitions to determine nesting
        processMacroNesting();
        
        file.close();
    }

    // Determine nesting levels of macros
    void processMacroNesting() {
        // Create a map of macro names for quick lookup
        map<string, int> macroNameToMNTIndex;
        for (size_t i = 0; i < mnt.size(); i++) {
            macroNameToMNTIndex[mnt[i].name] = i;
        }

        // Check each macro definition for calls to other macros
        for (size_t i = 0; i < mdt.size(); i++) {
            Macro& macro = mdt[i];
            
            for (const string& defLine : macro.definition) {
                vector<string> tokens = tokenize(defLine);
                if (tokens.size() == 0) continue;
                
                // Check if the first token is a macro name
                if (macroNameToMNTIndex.find(tokens[0]) != macroNameToMNTIndex.end() &&
                    tokens[0] != "MACRO" && tokens[0] != "MEND") {
                    int calledMacroIdx = macroNameToMNTIndex[tokens[0]];
                    // Update nesting level if necessary
                    mnt[calledMacroIdx].nesting_level = max(mnt[calledMacroIdx].nesting_level, 
                                                         mnt[i].nesting_level + 1);
                }
            }
        }
    }

    // Print the Macro Definition Table
    void printMDT() {
        cout << "\n----- MACRO DEFINITION TABLE (MDT) -----\n";
        cout << "Index\tStatement\n";
        
        int index = 0;
        for (const Macro& macro : mdt) {
            for (const string& line : macro.definition) {
                cout << index++ << "\t" << line << "\n";
            }
        }
    }

    // Print the Macro Name Table
    void printMNT() {
        cout << "\n----- MACRO NAME TABLE (MNT) -----\n";
        cout << "Macro Name\tMDT Index\tNesting Level\tNum Params\n";
        
        for (const MNT_Entry& entry : mnt) {
            cout << entry.name << "\t\t" 
                 << entry.index << "\t\t"
                 << entry.nesting_level << "\t\t"
                 << entry.num_params << "\n";
        }
    }

    // Parse parameters from a parameter string
    vector<string> parseParameters(const string& paramStr) {
        vector<string> params;
        string temp = paramStr;
        
        // Remove any commas and clean up
        replace(temp.begin(), temp.end(), ',', ' ');
        temp = trim(temp);
        
        istringstream iss(temp);
        string param;
        
        while (iss >> param) {
            if (!param.empty()) {
                params.push_back(trim(param));
            }
        }
        
        return params;
    }

    // Helper function to tokenize a string
    vector<string> tokenize(const string& str) {
        vector<string> tokens;
        istringstream iss(str);
        string token;
        
        while (iss >> token) {
            if (!token.empty()) {
                // Remove trailing comma if present
                if (token.back() == ',') {
                    token = token.substr(0, token.length() - 1);
                }
                tokens.push_back(token);
            }
        }
        
        return tokens;
    }

    // Helper function to trim whitespace
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, last - first + 1);
    }

    // Function to write the input code with macro definitions to a temporary file
    void writeInputToFile(const string& content, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error creating temporary file: " << filename << endl;
            return;
        }
        
        file << content;
        file.close();
    }
};

int main() {
    // Hardcoded input macro assembly code
    string inputCode =
        "LOAD J\n"
        "STORE M\n"
        "MACRO EST1\n"
        "LOAD e\n"
        "ADD d\n"
        "MEND\n"
        "MACRO EST ABC\n"
        "EST1\n"
        "STORE ABC\n"
        "MEND\n"
        "MACRO ADD7 P4, P5, P6\n"
        "LOAD P5\n"
        "EST 8\n"
        "SUB4 2\n"
        "STORE P4\n"
        "STORE P6\n"
        "MEND\n"
        "EST\n"
        "ADD7 C4, C5, C6\n"
        "END\n";

    string tempFileName = "temp_macro_input.txt";
    ofstream tempFile(tempFileName);
    if (!tempFile.is_open()) {
        cerr << "Error creating temporary file: " << tempFileName << endl;
        return 1;
    }
    tempFile << inputCode;
    tempFile.close();

    // Process the macro assembly code
    MacroProcessor processor;
    processor.processMacros(tempFileName);

    // Display the MDT and MNT
    processor.printMDT();
    processor.printMNT();

    return 0;
}
