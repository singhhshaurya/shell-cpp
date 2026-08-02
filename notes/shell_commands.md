# SHELL COMMANDS

1. BUILTINS

# COMPLETE
- complete is a Bash builtin used to define or modify tab completion for commands.
- It allows you to specify how the shell should complete arguments for a given command when the user presses the Tab key.
- EXAMPLE:
```bash
complete -W "start stop restart" myservice
```
- This command sets up tab completion for the `myservice` command, allowing the user to complete the arguments "start", "stop", and "restart" when they press Tab after typing `myservice`.

## OPTION FLAGS IN COMPLETE
- `-W`: Specifies a list of words for completion.
- `-F`: Specifies a function to generate completions dynamically.
- `-C`: Specifies a command to generate completions dynamically.


# ALIAS
- alias is a shell builtin that allows you to create shortcuts for longer commands or command sequences.
- It helps to save time and reduce typing by allowing you to define a new name for a command or a series of commands.
- EXAMPLE:
```bash
alias ll='ls -la'
```
- This command creates an alias `ll` that runs `ls -la`, which lists files in long format, including hidden files.



2. EXECUTABLES
# WC (word count)
- wc is a command-line utility that counts the number of lines, words, and characters in a file or input provided to it.

wc [option] file
- OPTIONS:
    - `-l`: Count lines
    - `-w`: Count words
    - `-c`: Count bytes
    - `-m`: Count characters

3 ways to use wc:
1. from a file: wc file.txt
2. from standard input: wc then type text and press Ctrl+D to end input
3. with a pipe: echo "Hello World" | wc

# TAIL 
- tail is a command-line utility that displays the last part of a file or input provided to it.

tail [option] file
- OPTIONS:
    - `-n`: Specify the number of lines to display (default is 10) 
    - `-f`: Follow the file as it grows, displaying new lines as they are added (useful for log files)
    - `-c`: Specify the number of bytes to display from the end of the file

# HEAD
- head is a command-line utility that displays the first part of a file or input provided to it.

head [option] file
- OPTIONS:
    - `-n`: Specify the number of lines to display (default is 10)
    - `-c`: Specify the number of bytes to display from the beginning of the file



# grep
- grep is a command-line utility used for searching plain-text data for lines that match a specified pattern. It stands for "Global Regular Expression Print."
grep [option] pattern [file]
- OPTIONS:
    - `-i`: Ignore case (case-insensitive search)
    - `-v`: Invert match (show lines that do not match the pattern)
    - `-r` or `-R`: Recursively search directories

Reading from STDIN:
grep "pattern" -> searches for "pattern" in the input provided to it via standard input (STDIN). You can type text directly or pipe output from another command into grep.




# SHELL OPERATORS
1. PIPE (|)
- The pipe operator `|` is used to pass the output of one command as input to another command. It allows you to chain commands together, creating a pipeline of processes.
- The two commands run PARALLELY, and the output of the first command is sent directly to the input of the second command.
- Change stdout of first command to pipe, and stdin of second command to pipe.

2. &&
- The `&&` operator is used to execute the second command only if the first command succeeds (returns an exit status of 0). It is often used for conditional execution of commands. NO PARALLEL EXECUTION. The second command will only run if the first command is successful.

3. ||
- The `||` operator is used to execute the second command only if the first command fails (returns a non-zero exit status). It is often used for error handling or fallback actions. NO PARALLEL EXECUTION. The second command will only run if the first command fails.

4. ;
- The `;` operator is used to separate multiple commands on a single line. Each command is executed sequentially, regardless of the success or failure of the previous command. NO PARALLEL EXECUTION. Each command will run one after the other, regardless of the success or failure of the previous command.

5. &
- The `&` operator is used to run a command in the background. It allows the shell to continue accepting new commands while the specified command runs in the background. The command will run asynchronously

