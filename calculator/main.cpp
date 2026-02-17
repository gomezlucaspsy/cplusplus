#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

class Calculator {
private:
    double result;
    
public:
    Calculator() : result(0) {}
    
    void add(double num) {
        result += num;
    }
    
    void subtract(double num) {
        result -= num;
    }
    
    void multiply(double num) {
        result *= num;
    }
    
    void divide(double num) {
        if (num == 0) {
            cout << "Error: Division by zero!" << endl;
        } else {
            result /= num;
        }
    }
    
    void power(double num) {
        result = pow(result, num);
    }
    
    void setResult(double num) {
        result = num;
    }
    
    double getResult() const {
        return result;
    }
    
    void reset() {
        result = 0;
    }
    
    void displayResult() {
        cout << "Result: " << fixed << setprecision(2) << result << endl;
    }
};

void displayMenu() {
    cout << "\n=== Simple Calculator ===" << endl;
    cout << "1. Add" << endl;
    cout << "2. Subtract" << endl;
    cout << "3. Multiply" << endl;
    cout << "4. Divide" << endl;
    cout << "5. Power" << endl;
    cout << "6. Reset" << endl;
    cout << "7. View Result" << endl;
    cout << "8. Exit" << endl;
    cout << "========================" << endl;
}

int main() {
    Calculator calc;
    int choice;
    double num;
    
    cout << "Welcome to the C++ Calculator!" << endl;
    
    while (true) {
        displayMenu();
        cout << "Enter your choice (1-8): ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Please enter a number between 1 and 8." << endl;
            continue;
        }
        
        switch (choice) {
            case 1:
                cout << "Enter number to add: ";
                cin >> num;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input!" << endl;
                } else {
                    calc.add(num);
                    calc.displayResult();
                }
                break;
                
            case 2:
                cout << "Enter number to subtract: ";
                cin >> num;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input!" << endl;
                } else {
                    calc.subtract(num);
                    calc.displayResult();
                }
                break;
                
            case 3:
                cout << "Enter number to multiply: ";
                cin >> num;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input!" << endl;
                } else {
                    calc.multiply(num);
                    calc.displayResult();
                }
                break;
                
            case 4:
                cout << "Enter number to divide: ";
                cin >> num;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input!" << endl;
                } else {
                    calc.divide(num);
                    calc.displayResult();
                }
                break;
                
            case 5:
                cout << "Enter exponent: ";
                cin >> num;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input!" << endl;
                } else {
                    calc.power(num);
                    calc.displayResult();
                }
                break;
                
            case 6:
                calc.reset();
                cout << "Calculator reset to 0" << endl;
                break;
                
            case 7:
                calc.displayResult();
                break;
                
            case 8:
                cout << "Thank you for using the calculator. Goodbye!" << endl;
                return 0;
                
            default:
                cout << "Invalid choice! Please enter a number between 1 and 8." << endl;
        }
    }
    
    return 0;
}
