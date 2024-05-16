# Labratory C final Project By Elisheva Cirota and Daniel Oren
This is the Final project (mmn 14) in Laboratory C 20465- 2024A course at the Open University. This project was developed as part of the "20465 - Systems Programming Laboratory" 2024A course at the Open University. It's a comprehensive assembler program for a specific assembly language, written in C. The purpose is to simulate the operation of common system programs, and thus experience writing a large-scale software project.

## 📖 Table of Contents
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [Usage](#-usage)
- [Testing](#-testing)
- [Documentation](#-documentation)
- [Contribution](#-contribution)
  
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

<details closed><summary>📂 Project Structure</summary>

```
.
├── assembler
├── expected_output
│   ├── error_1
│   │   ├── error1.am
│   │   └── error1OUT.txt
│   ├── ps1
│   │   ├── ps1.ent
│   │   ├── ps1.ext
│   │   └── ps1.ob
│   ├── test1
│   │   ├── test1.am
│   │   ├── test1.ent
│   │   ├── test1.ext
│   │   └── test1.ob
│   ├── test2
│   │   ├── test2.am
│   │   ├── test2.as
│   │   └── test2OUT.txt
│   ├── test3
│   │   ├── test3.am
│   │   ├── test3.ent
│   │   ├── test3.ext
│   │   └── test3.ob
│   ├── test4
│   │   ├── test4.am
│   │   ├── test4.as
│   │   └── test4OUT.txt
│   ├── test5
│   │   ├── test5.am
│   │   ├── test5.as
│   │   ├── test5.ent
│   │   ├── test5.ext
│   │   └── test5.ob
│   └── test6
│       ├── test6.am
│       ├── test6.as
│       └── test6.ob
├── invalid_input
│   ├── error1
│   │   ├── error1.am
│   │   ├── error1.as
│   │   └── error1.out
│   ├── test2
│   │   ├── test2.am
│   │   ├── test2.as
│   │   └── test2.out
│   ├── test4
│   │   ├── test4.am
│   │   ├── test4.as
│   │   └── test4.out
│   └── test7
│       ├── test7.as
│       └── test7.out
├── logfile.txt
├── makefile
├── README.md
├── src
│   ├── assembler
│   │   ├── assembler_consts.h
│   │   ├── assembler_first_pass.c
│   │   ├── assembler.h
│   │   ├── assembler_second_pass.c
│   │   └── test_assembler.c
│   ├── assembler_helper
│   │   ├── assembler_helper.h
│   │   ├── const_define_handler.c
│   │   ├── directive_handler.c
│   │   ├── instruction_handler.c
│   │   ├── label_handler.c
│   │   ├── line_handler.c
│   │   └── symbol_handler.c
│   ├── assembler_main
│   │   ├── assembler_main.c
│   │   ├── assembler_main.h
│   │   └── test_assembler_main.c
│   ├── dynamic_queue
│   │   ├── dynamic_queue.c
│   │   └── dynamic_queue.h
│   ├── ext_ent_file_builder
│   │   ├── ext_ent_file_builder.c
│   │   └── ext_ent_file_builder.h
│   ├── general_const
│   │   ├── general_const.c
│   │   └── general_const.h
│   ├── general_todo.txt
│   ├── hash_table
│   │   ├── hash_table.c
│   │   └── hash_table.h
│   ├── logger
│   │   ├── logger.c
│   │   └── logger.h
│   ├── obj_file_builder
│   │   ├── obj_file_builder.c
│   │   └── obj_file_builder.h
│   ├── open_questions.txt
│   ├── pre_processor
│   │   ├── preprocessor.c
│   │   ├── preprocessor_consts.h
│   │   ├── preprocessor.h
│   │   └── test_preprocessor.c
│   ├── string_vector
│   │   ├── string_vector.c
│   │   └── string_vector.h
│   └── utils
│       ├── utils.c
│       └── utils.h
├── tree_structure.txt
└── valid_input
    ├── ps1
    │   ├── ps1.am
    │   ├── ps1.as
    │   ├── ps1.ent
    │   ├── ps1.ext
    │   └── ps1.ob
    ├── test1
    │   ├── test1
    │   ├── test1.am
    │   ├── test1.as
    │   ├── test1.ent
    │   ├── test1.ext
    │   └── test1.ob
    ├── test3
    │   ├── test3.am
    │   ├── test3.as
    │   ├── test3.ent
    │   ├── test3.ext
    │   └── test3.ob
    ├── test5
    │   ├── test5.am
    │   ├── test5.as
    │   ├── test5.ent
    │   ├── test5.ext
    │   └── test5.ob
    └── test6
        ├── test6.am
        ├── test6.as
        └── test6.ob

33 directories, 104 files
```

</details>

## ⚙️ Getting Started

### ✔️ Prerequisites

Before you begin, ensure that you have the following prerequisites installed:
> - `ℹ️ GCC compiler`
> - `ℹ️ Ubuntu system (but others will also work)`

### 📦 Installation

1. Clone the Labratory-C-Final-Project repository:
```sh
git clone https://github.com/elisheva321/Labratory-C-Final-Project.git
```

2. Change to the project directory:
```sh
cd Labratory-C-Final-Project
```

5. Run Makefile to compile the program:
```sh
`make`
```


## 🔧 Usage

Run the compiled program using the following command: `./assembler file_name_1 ... file_name_N`

This will output machine code generated from the provided assembly file.

This is valid input and output files:

<p>
  <img src="valid_input/ps1/valid input screenshot.png" width="200"> <img src="valid_input/ps1/valid output screenshot.png" width="200">  <img src="valid_input/ps1/valid output ext screenshot.png" width="200">  <img src="valid_input/ps1/valid output ent screenshot.png" width="200">
</p>

And for Invalid output:

<p align="center">
  <img src="Readme_imgs/Invalid1.png" width="600">
</p>


___




___ 

## 🧪 Testing

Several assembly language input files are provided to demonstrate the use of various operations and data types, as well as the assembler's handling of errors.

___ 

## 📚 Documentation

Each function in the source code is accompanied by concise, clear documentation in the form of header comments, explaining its purpose and usage. Important variables are also explained. Detailed comments are present throughout the code for better understanding.

___ 

