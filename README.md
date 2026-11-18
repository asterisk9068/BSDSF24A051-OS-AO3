# PA-03: UNIX Shell Implementation (myshell)

## Course Information
- **Course**: Operating Systems
- **Student Name**: Muhammad Ahmad
- **Roll Number**: BSDSF24A051

## Overview
This project is an incremental implementation of a UNIX command-line interpreter (shell) called `myshell`, developed as part of Operating Systems Programming Assignment 03 (PA-03).

### Version: `v1.0-base` (Release 1 - The Base Shell)
The base shell implements the foundational UNIX command execution cycle using POSIX system calls:
1. **Command Reading (`read_cmd`)**: Prompts the user with `myshell> ` and reads input from standard input using `getline()`.
2. **Tokenization (`tokenize`)**: Splits the command line into individual arguments delimited by whitespace.
3. **Execution (`execute`)**:
   - Creates a child process using `fork()`.
   - In the child process, invokes `execvp()` to execute external binaries in the system's `PATH`.
   - In the parent process, waits for the child process to complete using `waitpid()`.

## Directory Structure
```
BSDSF24A051-OS-AO3/
├── Makefile
├── README.md
├── include/
│   └── shell.h
└── src/
    ├── execute.c
    ├── main.c
    └── shell.c
```

## Compilation and Usage

### Building the Project
Compile the shell executable by running:
```bash
make
```
This compiles the source files into `obj/` and outputs the executable binary at `bin/myshell`.

### Running the Shell
Launch the shell:
```bash
./bin/myshell
```

Test commands such as:
```bash
myshell> ls -l
myshell> pwd
myshell> whoami
```

To exit the shell, send an EOF signal (`Ctrl+D`).

### Cleaning Build Artifacts
To remove compiled object files and binaries:
```bash
make clean
```
