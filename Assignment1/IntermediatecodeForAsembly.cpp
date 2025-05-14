#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>

// Structure to represent a symbol in the symbol table
struct Symbol {
    std::string name;
    int address;
    std::string type;
};

// Structure to represent an instruction in the literal table
struct Literal {
    std::string name;
    int address;
    std::string value;
};

// Structure to represent an instruction in the intermediate code
struct Instruction {
    int locationCounter;
    std::string label;
    std::string opcode;
    std::string operand1;
    std::string operand2;
    std::string intermediateCode;
};

// Class for the two-pass assembler
class TwoPassAssembler {
private:
    std::vector<Symbol> symbolTable;
    std::vector<Literal> literalTable;
    std::vector<Instruction> intermediateCode;
    std::map<std::string, std::string> opcodeTable;
    std::map<std::string, std::string> registerTable;
    int locationCounter;
    int startAddress;

public:
    TwoPassAssembler() {
        initializeOpcodeTable();
        initializeRegisterTable();
        locationCounter = 0;
        startAddress = 0;
    }

    void initializeOpcodeTable() {
        opcodeTable["START"] = "(AD,01)";
        opcodeTable["END"] = "(AD,02)";
        opcodeTable["ORIGIN"] = "(AD,03)";
        opcodeTable["EQU"] = "(AD,04)";
        opcodeTable["LTORG"] = "(AD,05)";
        
        opcodeTable["READ"] = "(IS,09)";
        opcodeTable["PRINT"] = "(IS,10)";
        
        opcodeTable["MOVER"] = "(IS,04)";
        opcodeTable["MOVEM"] = "(IS,05)";
        opcodeTable["ADD"] = "(IS,01)";
        opcodeTable["SUB"] = "(IS,02)";
        opcodeTable["MULT"] = "(IS,03)";
        opcodeTable["DIV"] = "(IS,06)";
        opcodeTable["COMP"] = "(IS,07)";
        opcodeTable["BC"] = "(IS,08)";
        
        opcodeTable["DS"] = "(DL,01)";
        opcodeTable["DC"] = "(DL,02)";
        
        opcodeTable["STOP"] = "(IS,00)";
    }

    void initializeRegisterTable() {
        registerTable["AREG"] = "(1)";
        registerTable["BREG"] = "(2)";
        registerTable["CREG"] = "(3)";
        registerTable["DREG"] = "(4)";
    }

    // Add a symbol to the symbol table
    void addSymbol(const std::string& name, int address, const std::string& type = "Variable") {
        // Check if the symbol already exists
        for (auto& sym : symbolTable) {
            if (sym.name == name) {
                sym.address = address;
                return;
            }
        }
        
        // Add new symbol
        symbolTable.push_back({name, address, type});
    }

