#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

struct LiteralEntry {
    string name;
    string value;
    int length;
    string address;
};

struct SymbolEntry {
    string name;
    string address;
};

bool isLiteral(const string& s) {
    return !s.empty() && s[0] == '=';
}

string cleanLiteralValue(string value) {
    if (value[0] == '=') value = value.substr(1);
    if (value[0] == '\'' && value.back() == '\'') {
        value = value.substr(1, value.length() - 2);
    }
    transform(value.begin(), value.end(), value.begin(), ::toupper);
    return value;
}

pair<vector<LiteralEntry>, vector<SymbolEntry>> generateTables(istream& inputFile) {
    vector<LiteralEntry> literalTable;
    vector<SymbolEntry> symbolTable;
    map<string, bool> literalMap;
    map<string, bool> symbolMap;
    int locationCounter = 0;

    string line;
    while (getline(inputFile, line)) {
        size_t commentPos = line.find(';');
        if (commentPos != string::npos) line = line.substr(0, commentPos);

        istringstream iss(line);
        vector<string> tokens;
        string token;
        while (iss >> token) tokens.push_back(token);

        if (tokens.empty()) continue;

        // Handle START directive
        if (tokens[0] == "START") {
            if (tokens.size() > 1) {
                try {
                    locationCounter = stoi(tokens[1]);
                } catch (...) {
                    cerr << "Warning: Invalid start address, using 0" << endl;
                }
            }
            continue;
        }

        // Skip END directive
        if (tokens[0] == "END") continue;

        // Label check
        if (tokens[0].back() == ':') {
            string label = tokens[0].substr(0, tokens[0].length() - 1);
            if (symbolMap.find(label) == symbolMap.end()) {
                symbolTable.push_back({label, to_string(locationCounter)});
                symbolMap[label] = true;
            }
            tokens.erase(tokens.begin());
        }

        // Check for variable declaration (e.g., X DS 1)
        if (tokens.size() >= 2 && (tokens[1] == "DS" || tokens[1] == "DC")) {
            string varName = tokens[0];
            if (symbolMap.find(varName) == symbolMap.end()) {
                symbolTable.push_back({varName, to_string(locationCounter)});
                symbolMap[varName] = true;
            }
            locationCounter += 1;
            continue;
        }

        // Detect literals
        for (const auto& t : tokens) {
            if (isLiteral(t)) {
                string val = cleanLiteralValue(t);
                if (literalMap.find(val) == literalMap.end()) {
                    literalTable.push_back({
                        "LIT_" + to_string(literalTable.size() + 1),
                        t,
                        1,
                        ""
                    });
                    literalMap[val] = true;
                }
            }
        }

        locationCounter += 1;
    }

    // Assign addresses to literals
    int literalStartAddr = locationCounter;
    for (auto& entry : literalTable) {
        entry.address = to_string(literalStartAddr);
        literalStartAddr += entry.length;
    }

    return {literalTable, symbolTable};
}

void printTables(const vector<LiteralEntry>& literalTable, const vector<SymbolEntry>& symbolTable) {
    cout << "\nLiteral Table:\n";
    cout << "+--------+---------+--------+---------+\n";
    cout << "| Name   | Value   | Length | Address |\n";
    cout << "+--------+---------+--------+---------+\n";
    for (const auto& entry : literalTable) {
        cout << "| " << setw(6) << left << entry.name << " | "
             << setw(7) << left << entry.value << " | "
             << setw(6) << left << entry.length << " | "
             << setw(7) << left << entry.address << " |\n";
    }
    cout << "+--------+---------+--------+---------+\n";

    cout << "\nSymbol Table:\n";
    cout << "+--------+---------+\n";
    cout << "| Name   | Address |\n";
    cout << "+--------+---------+\n";
    for (const auto& entry : symbolTable) {
        cout << "| " << setw(6) << left << entry.name << " | "
             << setw(7) << left << entry.address << " |\n";
    }
    cout << "+--------+---------+\n";
}

int main() {
    // Your provided input as string
    string inputCode = R"(START 200
READ X
READ Y
MOVER AREG, ='5'
MOVER BREG, ='6'
ADD AREG, BREG
LOOP: MOVER CREG, X  
ADD CREG, ='1'
COMP CREG, Y
BC LT, LOOP
NEXT: SUB AREG, ='1'
COMP AREG, Y 
BC GT, NEXT
STOP
X DS 1
Y DS 1
END)";

    istringstream inputStream(inputCode);
    auto tables = generateTables(inputStream);
    printTables(tables.first, tables.second);

    return 0;
}
