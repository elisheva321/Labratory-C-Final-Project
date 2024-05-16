# Labratory C final Project By Elisheva Cirota and Daniel Oren
This is the Final project (mmn 14) in Laboratory C 20465- 2024A course at the Open University. This project was developed as part of the "20465 - Systems Programming Laboratory" 2024A course at the Open University. It's a comprehensive assembler program for a specific assembly language, written in C. The purpose is to simulate the operation of common system programs, and thus experience writing a large-scale software project.

## 🧩 Project Structure
The project consists of multiple C and header files, each serving a specific purpose in the program. It adheres to principles of clarity, readability, structured writing, and data abstraction.

The assembler works in three main stages:
1. **Pre-Processing**
2. **First Pass**
3. **Second Pass**
   
These stages form the core algorithm of the assembler.

### Pre-Processing
The pre-assembler reads the source file line by line and identifies macro definitions. If a macro is identified, the lines of the macro are added to a macro table and replaced in the source file.

### First Pass
The first pass identifies symbols (labels) and assigns them numerical values representing their corresponding memory addresses.

### Second Pass
In the second pass, the assembler generates the final machine code, replacing operation names with their binary equivalents and symbol names with their assigned memory locations.

Details about the algorithms for these stages can be found in the documentation.
