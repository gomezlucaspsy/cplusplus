#include <stdio.h>
#include <math.h>

typedef struct {
    double result;
} Calculator;

Calculator* CalcCreate() {
    Calculator *calc = (Calculator*)malloc(sizeof(Calculator));
    calc->result = 0.0;
    return calc;
}

Void CalcAdd(Calculator *calc, F64 num) {
    calc->result += num;
}

Void CalcSubtract(Calculator *calc, F64 num) {
    calc->result -= num;
}

Void CalcMultiply(Calculator *calc, F64 num) {
    calc->result *= num;
}

Void CalcDivide(Calculator *calc, F64 num) {
    if (num == 0.0) {
        Print("Error: Division by zero!\n");
    } else {
        calc->result /= num;
    }
}

Void CalcPower(Calculator *calc, F64 num) {
    calc->result = pow(calc->result, num);
}

Void CalcReset(Calculator *calc) {
    calc->result = 0.0;
}

Void CalcDisplay(Calculator *calc) {
    Print("Result: %.2f\n", calc->result);
}

Void DisplayMenu() {
    Print("\n=== Holy C Calculator ===\n");
    Print("1. Add\n");
    Print("2. Subtract\n");
    Print("3. Multiply\n");
    Print("4. Divide\n");
    Print("5. Power\n");
    Print("6. Reset\n");
    Print("7. View Result\n");
    Print("8. Exit\n");
    Print("=========================\n");
}

Void Main() {
    Calculator *calc = CalcCreate();
    I64 choice;
    F64 num;
    
    Print("Welcome to the Holy C Calculator!\n");
    
    while (1) {
        DisplayMenu();
        Print("Enter your choice (1-8): ");
        choice = ScanI64();
        
        switch (choice) {
            case 1:
                Print("Enter number to add: ");
                num = ScanF64();
                CalcAdd(calc, num);
                CalcDisplay(calc);
                break;
                
            case 2:
                Print("Enter number to subtract: ");
                num = ScanF64();
                CalcSubtract(calc, num);
                CalcDisplay(calc);
                break;
                
            case 3:
                Print("Enter number to multiply: ");
                num = ScanF64();
                CalcMultiply(calc, num);
                CalcDisplay(calc);
                break;
                
            case 4:
                Print("Enter number to divide: ");
                num = ScanF64();
                CalcDivide(calc, num);
                CalcDisplay(calc);
                break;
                
            case 5:
                Print("Enter exponent: ");
                num = ScanF64();
                CalcPower(calc, num);
                CalcDisplay(calc);
                break;
                
            case 6:
                CalcReset(calc);
                Print("Calculator reset to 0\n");
                break;
                
            case 7:
                CalcDisplay(calc);
                break;
                
            case 8:
                Print("Thank you for using the Holy C Calculator. Goodbye!\n");
                free(calc);
                return;
                
            default:
                Print("Invalid choice! Please enter a number between 1 and 8.\n");
        }
    }
}
