#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <cctype>
#include <sstream>

using namespace std;

int tempCount = 1;

string newTemp() {
    return "t" + to_string(tempCount++);
}

int precedence(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

vector<string> generateTAC(const string& expr) {
    stack<string> operands;
    stack<char> operators;
    vector<string> tac;

    string var = "", lhs = "";
    int i = 0, n = expr.size();

    // Extract LHS (assignment)
    while (i < n && expr[i] != '=') {
        if (!isspace(expr[i])) lhs += expr[i];
        i++;
    }
    i++; // skip '='

    while (i < n) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }
        if (isalpha(expr[i]) || isdigit(expr[i])) {
            string token = "";
            while (i < n && (isalnum(expr[i]) || expr[i] == '_')) {
                token += expr[i++];
            }
            operands.push(token);
        } else if (expr[i] == '(') {
            operators.push(expr[i++]);
        } else if (expr[i] == ')') {
            while (!operators.empty() && operators.top() != '(') {
                string op2 = operands.top(); operands.pop();
                string op1 = operands.top(); operands.pop();
                char op = operators.top(); operators.pop();
                string temp = newTemp();
                tac.push_back(temp + " = " + op1 + " " + op + " " + op2);
                operands.push(temp);
            }
            operators.pop(); // remove '('
            i++;
        } else if (isOperator(expr[i])) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(expr[i])) {
                char op = operators.top(); operators.pop();
                string op2 = operands.top(); operands.pop();
                string op1 = operands.top(); operands.pop();
                string temp = newTemp();
                tac.push_back(temp + " = " + op1 + " " + op + " " + op2);
                operands.push(temp);
            }
            operators.push(expr[i++]);
        } else {
            i++;
        }
    }

    while (!operators.empty()) {
        char op = operators.top(); operators.pop();
        string op2 = operands.top(); operands.pop();
        string op1 = operands.top(); operands.pop();
        string temp = newTemp();
        tac.push_back(temp + " = " + op1 + " " + op + " " + op2);
        operands.push(temp);
    }

    tac.push_back(lhs + " = " + operands.top());
    return tac;
}

int main() {
    string input;
    cout << "Enter an expression (e.g., a = (b*b + c*c) * (p - q - r)):\n";
    getline(cin, input);

    vector<string> tac = generateTAC(input);
    cout << "\nThree Address Code:\n";
    for (const string& line : tac) {
        cout << line << endl;
    }

    return 0;
}
