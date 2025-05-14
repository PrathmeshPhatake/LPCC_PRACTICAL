/* Applies simple code optimization techniques like:

Constant folding

Common subexpression elimination

Copy propagation
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <regex>

using namespace std;

bool isConstantExpression(const string& expr, int& result) {
    try {
        regex pattern(R"((\d+)\s*([\+\-\*/])\s*(\d+))");
        smatch match;
        if (regex_match(expr, match, pattern)) {
            int lhs = stoi(match[1]);
            int rhs = stoi(match[3]);
            char op = match[2].str()[0];
            switch (op) {
                case '+': result = lhs + rhs; return true;
                case '-': result = lhs - rhs; return true;
                case '*': result = lhs * rhs; return true;
                case '/': if (rhs != 0) { result = lhs / rhs; return true; }
            }
        }
    } catch (regex_error& e) {
        cerr << "Regex error in constant evaluation: " << e.what() << endl;
    }
    return false;
}

string trim(const string& s) {
    auto start = s.find_first_not_of(" \t");
    auto end = s.find_last_not_of(" \t");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

int main() {
    ifstream infile("input.txt");
    ofstream outfile("optimized.txt");

    if (!infile.is_open()) {
        cerr << "Error: Cannot open input file.\n";
        return 1;
    }

    unordered_map<string, string> exprMap;
    unordered_map<string, string> copyMap;
    vector<string> optimizedCode;

    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string lhs, eq, rhs;
        ss >> lhs >> eq;
        getline(ss, rhs);
        rhs = trim(rhs);

        // Apply copy propagation
        for (auto it = copyMap.begin(); it != copyMap.end(); ++it) {
            size_t pos = rhs.find(it->first);
            if (pos != string::npos) {
                rhs.replace(pos, it->first.length(), it->second);
            }
        }

        int constResult;
        if (isConstantExpression(rhs, constResult)) {
            rhs = to_string(constResult);
        }

        if (exprMap.find(rhs) != exprMap.end()) {
            optimizedCode.push_back(lhs + " = " + exprMap[rhs]);
            copyMap[lhs] = exprMap[rhs];
        } else {
            optimizedCode.push_back(lhs + " = " + rhs);
            exprMap[rhs] = lhs;
            if (regex_match(rhs, regex("^[a-zA-Z_][a-zA-Z0-9_]*$"))) {
                copyMap[lhs] = rhs;
            }
        }
    }

    for (const string& code : optimizedCode) {
        outfile << code << endl;
    }

    infile.close();
    outfile.close();

    cout << "Optimization complete. Output written to 'optimized.txt'\n";
    return 0;
}
