# CPP

alias run='g++ src/main.cpp src/helping_functions.cpp src/key_bindings.cpp -o main && ./main'

chmod +x .git/hooks/pre-commit    ---   makes a file executable

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


  ## ESCAPE SEQUENCES

  | Escape | Meaning                                     | Example                   |
| ------ | ------------------------------------------- | ------------------------- |
| `\n`   | New line                                    | `cout << "Hello\nWorld";` |
| `\t`   | Horizontal tab                              | `cout << "A\tB";`         |
| `\b`   | Backspace                                   | `cout << "ABC\bD";`       |
| `\r`   | Carriage return (move to beginning of line) | `cout << "Hello\rHi";`    |
| `\\`   | Backslash (`\`)                             | `cout << "\\";`           |
| `\'`   | Single quote (`'`)                          | `char c = '\'';`          |
| `\"`   | Double quote (`"`)                          | `cout << "\"Hello\"";`    |
| `\0`   | Null character                              | `char c = '\0';`          |
| `\a`   | Alert/Bell (beep, if supported)             | `cout << "\a";`           |
| `\f`   | Form feed                                   | Rarely used               |
| `\v`   | Vertical tab                                | Rarely used               |
| `\?`   | Literal `?` (rarely needed)                 | `cout << "\?";`           |


## TERMINOS (raw mode) IMPORTANT SEQUENCES. (check out shell for terminos and raw mode.)
### CURSOR MOVEMENTS 
| Sequence   | Action                     | Typical use            |
| ---------- | -------------------------- | ---------------------- |
| `\033[A`   | Move cursor up 1 line      | Command history redraw | (ESC [ A) . 
| `\033[B`   | Move cursor down 1 line    | Menus                  |
| `\033[C`   | Move cursor right 1 column | Cursor editing         |
| `\033[D`   | Move cursor left 1 column  | Left arrow editing     |
| `\033[5A`  | Move up 5 lines            | Multi-line editing     |
| `\033[10C` | Move right 10 columns      | Jump cursor            |

### CURSOR POSITIONING - IMPORTANT FOR TERMINOS (raw mode).
| Sequence        | Action               | Use                |
| --------------- | -------------------- | ------------------ |
| `\033[H`        | Top-left corner      | Home position      |
| `\033[row;colH` | Move to row & column | Drawing interfaces |
| `\033[f`        | Same as `H`          | Cursor positioning |


### CLEARING
| Sequence  | Action                     | Use                    |
| --------- | -------------------------- | ---------------------- |
| `\033[2J` | Clear entire screen        | `clear` command        |
| `\033[J`  | Clear from cursor downward | Partial refresh        |
| `\033[K`  | Clear to end of line       | Updating prompts       |
| `\033[2K` | Clear whole current line   | Redrawing command line |



### ARROW KEYS
| Key | Bytes received |
| --- | -------------- |
| ↑   | `\033[A`       |
| ↓   | `\033[B`       |
| →   | `\033[C`       |
| ←   | `\033[D`       |


### TEXT FORMATTING
| Escape    | Meaning        |
| --------- | -------------- |
| `\033[0m` | Reset          |
| `\033[1m` | Bold           |
| `\033[4m` | Underline      |
| `\033[7m` | Reverse colors |

### COLORS

`\033[30m` Black
`\033[31m` Red
`\033[32m` Green
`\033[33m` Yellow
`\033[34m` Blue
`\033[35m` Magenta
`\033[36m`  Cyan
`\033[37m` White


## WRITE AND READ IN FILES IN CPP - FSTREAM HEADER.
- For writing, use ofstream and for reading use ifstream. 
- ofstream and ifstream are classes provided by the fstream header in C++ for file handling. ofstream is used to create and write to files, while ifstream is used to read from files.
- Can also use write() and read() POSIX functions with file descriptors to write and read from files. These functions are lower-level and provide more control over file operations, but they require more manual management of file descriptors and buffers.


1. WRITE - METHOD 1 FSTREAM.
```cpp
#include <iostream>
#include <fstream>
int main() {
    ofstream file("data.txt");

    file << "Hello, File!" << endl;
    file << "C++ File Handling";

    file.close();
    return 0;
}
```

2. WRITE - METHOD 2 write() function.
```cpp
#include <fcntl.h>
#include <unistd.h>
int main() {
    int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // open using file descriptor. 

    write(fd, "Hello\n", 6);

    close(fd);
}
```


3. READ - FSTREAM.
```cpp
#include <iostream>
#include <fstream>
int main() {
    ifstream file("data.txt");

    string line;
    while (getline(file, line)) { // stores each line in the variable 'line'. 
        cout << line << endl;
    }

    file.close();
    return 0;
}


4. READ - read() function.
```cpp
#include <fcntl.h>
#include <unistd.h>
int main() {
    int fd = open("out.txt", O_RDONLY); // open using file descriptor.
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(fd);
}
```


| Python | C++                                                                             |
| ------ | ------------------------------------------------------------------------------- |
| `"r"`  | `std::ifstream file("f.txt");`                                                  |
| `"w"`  | `std::ofstream file("f.txt");`                                                  |
| `"a"`  | `std::ofstream file("f.txt", std::ios::app);`                                   |
| `"r+"` | `std::fstream file("f.txt", std::ios::in \| std::ios::out);`                    |
| `"w+"` | `std::fstream file("f.txt", std::ios::in \| std::ios::out \| std::ios::trunc);` |
| `"a+"` | `std::fstream file("f.txt", std::ios::in \| std::ios::out \| std::ios::app);`   |
```


