# CPP

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

