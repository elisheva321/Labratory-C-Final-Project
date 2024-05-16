# Labratory C final Project By Elisheva Cirota and Daniel Oren
This is the Final project (mmn 14) in Laboratory C 20465- 2024A course at the Open University. This project was developed as part of the "20465 - Systems Programming Laboratory" 2024A course at the Open University. It's a comprehensive assembler program for a specific assembly language, written in C. The purpose is to simulate the operation of common system programs, and thus experience writing a large-scale software project.

# 🧩 Project Structure
The project consists of multiple C and header files, each serving a specific purpose in the program. It adheres to principles of clarity, readability, structured writing, and data abstraction.

The assembler works in three main stages:
  1. Pre-Processing
  2. First Pass
  3. Second Pass
These stages form the core algorithm of the assembler.

# Pre-Processing
The pre-assembler reads the source file line by line and identifies macro definitions. If a macro is identified, the lines of the macro are added to a macro table and replaced in the source file.

# First Pass
The first pass identifies symbols (labels) and assigns them numerical values representing their corresponding memory addresses.

# Second Pass
In the second pass, the assembler generates the final machine code, replacing operation names with their binary equivalents and symbol names with their assigned memory locations.

Details about the algorithms for these stages can be found in the documentation.


___
⚙️ Getting Started
✔️ Prerequisites
Please switch to the linux branch to run the project. Use the following instructions:

Before you begin, ensure that you have the following prerequisites installed:

ℹ️ GCC compiler
ℹ️ Ubuntu system (but others will also work)
📦 Installation
Clone the Labratory-C-Final-Project repository:
git clone https://github.com/AlmogShKt/Labratory-C-Final-Project.git
Change to the project directory:
cd Labratory-C-Final-Project
Checkout the Linux branch:!
git checkout linux
Run Makefile to compile the program:
`make`
🔧 Usage
Run the compiled program using the following command: ./assembler file_name_1 ... file_name_N

This will output machine code generated from the provided assembly file.

This is valid input and output files:

 

And for Invalid output:



🧪 Testing
Several assembly language input files are provided to demonstrate the use of various operations and data types, as well as the assembler's handling of errors.

📚 Documentation
Each function in the source code is accompanied by concise, clear documentation in the form of header comments, explaining its purpose and usage. Important variables are also explained. Detailed comments are present throughout the code for better understanding.

notes:
- all internal functions (functions that are not exposed to the user of the library) will start with prefix "_".
- we used in the project LOGGER. all logs will be written into "logfile.txt"
- line count refers to the am file and not the original file.
- in case of an error in pre processor stage, we will not continue to first/second pass.
- in case of an error in first_pass we do continue to second pass to find more errors if possible.

