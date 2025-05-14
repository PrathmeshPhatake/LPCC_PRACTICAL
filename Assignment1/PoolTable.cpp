#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <set>
using namespace std;

int main() {
    string input = R"(START 200
READ X
READ Y
MOVER AREG, ='5'
MOVER BREG, ='6'
ADD AREG, BREG
LOOP: MOVER CREG, X  
ADD CREG, ='1'
COMP CREG, Y
BC LT, LOOP
LTORG
NEXT: SUB AREG, ='1'
COMP AREG, Y 
BC GT, NEXT
STOP
X DS 1
Y DS 1
END)";

    vector<string> literalTable;
    vector<int> poolTable;
    set<string> literalSet;

    istringstream inputStream(input);
    string line;
    int literalIndex = 0;
    bool started = false;

    while (getline(inputStream, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string word;

        if (!started && line.find("START") != string::npos) {
            started = true;
            continue;
        }

        if (!started) continue;

        while (ss >> word) {
            if (word.find("='") != string::npos) {
                if (literalSet.find(word) == literalSet.end()) {
                    literalTable.push_back(word);
                    literalSet.insert(word);
                }
            }
        }

        if (line.find("LTORG") != string::npos || line.find("END") != string::npos) {
            poolTable.push_back(literalIndex + 1); // 1-based index
            literalIndex = literalTable.size();
            literalSet.clear(); // start a new pool
        }
    }

    // Output Literal Table
    cout << "\nLiteral Table:\n";
    cout << "+--------+---------+\n";
    cout << "| Index  | Literal |\n";
    cout << "+--------+---------+\n";
    for (size_t i = 0; i < literalTable.size(); ++i) {
        cout << "| " << setw(6) << i + 1 << " | " << setw(7) << literalTable[i] << " |\n";
    }
    cout << "+--------+---------+\n";

    // Output Pool Table
    cout << "\nPool Table:\n";
    cout << "+--------+\n";
    cout << "| Index  |\n";
    cout << "+--------+\n";
    for (size_t i = 0; i < poolTable.size(); ++i) {
        cout << "| " << setw(6) << poolTable[i] << " |\n";
    }
    cout << "+--------+\n";

    return 0;
}
