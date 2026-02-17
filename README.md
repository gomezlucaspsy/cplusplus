# C++ Projects

Here is where I build my projects in C++

## 1. Calculator

A simple, interactive command-line calculator built with C++.

### Features

- **Basic Operations**: Add, Subtract, Multiply, Divide
- **Advanced Functions**: Power calculation
- **Error Handling**: Prevents division by zero
- **Persistent Result**: Results carry over between operations
- **Reset Function**: Clear the calculator to start fresh
- **User-Friendly Menu**: Interactive menu-driven interface

### How to Compile

You need a C++ compiler to build this project. Here are your options:

#### Option 1: MinGW (Recommended for Windows)
1. Install MinGW from https://www.mingw-w64.org/
2. Open Command Prompt and navigate to the project directory
3. Run: `g++ -o calculator.exe main.cpp`

#### Option 2: Visual Studio
1. Install Visual Studio Community (free version available)
2. Open Developer Command Prompt for Visual Studio
3. Navigate to the project directory and run: `cl /o calculator.exe main.cpp`

#### Option 3: Online Compiler
1. Copy the code from main.cpp
2. Paste it into an online C++ compiler at https://www.onlinegdb.com/

### Running the Calculator

After compilation, run the executable:
```
calculator.exe
```

### Program Features

The calculator uses a `Calculator` class with the following methods:
- `add()`: Adds a number to the result
- `subtract()`: Subtracts a number from the result
- `multiply()`: Multiplies the result by a number
- `divide()`: Divides the result by a number (with zero-check)
- `power()`: Raises result to a power
- `reset()`: Resets the result to 0
- `displayResult()`: Displays the result with 2 decimal places

### Error Handling

- Division by zero: Displays error message and doesn't modify result
- Invalid input: Clears input buffer and prompts user to try again

## 2. Holy C Calculator

A calculator implementation in Holy C language (the language used in TempleOS).

### Files

- **calculator.c** - Holy C version of the calculator

### Features

Same as the C++ calculator:
- Basic arithmetic operations (Add, Subtract, Multiply, Divide, Power)
- Error handling for division by zero
- Reset and view result functions
- Interactive menu interface

### How to Compile in TempleOS

1. Open TempleOS
2. Copy the contents of `calculator.c` into a new HolyC file
3. Compile using the TempleOS compiler

### Holy C Notes

This implementation uses Holy C specific features:
- `Void` for void return type
- `I64` for 64-bit integers
- `F64` for 64-bit floating point
- `Print()` for output
- `ScanI64()` and `ScanF64()` for input
- `malloc()` and `free()` for memory management
