#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
using namespace std;

struct Symbol {
    string name;
    int address;
    int size;
};

bool isLabel(const string& word) {
    return word.find(':') != string::npos;
}

bool isDS(const string& line) {
    return line.find("DS") != string::npos;
}

bool isEND(const string& line) {
    return line.find("END") != string::npos;
}

int main() {
    vector<Symbol> symbolTable;
    ifstream inputFile("input.txt");
    string line;
    int locationCounter = 0;
    bool started = false;

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input.txt\n";
        return 1;
    }

    while (getline(inputFile, line)) {
        if (line.empty()) continue;

        // Handle START
        if (!started && line.find("START") != string::npos) {
            stringstream ss(line);
            string temp;
            ss >> temp >> locationCounter;
            started = true;
            continue;
        }

        if (!started) continue;

        if (isEND(line)) break;

        stringstream ss(line);
        string first, second, third;
        ss >> first >> second >> third;

        // Label detection
        if (isLabel(first)) {
            string label = first.substr(0, first.find(':'));
            symbolTable.push_back({label, locationCounter, 1});
        }

        // Variable definition (DS)
        if (isDS(line)) {
            string name;
            int size = 1;
            string temp;
            stringstream sds(line);
            sds >> name >> temp >> size;

            symbolTable.push_back({name, locationCounter, size});
            locationCounter += size;
            continue;
        }

        // Otherwise it's a normal instruction
        locationCounter++;
    }

    inputFile.close();

    // Print the symbol table
    cout << "\nCombined Symbol Table:\n";
    cout << "+--------+---------+------+\n";
    cout << "| Name   | Address | Size |\n";
    cout << "+--------+---------+------+\n";
    for (const auto& sym : symbolTable) {
        cout << "| " << setw(6) << left << sym.name
             << " | " << setw(7) << sym.address
             << " | " << setw(4) << sym.size << " |\n";
    }
    cout << "+--------+---------+------+\n";
    cout << "Program ends at address: " << locationCounter << "\n";

    return 0;
}