    // Get the symbol index from the symbol table
    int getSymbolIndex(const std::string& name) {
        for (size_t i = 0; i < symbolTable.size(); ++i) {
            if (symbolTable[i].name == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // First pass of the assembler
    void firstPass(const std::string& inputCode) {
        std::istringstream iss(inputCode);
        std::string line;
        
        while (std::getline(iss, line)) {
            processLine(line);
        }
        
        // Print the tables and intermediate code
        printSymbolTable();
        printIntermediateCode();
    }

    // Process a single line of assembly code
    void processLine(const std::string& line) {
        if (line.empty() || line.find_first_not_of(" \t") == std::string::npos) {
            return;  // Skip empty lines
        }
        
        std::istringstream iss(line);
        std::string label, opcode, operand1, operand2;
        
        // Extract label, opcode, and operands
        std::string token;
        int position = 0;
        while (iss >> token) {
            if (position == 0) {
                if (opcodeTable.find(token) == opcodeTable.end() && 
                    registerTable.find(token) == registerTable.end()) {
                    label = token;
                    position++;
                } else {
                    opcode = token;
                    position += 2;
                }
            } else if (position == 1) {
                opcode = token;
                position++;
            } else if (position == 2) {
                if (token.back() == ',') {
                    operand1 = token.substr(0, token.size() - 1);
                } else {
                    operand1 = token;
                    position++;
                }
            } else if (position == 3) {
                operand2 = token;
                position++;
            }
        }
        
        // Process the instruction
        processInstruction(label, opcode, operand1, operand2);
    }

    // Process an instruction and generate intermediate code
    void processInstruction(const std::string& label, const std::string& opcode, 
                           const std::string& operand1, const std::string& operand2) {
        if (opcode.empty()) {
            return;  // Skip if no opcode
        }
        
        Instruction instr;
        instr.locationCounter = locationCounter;
        instr.label = label;
        instr.opcode = opcode;
        instr.operand1 = operand1;
        instr.operand2 = operand2;
        
        // Process START directive
        if (opcode == "START") {
            startAddress = std::stoi(operand1);
            locationCounter = startAddress;
            instr.intermediateCode = "(AD,01) (C," + operand1 + ")";
        }
        // Process END directive
        else if (opcode == "END") {
            instr.intermediateCode = "(AD,02)";
        }
        // Process DS directive
        else if (opcode == "DS") {
            if (!label.empty()) {
                addSymbol(label, locationCounter);
            }
            int size = std::stoi(operand1);
            instr.intermediateCode = "(DL,01) (C," + operand1 + ")";
            locationCounter += size;
        }
        // Process DC directive
        else if (opcode == "DC") {
            if (!label.empty()) {
                addSymbol(label, locationCounter);
            }
            instr.intermediateCode = "(DL,02) (C," + operand1 + ")";
            locationCounter += 1;
        }
        // Process READ instruction
        else if (opcode == "READ") {
            if (getSymbolIndex(operand1) == -1) {
                addSymbol(operand1, 0);  // Address will be updated later
            }
            instr.intermediateCode = "(IS,09) (S," + std::to_string(getSymbolIndex(operand1)) + ")";
            locationCounter += 1;
        }
        // Process PRINT instruction
        else if (opcode == "PRINT") {
            if (getSymbolIndex(operand1) == -1) {
                addSymbol(operand1, 0);  // Address will be updated later
            }
            instr.intermediateCode = "(IS,10) (S," + std::to_string(getSymbolIndex(operand1)) + ")";
            locationCounter += 1;
        }
        // Process STOP instruction
        else if (opcode == "STOP") {
            instr.intermediateCode = "(IS,00)";
            locationCounter += 1;
        }
        // Process other instructions (MOVER, SUB, etc.)
        else if (opcodeTable.find(opcode) != opcodeTable.end()) {
            std::string opcodeEntry = opcodeTable[opcode];
            std::string regEntry = "";
            std::string operandEntry = "";
            
            if (!operand1.empty()) {
                if (registerTable.find(operand1) != registerTable.end()) {
                    regEntry = registerTable[operand1];
                }
            }
            
            if (!operand2.empty()) {
                if (getSymbolIndex(operand2) == -1) {
                    addSymbol(operand2, 0);  // Address will be updated later
                }
                operandEntry = "(S," + std::to_string(getSymbolIndex(operand2)) + ")";
            }
            
            instr.intermediateCode = opcodeEntry + " " + regEntry + " " + operandEntry;
            locationCounter += 1;
        }
        
        // Add label to symbol table if present
        if (!label.empty() && opcode != "DS" && opcode != "DC") {
            addSymbol(label, locationCounter, "Label");
        }
        
        // Add instruction to intermediate code
        intermediateCode.push_back(instr);
    }

    // Print the symbol table
    void printSymbolTable() {
        std::cout << "\n--- SYMBOL TABLE ---\n";
        std::cout << std::left << std::setw(10) << "Symbol" << std::setw(10) << "Address" << std::setw(10) << "Type" << std::endl;
        std::cout << "------------------------\n";
        
        for (const auto& sym : symbolTable) {
            std::cout << std::left << std::setw(10) << sym.name << std::setw(10) << sym.address << std::setw(10) << sym.type << std::endl;
        }
    }

    // Print the intermediate code
    void printIntermediateCode() {
        std::cout << "\n--- INTERMEDIATE CODE ---\n";
        std::cout << std::left << std::setw(10) << "LC" << std::setw(10) << "Label" << std::setw(10) 
                 << "Opcode" << std::setw(10) << "Operand1" << std::setw(10) << "Operand2" 
                 << "Intermediate Code" << std::endl;
        std::cout << "--------------------------------------------------------------\n";
        
        for (const auto& instr : intermediateCode) {
            std::cout << std::left << std::setw(10) << instr.locationCounter << std::setw(10) << instr.label 
                     << std::setw(10) << instr.opcode << std::setw(10) << instr.operand1 << std::setw(10) 
                     << instr.operand2 << instr.intermediateCode << std::endl;
        }
    }
};

int main() {
    // Example assembly code from your requirements
    std::string assemblyCode1 = R"(
START 100
READ A
READ B
MOVER AREG, A
SUB AREG, B
STOP
A DS 1
B DS 1
END
)";

    std::string assemblyCode2 = R"(
START 200
READ M
READ N
MOVER AREG, M
SUB AREG, N
STOP
M DS 1
N DS 1
END
)";

    std::string assemblyCode3 = R"(
START 300
READ M
READ N
MOVER AREG, M
SUB AREG, N
STOP
M DS 1
N DS 1
END
)";

    // Process the first example
    std::cout << "\n======== PROCESSING ASSEMBLY CODE 1 (START 100) ========\n";
    TwoPassAssembler assembler1;
    assembler1.firstPass(assemblyCode1);
    
    // // Process the second example
    // std::cout << "\n======== PROCESSING ASSEMBLY CODE 2 (START 200) ========\n";
    // TwoPassAssembler assembler2;
    // assembler2.firstPass(assemblyCode2);
    
    // // Process the third example
    // std::cout << "\n======== PROCESSING ASSEMBLY CODE 3 (START 300) ========\n";
    // TwoPassAssembler assembler3;
    // assembler3.firstPass(assemblyCode3);

    return 0;
}

/* 
✅ Step 1: Compile the C++ file

g++ your_file.cpp -o output_name
g++ main.cpp -o main

✅ Step 2: Run the compiled program

./output_name
./main

❗ If you don’t have g++ installed:
Install it with:

sudo apt update
sudo apt install g++
Let me know your file name if you want the exact commands. 



✅ Default Executable from C/C++ Compilation
When you compile a C or C++ program using gcc or g++ without specifying an output file name:
g++ file.cpp
It creates an executable file named a.out by default.

You can run it using:


./a.out

🔁 Used for Any Binary Executable
./a.out can refer to any executable file named a.out in the current directory. It’s not special in itself — it’s just a filename.

🧼 Better Practice: Name Your Executable
Instead of using a.out, name your output clearly:

g++ file.cpp -o myprogram
./myprogram
*/