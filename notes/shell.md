# SHELL
Information about the shell and its functionality can be found in the [shell_theory.md](shell_theory.md) file.

# Builtin commands
- Builtin commands are commands that are built into the shell itself, rather than being separate executable programs. They are executed directly by the shell and do not require a separate process to be created. Builtin commands are typically used for tasks that are closely related to the shell's operation, such as changing directories, managing environment variables, and controlling the shell's behavior.

- Examples of builtin commands include:
  - cd: Change the current working directory.
  - echo: Display a line of text or a variable's value.
  - export: Set environment variables for the current shell session.
  - history: Display the command history.
  - pwd: Print the current working directory.
  - exit: Exit the shell.
  - type: Display information about a command, including whether it is a builtin command or an external executable.


# Path
- The PATH environment variable is a list of directories that the shell searches for executable files when a command is entered.
-  When you type a command, the shell looks through each directory in the PATH variable in order until it finds an executable file with the same name as the command. If it finds one, it runs that file; if not, it returns an error indicating that the command was not found.
  
PATH="/usr/bin:/usr/local/bin:$PATH"
/usr/bin → search here first.
/usr/local/bin → search here second.
$PATH → append whatever directories were already in your PATH.


$
# Program Execution
- When you enter a command in the shell, it creates a new process to execute that command. The shell uses the fork() system call to create a new process, and then the exec() system call to replace the new process's memory space with the program being executed.

- the operating system:
loads the executable into RAM,
allocates memory,
creates a stack,
creates a heap,
initializes CPU registers,
starts executing main().

This running instance is called a process.

- Each process has its own:
- 1. Process ID (PID): A unique identifier for the process.
- 2. Parent Process ID (PPID): The PID of the process that created it.
- 3. memory
- 4. stack: A region of memory used for function calls and local variables.
- 5. heap: A region of memory used for dynamic memory allocation.
- 6. variables: Each process has its own set of environment variables, which can be accessed and modified by the process.
- 7. program counter: A register that keeps track of the current instruction being executed by the process.

- The OS is responsible for creating and managing processes, keeping a table with all the processes and their information, which is used for process management and scheduling.

## Fork and Exec
- *FORK*: The fork() system call creates a new process by duplicating the calling process. The new process is called the child process, and the calling process is called the parent process. The child process gets a unique PID, and it inherits a copy of the parent's memory space, including variables, stack, and heap.

Before calling fork():

Process
   |
 main

After fork():

          Original
              │
          fork()
         /      \
        /        \
 Original      Copy
 (parent)     (child)

 - Parent is the original process, and child is the new process created by fork(). both are almost identical, but they have different PIDs and can execute independently.
  
- fork() return 2 values:
  - In the parent process, fork() returns the PID of the child process (>0)
  - In the child process, fork() returns 0.
- In cpp, fork() is declared in the <unistd.h> header file.

- **WHY FORKING IS NECESSARY**: Forking is necessary because it allows the shell to create a new process to execute a command without terminating itself. This way, the shell can continue to run and accept new commands while the child process executes the command in parallel.


- *exec*: The exec() family of functions replaces the current process's memory space with a new program. 
- When a process calls exec(), it loads the specified executable into its memory space, replacing the current program. 
- The new program starts executing from its main() function, and the original program is no longer running.

- *wait*: The wait() system call is used by a parent process to wait for its child processes to finish executing. When a parent process calls wait(), it blocks until one of its child processes terminates. The parent can then retrieve the exit status of the terminated child process.

Shell running
      │
      │ fork()
      ▼
 ┌──────────────┐
 │              │
Parent       Child
 │              │
 │          exec(ls)
 │              │
 │          ls runs
 │              │
wait() <────────┘
 │
 prompt appears again

This fork() → exec() → wait() pattern is the foundation of most Unix command execution.


### Example of fork() and exec() in C++:
```cpp
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

pid_t pid = fork();
if  (pid < 0) {
    std::cerr << "fork() failed\n";     // fork failed

} else if (pid == 0) {
    // child process
    vector<char*> args = {"ls", "-l", nullptr};
    execv("/bin/ls", args);
    perror("execv"); // execv() failed
    exit(1); // exit child process with error code

} else {
    // parent process
    waitpid(pid, nullptr, 0);


};


```

- execl() is one of the exec() family of functions. it takes FULL PATH and a list of arguments, and replaces the current process with the new program.
- execpl() dont take full path, it searches for the executable in the directories listed in the PATH environment variable.
- 
- execv() takes a vector of arguments instead of a list of arguments. Much better for C++ programs, as it allows for dynamic argument lists.
- execvp() is similar to execv(), but it searches for the executable in the directories listed in the PATH environment variable.


- perror() is a function that prints a descriptive error message to stderr based on the current value of the global variable errno. It is often used after system calls like exec() to provide information about why the call failed.
- Program calls perror() only when exec() fails, because if exec() succeeds, the current process is replaced by the new program and the code after exec() is never executed.


- waitpid(pid, nullptr, 0) is a system call that waits for the child process with the specified PID to terminate. 
- The second argument is a pointer to an integer where the exit status of the child process will be stored (nullptr means we don't care about the exit status).
-  The third argument is options (0 means no special options).


- pid_t is a data type used to represent process IDs in Unix-like operating systems. It is typically defined as an integer type, and it is used to uniquely identify processes in the system. 
- The pid_t type is used in various system calls and functions related to process management, such as fork(), waitpid(), and kill(). It allows the operating system to keep track of processes and their relationships (parent-child) in a consistent manner.


- exit(1) returns exit code 1 to the parent process, indicating that the child process encountered an error. 
- exit code 1 means "general error" in Unix-like operating systems. 

## Single quotes and double quotes
These matter because a shell must decide how to parse input before executing a command.

- Single quotes (`'...'`) make everything inside literal. The shell does not expand variables, command substitution, or special characters.
- Double quotes (`"..."`) still preserve spaces and prevent word splitting, but they allow variable expansion and command substitution.
- Unquoted text is split on spaces and treated as separate words, and special characters may be interpreted by the shell.
- A shell should handle quotes carefully when splitting input into arguments, because quotes are not passed to the program as ordinary characters.
- Example:
  - `echo '$PATH'` prints the literal `$PATH`
  - `echo "$PATH"` prints the current value of the environment variable
  - `echo hello world` becomes two arguments: `hello` and `world`

While building a shell, the main thing to remember is that quotes affect parsing, not just display. A correct parser must remove the quote characters while keeping the inner content as one argument.
