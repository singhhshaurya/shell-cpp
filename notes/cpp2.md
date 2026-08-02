# IMPORTING FUNCTIONS FROM ANOTHER MODULE IN CPP
- In C++, you can import functions from another module (or file) by using the `#include` directive to include the header file that contains the function declarations. The implementation of the functions should be in a separate source file (with a .cpp extension).

- 1. Create a header file (e.g., `my_functions.h`) that contains the function declarations.
- 2. Create a source file (e.g., `my_functions.cpp`) that contains the function definitions (implementations).
- 3. In your main program file (e.g., `main.cpp`), include the header file using `#include "my_functions.h"` and call the functions as needed.

- #pragma once --> This is a preprocessor directive that prevents the header file from being included multiple times in the same compilation unit.






# PIPE
- pipe() is one of the simplest inter-process communication (IPC) mechanisms in Unix.
- IPC because kernel manages the pipe and allows communication between processes. Pipes are shared between processes instead of having own copies.

- A pipe is a kernel-managed byte buffer with two ends:

Read end — you can only read from it.
Write end — you can only write to it.

- Pipe is unidirectional, meaning data flows in one direction: from the write end to the read end. It is used for communication between processes, allowing one process to send data to another.
- It is used in scenarios where you want to connect the output of one process to the input of another, enabling a form of communication between them.

- In cpp, pipe() function from the POSIX library is used to create a pipe. It takes an array of two integers as an argument, which will hold the file descriptors for the read and write ends of the pipe.

- WRITING: either use write() function, or change STOUT using dup2() to redirect the standard output to the write end of the pipe, and then use std::cout to write to the pipe. Can also use ofstream to write to the pipe.

- READING: either use read() function, or change STDIN using dup2() to redirect the standard input to the read end of the pipe, and then use std::cin to read from the pipe. Can also use ifstream to read from the pipe.

*IMPORTANT: PIPE IS KERNEL MANAGED BYTE BUFFER. THAT MEANS WHEN FORKING A CHILD PROCESS, THE CHILD WILL HAVE ITS OWN COPY OF THE PIPE FILE DESCRIPTORS BUT THE MAIN PIPE REMAINS SHARED, unlike other data structures.*

write end  ----->  kernel buffer  ----->  read end


```cpp
#include <unistd.h>

int pipefd[2];
pipe(pipefd);

// pipefd[0] -> read end
// pipefd[1] -> write end
```
- pipe() function allocates a pipe object in kernel memory, allocates an internal buffer, and returns two file descriptors: one for the read end (pipefd[0]) and one for the write end (pipefd[1]). 
- These file descriptors can be used with read() and write() functions to communicate between processes.
- Returns 0 on success, -1 on failure


## WRITING IN PIPE
1. Using write() function.
```cpp
#include <unistd.h>
int pipefd[2];
pipe(pipefd);
write(pipefd[1], "Hello, Pipe!", 12); // write to the write end of the pipe
``` 

2. Using dup2() to redirect STDOUT to the write end of the pipe.
```cpp
#include <unistd.h>
int pipefd[2];
pipe(pipefd);
dup2(pipefd[1], STDOUT_FILENO); // redirect STDOUT to the write end of the pipe
std::cout << "Hello, Pipe!" << std::endl; // write to the pipe using std::cout
``` 

## READING FROM PIPE
1. Using read() function.
```cpp
#include <unistd.h>
int pipefd[2];
pipe(pipefd);
char buffer[128];
read(pipefd[0], buffer, sizeof(buffer)); // read from the read end of the pipe
std::cout << buffer << std::endl; // print the data read from the pipe
``` 

2. Using dup2() to redirect STDIN to the read end of the pipe.
```cpp
#include <unistd.h>
int pipefd[2];
pipe(pipefd);
dup2(pipefd[0], STDIN_FILENO); // redirect STDIN to the read end of the pipe
std::string input;
std::cin >> input; // read from the pipe using std::cin
std::cout << input << std::endl; // print the data read from the pipe
```

## CLOSING PIPE DESCRIPTORS.
- After you are done using the pipe, it is important to close the file descriptors to free up system resources. You can use the close() function to close the read and write ends of the pipe.

```cpp
// IF PIPE NEEDED FOR READ ONLY.
int pipefd[2];
pipe(pipefd);

close(pipefd[1]); // close the write end of the pipe at the beginning as no need of it.
dup2(pipefd[0], STDIN_FILENO);
close(pipefd[0]); // close the read end of the pipe. 


```
INITIALLY
fd 0 (stdin)  ─────► Terminal

fd 3 ──────────────► Pipe (READ end) 
fd 4 ──────────────► Pipe (WRITE end)

AFTER close(pipefd[1]) and dup2(pipefd[0], STDIN_FILENO)

fd 0 (stdin)  ─────► Pipe (READ end)
fd 3 ──────────────► Pipe (READ end)

AFTER close(pipefd[0])
fd 0 (stdin)  ─────► Pipe (READ end) only remains.

- When a process ends, all its file descriptors are automatically closed, including those for the pipe.

## CLOSING PROGRAMS USING PIPES
- If another process is using write fd of the pipe and all the read fds are closed, the writing process will receive a SIGPIPE signal, which by default terminates the process. If the writing process ignores the SIGPIPE signal, the write() function will return -1 and set errno to EPIPE.