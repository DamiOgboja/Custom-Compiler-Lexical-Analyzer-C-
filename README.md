# Custom Compiler & Lexical Analyzer (C)

## 📌 Overview
A multi-stage compiler built from scratch in C that translates high-level programming syntax into functional virtual machine assembly instructions. This project demonstrates deep computer science fundamentals, including low-level memory management, custom tokenization, and recursive syntax evaluation.

## 🚀 Features
* **Lexical Analysis:** A custom tokenization engine that reads raw source code, groups characters into actionable tokens, ignores whitespace/comments, and strictly flags syntax and length errors.
* **Recursive Descent Parsing:** Validates grammar rules, variable declarations, conditional loops (`while`, `if/then`), and complex mathematical expressions using a recursive descent parser.
* **Dynamic Symbol Table:** Manages memory addresses and symbol tracking for variables, constants, and procedures.
* **Code Generation:** Translates high-level parsed statements into actionable low-level virtual machine assembly instructions (e.g., `JMP`, `STO`, `LOD`, `JPC`).

## 🛠️ Tech Stack
* **Language:** C
* **Concepts:** Compiler Design, Lexical Analysis, Parsing Algorithms, Virtual Machines, Low-Level Memory Management

## ⚙️ How to Compile & Run

### Prerequisites
Ensure you have a C compiler (like GCC) installed on your system. 

### 1. Download the Repository
Clone this repository and ensure `parsercodegen.c` and your test `.txt` input files are in the same directory.

### 2. Compile the Code
Open your terminal, navigate to the project directory, and compile the program using GCC:
```bash
gcc parsercodegen.c
```
(This will generate an executable file, typically named a.out on Linux/macOS or a.exe on Windows).

### 3. Run the Compiler
Execute the compiled program while passing in your source code text file as an argument:
```bash
./a.out input.txt
```
## 📄 Output
Upon a successful run, the compiler will provide two forms of output:

### 1. Terminal Output: 
Displays the generated Assembly Code (OP, L, M) and a detailed printout of the Symbol Table (Kind, Name, Value, Level, Address) directly in your terminal.

### 2. Executable File: 
Generates an elf.txt file in the same directory containing the raw virtual machine instructions generated from your input code.
