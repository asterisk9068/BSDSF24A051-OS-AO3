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
- **Release 3 (`v3` / Feature 3 - Command History)**:
  - Command History Storage: In-memory history tracking for at least 20 commands (capacity 100).
  - `history`: Built-in command listing previously executed commands with 1-based indexing.
  - `!n` and `!!`: Re-executes the n-th command from history or the last command before tokenization.
- **Release 4 (`v4` / Feature 4 - Tab Completion with Readline)**:
  - GNU Readline Integration: Replaces standard line reading with `readline()` for command line editing and history navigation.
  - Tab Completion: Auto-completion for built-in command names and current directory files via `rl_attempted_completion_function`.
  - Arrow Key History Navigation: Interactively cycles through previous commands via `add_history()`.
- **Release 5 (`v5` / Feature 5 - I/O Redirection and Pipes)**:
  - Output Redirection (`>` and `>>`): Redirects stdout to files with truncate or append mode using `open()`, `dup2()`, and `close()`.
  - Input Redirection (`<`): Redirects stdin from files with error handling.
  - Inter-Process Pipes (`|`): Connects stdout of one process to stdin of another across single or multi-stage pipelines using `pipe()` and `dup2()`.
  - Built-in Redirection: Built-ins (`pwd`, `history`, `help`, etc.) seamlessly support input and output redirection.
- **Release 6 (`v6` / Feature 6 - Command Chaining and Background Execution)**:
  - Command Chaining (`;`): Executes multiple commands sequentially on a single line.
  - Background Execution (`&`): Asynchronously launches commands in the background without blocking the parent shell.
  - Non-blocking Zombie Reaper: Periodically reaps completed children via `waitpid(-1, &status, WNOHANG)`.
  - `jobs`: Built-in command listing currently running background processes and their PIDs.
- **Release 7 (`v7` / Feature 7 - The if-then-else-fi Control Structure)**:
  - Multi-line Control Flow: Supports parsing and executing multi-line conditional blocks (`if`, `then`, `else`, `fi`).
  - Exit Status Inspection: Evaluates child termination codes using `WEXITSTATUS(status)` to branch execution.
  - Optional `else` block: Supports both full `if-then-else-fi` and `if-then-fi` conditionals.
- **Release 8 (`v8` / Feature 8 - Shell Variables)**:
  - Variable Assignment: Supports `VAR=value` syntax (without spaces) as a built-in action.
  - Variable Expansion: Expands `$VAR` and `${VAR}` before command execution; falls back to environment variables.
  - `set` Built-in: Displays all currently defined shell variables and their values.
  - Linked List Storage: Variables stored in a singly-linked list of key-value pairs with memory-safe lifecycle management.

## Directory Structure
```
BSDSF24A051-OS-AO3/
├── Makefile
├── README.md
├── include/
│   ├── control.h
│   ├── history.h
│   ├── jobs.h
│   ├── shell.h
│   └── variables.h
└── src/
    ├── builtin.c
    ├── control.c
    ├── execute.c
    ├── history.c
    ├── jobs.c
    ├── main.c
    ├── shell.c
    └── variables.c
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
