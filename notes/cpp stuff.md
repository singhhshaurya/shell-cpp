# CPP


alias run='g++ src/main.cpp -o main && ./main'
```cpp


int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
}
```


## COUT : 
- standard output stream. By default, it writes to the terminal (console).
  // standard output stream means that it is the default destination for output from a program. When you use std::cout, the output is sent to the console or terminal window where the program is running.

## CERR: 
  // cerr : standard error stream. By default, it also writes to the terminal (console).
  // Separate from cout, so output can be redirected independently.

//   You can redirect them separately:

// ./program > output.txt        # only cout
// ./program 2> errors.txt       # only cerr
// ./program > out.txt 2> err.txt

// Here, 2 is the file descriptor for stderr.


## BUFFERING
// BUFFERING: the output stream is not immediately sent to the terminal. Instead, it is stored in a buffer (a temporary storage area) and sent to the terminal in larger chunks. This buffering can improve performance, but it can also lead to situations where output is not immediately visible.

// buffer is flushed (sent to the terminal) when:
// - The buffer is full
// - The program ends normally
// - You explicitly flush the buffer using std::flush or std::endl
// - or other implementation-defined events occur.

## UNITBUF
// std::unitbuf is an I/O manipulator.
// It tells a stream:

// Flush after every output operation. Every output operation will be immediately sent to the terminal and visible, rather than being buffered. This is useful for real-time output, such as logging or interactive applications, where you want to see the output immediately. 



## char* vs string
- use str.data() to get a pointer to the underlying character array of a std::string. 
- Used when you need to pass a string to a function that expects a C-style string (char*), such as many functions in the C standard library.

- vector<char*> can also be converted to char*[] using data() method of vector. This is useful when you need to pass an array of C-style strings to a function that expects char*[].


## POSIX 
- POSIX (Portable Operating System Interface) is a family of standards specified by the IEEE for maintaining compatibility between operating systems. POSIX defines the application programming interface (API), along with command line shells and utility interfaces, for software compatibility with variants of Unix and other operating systems.
- Important POSIX functions for C++ programming include:
  - fork(): creates a new process by duplicating the calling process.
  - exec(): replaces the current process image with a new process image.
  - wait(): suspends execution of the calling process until one of its child processes terminates.
  - pipe(): creates a unidirectional data channel that can be used for interprocess communication.
  - signal(): sets a function to handle signals (asynchronous events) sent to the process.
  - cwd(): gets the current working directory of the process.


  #include <unistd.h> for POSIX functions like fork(), exec(), wait(), pipe(), and cwd().

  