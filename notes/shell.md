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



# Variables in Unix Shell and Environment
- ## Shell variables
  - A shell variable exists only inside the shell process. 
  - Exists only inside the shell, child processes cannot access them, getenv() cannot access them.
  ```bash
  NAME="Alice"
  COUNT=10
  HISTFILE=~/.bash_history
  PS1="\u@\h$ "
  ```
- ## Environment variables
  - Environment variables are shell variables that have been exported.
  - They are available to the shell and any child processes created by the shell.
  - They can be accessed by child processes using the getenv() function in C/C++.
  - By 'export' it means that the variable is added to the environment of the shell and its child processes.
  ```bash
  export PATH="/usr/local/bin:$PATH"
  export HOME="/home/user"
  export LANG="en_US.UTF-8"
  ```

  *RELATIONSHIP* Every env variable is a shell variable, but not every shell variable is an env variable. Only those shell variables that have been exported become environment variables.

## Process Environment
  - Every process has its own environment, which is a collection of environment variables that are inherited from the parent process. The environment is used to configure the behavior of the process and provide information about the system and user settings.
  - So an environment is just a collection of strings, KEY=value pair.

  - A shell, when starting executes bunch of EXPORT commands, which sets up the environment for the shell and its child processes. These environment variables can be used by the shell and any programs it runs to customize their behavior based on the user's preferences and system settings.


## Checking if a variable is a shell variable or an environment variable:
- Use the `set` command to list all shell variables and functions. This will show both shell variables and environment variables, but it will not distinguish between them.
- Use the `export` command to list all environment variables. This will show only the variables that have been exported and are available to child processes.
- Use the `env` command to list all environment variables. This will show only the variables that are part of the environment and are available to child processes.
- For particular variable, use `declare -p VARIABLE_NAME` to check if it is a shell variable or an environment variable. If the variable is a shell variable, it will be displayed with the `declare -- ` command. If it is an environment variable, it will be displayed with the `declare -x` command.





##  Path
- The PATH environment variable is a list of directories that the shell searches for executable files when a command is entered.
-  When you type a command, the shell looks through each directory in the PATH variable in order until it finds an executable file with the same name as the command. If it finds one, it runs that file; if not, it returns an error indicating that the command was not found.
  
PATH="/usr/bin:/usr/local/bin:$PATH"
/usr/bin → search here first.
/usr/local/bin → search here second.
$PATH → append whatever directories were already in your PATH.


$

# REPL 
- REPL stands for Read-Eval-Print Loop. It is an interactive programming environment that takes user input (Read), evaluates it (Eval), and returns the result to the user (Print). The loop continues, allowing the user to enter new commands or expressions.

- In the context of a shell, the REPL allows users to enter commands, which are then executed by the shell, and the output is displayed back to the user. This process repeats, allowing for continuous interaction with the shell. The REPL is a fundamental concept in many programming languages and interactive environments, providing a way for users to experiment with code and receive immediate feedback.

# WORKING DIRECTORY
- The working directory is the current directory in which a process is running. 

- The OS keeps track of the working directory for each process, and it is used as the default location for file operations (like opening files) when a relative path is specified.

- The working directory can be changed using the chdir() system call in C/C++ or the cd command in a shell. When you change the working directory, it affects the current process and any child processes that are created after the change.

- However, it does not affect the working directory of the parent process or other unrelated processes. Each process maintains its own working directory, and changes to the working directory are local to that process and its children.


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


## Fork and Exec - CREATING CHILD PROCESSES.
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
  - In the parent process, fork() returns the PID of the child process (>0) *IMPORTANT*
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


## WHAT IS SHARED BETWEEN PARENT AND CHILD PROCESS
- The child process gets a copy of the parent's memory space, including variables, stack, and heap.
- However, the child process has its own separate memory space, so changes made in the child process do not affect the parent process, and vice versa.

- *KERNEL OBJECTS*: Some kernel-managed resources are shared between the parent and child processes, such as *file descriptors*, *pipes*, and *sockets*. This allows for inter-process communication (IPC) between the parent and child processes.



### WORKING CHILD PROCESS IN THE BACKGROUD - SIGNALS

- 1. use pid in the parent process to keep track of the child process. pid in parent process refers to process id of child process.
- 2. while ((pid = waitpid(-1, &status, WNOHANG)) > 0) use this to reap all child processes that have finished executing.

#### Option 1: WNOHANG
- WNOHANG option allows the parent process to continue running without blocking, even if some child processes are still running.

while ((pid = waitpid(-1, &status, WNOHANG)) > 0) 

returns the PID of a child process that has finished executing, or 0 if no child processes have finished yet. The loop continues until all finished child processes have been reaped.


- #### OPTION 2: Signals (SIGCHILD)
- A signal is a small asynchronous notification sent by the operating system.
- Some commong and importang singals to know:
| Signal    | Meaning               |
| --------- | --------------------- |
| `SIGINT`  | Ctrl+C                |
| `SIGTERM` | Termination request   |
| `SIGKILL` | Force kill            |
| `SIGSTOP` | Stop process          |
| `SIGCONT` | Continue process      |
| `SIGCHLD` | A child changed state |

