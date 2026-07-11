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