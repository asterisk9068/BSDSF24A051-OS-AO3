# PA-03: UNIX Shell Implementation (myshell)

## Course Information
- **Course**: Operating Systems
- **Student Name**: Muhammad Ahmad
- **Roll Number**: BSDSF24A051

## Overview
This project is an incremental implementation of a UNIX command-line interpreter (shell) called `myshell`, developed as part of Operating Systems Programming Assignment 03 (PA-03).

### Features Implemented
- **Release 1 (`v1.0-base`)**:
  - Command Reading (`read_cmd`): Prompts with `myshell> ` and reads input using `getline()`.
  - Tokenization (`tokenize`): Parses command lines into whitespace-delimited tokens.
  - Execution (`execute`): Executes external commands using `fork()`, `execvp()`, and `waitpid()`.
- **Release 2 (`v2` / Feature 2 - Built-in Commands)**:
  - `cd [dir]`: Changes current directory using `chdir()` (supports home directory when omitted or `~`, and error reporting).
  - `pwd`: Displays current working directory using `getcwd()`.
  - `exit [code]`: Terminates the shell gracefully.
  - `help`: Lists supported built-in commands.
  - `jobs`: Placeholder command for background job control.

## Directory Structure
```
BSDSF24A051-OS-AO3/
├── Makefile
├── README.md
├── include/
│   └── shell.h
└── src/
    ├── builtin.c
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