- When any child changes state (usually exits), the kernel sends the parent process a signal calledd `SIGCHLD`.

*ZOMBIE PROCESS*: A zombie process is a process that has completed execution but still has an entry in the process table. It occurs when a child process terminates, but the parent has not yet called wait() to retrieve its exit status. 
- The zombie process remains in the process table until the parent calls wait() or the parent itself terminates. Linux keeps tiny amt of info like PID, exit status, etc. about the zombie process in the process table so that the parent can retrieve it later.


## CLOSING CHILD PROCESSES MID PROCESS
- using kill(pid, SIGKILL) to terminate a child process.
- using file descriptors: if a process uses write to write to a pipe, and the read end of the pipe is closed, the process will receive a SIGPIPE signal, which by default terminates the process. This can be used to close child processes that are writing to a pipe when the parent process no longer needs their output.






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

void reap_finished_jobs(Shell &shell) { // run this function everytime the shell prompt is displayed. it reaps all finished child processes and removes them from the jobs list.

    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		
    }
}



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


## STDOUT AND STDERR
Standard Output (stdout) and Standard Error (stderr) are two separate output streams used by operating systems and programming languages.

| Feature                       | Standard Output (stdout)       | Standard Error (stderr)                 |
| ----------------------------- | ------------------------------ | --------------------------------------- |
| Purpose                       | Displays normal program output | Displays error messages and diagnostics |
| Default destination           | Terminal/console               | Terminal/console                        |
| File descriptor (Unix/Linux)  | 1                              | 2                                       |
| Can be redirected separately? | Yes                            | Yes                                     |


- > operator: 
  - Redirects stdout to a file, overwriting the file if it exists.
  - Example: `ls > output.txt` writes the output of `ls` to `output.txt`, replacing its contents.
- >> operator:
  - Redirects stdout to a file, appending to the file if it exists.
  - Example: `ls >> output.txt` appends the output of `ls` to `output.txt`, preserving its existing contents.
- 2> operator:
  - Redirects stderr to a file, overwriting the file if it exists.
  - Example: `ls non_existent_file 2> error.txt` writes the error message to `error.txt`, replacing its contents.
- 2>> operator:
  - Redirects stderr to a file, appending to the file if it exists.
  - Example: `ls non_existent_file 2>> error.txt` appends the error message to `error.txt`, preserving its existing contents.
- &> operator:
  - Redirects both stdout and stderr to a file, overwriting the file if it exists.
  - Example: `ls > output.txt 2>&1` writes both the output and error messages to `output.txt`, replacing its contents.
- &>> operator:
  - Redirects both stdout and stderr to a file, appending to the file if it exists.
  - Example: `ls >> output.txt 2>&1` appends both the output and error messages to `output.txt`, preserving its existing contents.


## UNIX FILE DESCRIPTORS
- In Unix-like operating systems, almost everything is treated as a file. Regular files, directories, sockets, terminals, pipes, socket connections, and even hardware devices are represented as files.

- Each file is associated with a file descriptor, which is a non-negative integer that uniquely identifies the file within a process.
- FILE DESCRIPTORS are just a small integer that identifies an open file or device.


- When any program starts, the operating system automatically gives it three file descriptors.

Descriptor     Meaning          Default (can be changed.)
----------------------------------------
0              stdin            Keyboard
1              stdout           Terminal
2              stderr           Terminal

STDIN_FILENO   = 0
STDOUT_FILENO  = 1
STDERR_FILENO  = 2

```cpp
std::cout << "Hello";

// internally, it becomes:
write(1, "Hello", 5); // 1 is the file descriptor for stdout. 5 is the number of bytes to write.

// 1 means write to descriptor 1, which is assigned to the terminal.
```

### Opening another file.
- When we open a new file in a program, the operating system assigns it a new file descriptor. The first available file descriptor is usually 3, since 0, 1, and 2 are already taken by stdin, stdout, and stderr.

```cpp
int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

Here fd = 3, which is file descriptor for out.txt. 

Now the process looks like

0 ---> Keyboard
1 ---> Terminal (stdout)
2 ---> Terminal (stderr)
3 ---> out.txt


## CHANGING DEFAULT FILE DESCRIPTORS of COUT AND CIN
- We can change the default file descriptors for stdout and stderr using the dup2() system call.
- By default, stdout (file descriptor 1) and stderr (file descriptor 2) point to the terminal. We can redirect them to a file or another output stream.

```cpp
dup2(3, 1);
```
Means "Make descriptor 1 refer to whatever descriptor 3 refers to."
Therefore, now file descriptor 1 (stdout) points to out.txt instead of the terminal. Any output sent to stdout will now go to out.txt.



### EXAMPLE CODE AND EXPLANATION
```cpp
#include <fcntl.h> // for open() and O_WRONLY, O_CREAT, O_TRUNC.
#include <unistd.h>

int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);   // stdout now goes to out.txt
close(fd);

execvp(argv[0], argv);
```

1. `open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);`
   - Opens the file "out.txt" for writing. If it doesn't exist, it creates it. 
   - O_WRONLY: Open for writing only. It is a flag that specifies the access mode for the file. 
   - O_CREAT: Create the file if it doesn't exist.
   - O_TRUNC: If the file already exists, truncate it to zero length (i.e., clear its contents).
  - 0644: File permissions (owner can read/write, group can read, others can read).

   - Returns a file descriptor (fd) for the opened file.


2. `dup2(fd, STDOUT_FILENO);`
   - Duplicates the file descriptor `fd` (which points to "out.txt") onto `STDOUT_FILENO` (which is 1, the standard output).
   - After this call, any output sent to stdout in this process (like using `std::cout`) will go to "out.txt" instead of the terminal.


3. `close(fd);`
   - Closes the original file descriptor `fd`. After this, `fd` is no longer valid, but stdout (file descriptor 1) still points to "out.txt" because of the previous `dup2` call.

4. `execvp(argv[0], argv);`
    - Replaces the current process with a new process specified by `argv[0]` (the command to execute) and `argv` (the arguments for that command).
    - After this call, the current process is replaced by the new program, and it will inherit the modified stdout (which now points to "out.txt").
    - stout for this process also has been redirected to "out.txt", so any output from the new program will go to that file instead of the terminal.
    - When child process terminates, parent process still has stdout pointing to terminal, because dup2() only affects the current process and its children, not the parent process.


  
# CANONICAL MODE
  - Canonical mode is a mode of input processing in Unix-like operating systems that allows the terminal to process input line by line. In canonical mode, the terminal buffers the input until a newline character (Enter key) is received, at which point it sends the entire line of input to the program for processing. This mode is also known as "cooked" mode.

# NON CANONICAL MODE
  - This is actually the default mode for terminals. 
  - In non-canonical mode, input is processed immediately, character by character, without waiting for a newline. This allows programs to respond to user input in real-time, which is useful for interactive applications like text editors or games.


# TERMINOS
- Terminos is a POSIX API that provides an interface for controlling terminal I/O characteristics. It allows programs to configure terminal settings, such as input and output modes, special characters, and control sequences (non canonocal mode)

- Terminos helps changing terminal settings as to our requirements.

## ICANON FLAG
- This flag decides whether the terminal is in canonical mode or non-canonical mode.
- Using TERMINOS, we can set or unset the ICANON flag to switch between canonical and non-canonical modes.

ICANON = ON -> we get hello\n when pressed enter.
ICANON = OFF -> typing

hello

causes your program to receive

'h'
'e'
'l'
'l'
'o'

immediately.

## ECHO FLAG
- This flag controls whether the terminal echoes (displays) the characters typed by the user.
- When ECHO is enabled, characters typed by the user are displayed on the terminal. When ECHO is disabled, characters typed by the user are not displayed, which is useful for password input

## RAW MODE (non-canonical mode with ECHO disabled)
- Raw mode is a terminal mode that combines non-canonical input processing (ICANON disabled) with the disabling of input echoing (ECHO disabled).
- Now we have control over what is printed on terminal, and what key pressing does what.

## VMIN AND VTIME
- VMIN and VTIME are two parameters used in non-canonical mode to control the behavior of input processing in Unix-like operating systems. They are part of the termios structure, which is used to configure terminal settings.
- VMIN specifies the minimum number of characters that must be read before the read() function returns
- VTIME specifies the maximum amount of time to wait for input before the read() function returns

## SAVING AND RESTORING
- Never leave the user's terminal in raw mode.

Always save the original settings.

```cpp
termios original;

tcgetattr(STDIN_FILENO, &original);
```

Enable raw mode.
When the program exits

```cpp
tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
```

Keyboard
      │
      ▼
Kernel Terminal Driver
      │
      │   (raw mode)
      ▼
Bash
      │
      ├── read one byte
      ├── maintain input buffer
      ├── cursor position
      ├── history
      ├── autocomplete
      ├── redraw line
      ▼
Screen


## CPP Example with explanation
```cpp
#include <iostream>
#include <termios.h>
#include <unistd.h>

int main() {
    termios original, raw;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &original);
    raw = original;

    // Disable canonical mode and echo
    raw.c_lflag &= ~(ICANON | ECHO);

    // Set VMIN and VTIME for non-canonical mode
    raw.c_cc[VMIN] = 1;  // Minimum number of characters to read
    raw.c_cc[VTIME] = 0; // No timeout

    // Apply the new settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    std::cout << "Raw mode enabled. Type something (Ctrl+C to exit):\n";

    char c;
    while (true) {
        read(STDIN_FILENO, &c, 1); // Read one character at a time
        std::cout << "You typed: " << c << std::endl;
    }

    // Restore the original terminal settings before exiting
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    return 0;
}
```
