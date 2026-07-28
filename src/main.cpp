#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <unordered_set>

#include <filesystem> // directories, navigation, opening files and programs.
#include <unistd.h> // standard posix functions.
#include <sys/wait.h> // wait process
#include <fstream> // for normal writing and reading in a file.
#include <fcntl.h> // for open(), file descriptor changing.

#include "shell.h"
#include "helping_functions.h"
#include "key_bindings.h"


using namespace std;
using namespace filesystem;


Shell shell;








bool invalid_command(string s){
	return shell.builtins.find(s) == shell.builtins.end();
}

// BUILTIN COMMANDS

int type(vector<string>& args){
	// check if its builtin
	string command = args[0];
	if(shell.builtins.find(command) != shell.builtins.end()){
		cout << command << " is a shell builtin" << endl;
		return 1;
	}


	// check if its in path and executable.
	string path = program_find_in_path(command);
	if(path != "" && is_executable(path)){
		cout << command << " is " << path << endl;
		return 1;
	}

	cout << command << ": not found" << endl;
	return 1;
}


void echo(vector<string>& args){
	string output_file;
	string output;
	int add_to_output = 0;

	string error_file;
	int error = 0;

	for(string s:args){
		if(s == ">" or s == "1>" or s == ">>" or s == "1>>") {
			if(add_to_output){
				create_file(output_file, add_to_output-1);
				output_file = "";
			}
			
			add_to_output = 1;
			error = 0;
			if(s == ">>" or s == "1>>"){
				add_to_output = 2;
			}
			continue;
		}
		if(s == "2>" or s == "2>>"){
			if(error){
				create_file(error_file, error-1);
			}
			error = 1;
			add_to_output = 0;
			if(s == "2>>") error = 2;
			continue;
		}

		if(add_to_output) output_file += s;
		else if(error) error_file += s;
		else{
			output += s + " ";
		}
	} 
	if(error){
		int fd = create_file(error_file, error);
		close(fd);
	}
	if(output_file != ""){
		ofstream file;

		if (add_to_output == 2){
			file.open(output_file, std::ios::app); // append.
		}else{
			file.open(output_file);
		}		
		file << output << '\n';
	}
	else{
		cout << output << shell.backspace << endl;
	}
}


void pwd(vector<string>& args){
	if(shell.curr_directory == "" or shell.curr_directory == "/"){
		cout << getenv("HOME") << endl;
	}
	else cout << shell.curr_directory.string() << endl;

}


void cd(vector<string>& args){
	if(args.size()==0){
		shell.curr_directory = const_cast<char*>("");

	}else if(args.size()>1){
		cout << "cd: too many arguments" << endl;
	}
	else{
		string path = args[0];
		if(path[0]=='/'){ // absolute path
			if(exists(path)){
				shell.curr_directory = path;
			}
			else {
				cout << "cd: " << path << ": No such file or directory" << endl;
			}
		}
		else{
			filesystem::path temp_curr_directory = shell.curr_directory;
			vector<string> folders = split(path, '/');

			for(string folder:folders){
				if(folder=="." || folder == "") continue;

				else if(folder == ".." && temp_curr_directory.has_parent_path()) {
					temp_curr_directory = temp_curr_directory.parent_path();
				}
				else if(folder == "~"){
					temp_curr_directory = "";

				}
				else{
					if(exists(temp_curr_directory / folder)) temp_curr_directory = temp_curr_directory / folder;
					else{
						cout << "cd: " << path << ": No such file or directory" << endl;
						break;
					}
				}
			}
			shell.curr_directory = temp_curr_directory;
			chdir(shell.curr_directory.string().data()); // changes the current working directory of the process to the new directory.

		}
	}
}

void complete(vector<string>& args){
	string option_flag = args[0];

	if(option_flag == "-p"){
		string name = args[1];
		if(shell.tab_completions.find(name) == shell.tab_completions.end()){
			cout << "complete: " << name << ": no completion specification\n";
		}else{
			cout << "complete " << shell.tab_completions[name].first << " '" << shell.tab_completions[name].second << "' " << name << "\n";
		}
	}
	else{
		string command = args[1];
		string name = args[2];
		
		shell.tab_completions[name] = {option_flag, command};
	}
}
// EXECUTION


void execute_line(string& command, vector<string>& args){
	if (shell.commands.find(command)!=shell.commands.end()){
		shell.commands[command](args); // execute that 
	}else{
		execute_program(command, args);
	}
}



int main() {
	shell.builtins = {"echo", "exit", "type", "pwd", "complete"};
	shell.commands = {{"echo", echo}, {"type", type}, {"pwd", pwd}, {"cd", cd}, {"complete", complete}}; // add all the builtins.
    // Flush after every std::cout / std:cerr
    // # REPL  Read-Eval-Print Loop
	
	cout << std::unitbuf;
	cerr << std::unitbuf;

	enableRawMode();
	get_all_executables(shell);


	int TERMINAL_OUT = dup(STDOUT_FILENO);
	int TERMINAL_IN = dup(STDIN_FILENO);

	while(true){
		// cout << get_directory(shell) << "$ ";
		cout << "$ ";

		bool go = 1;
		int tab_count = 0;

		while(go){
			char c;
			read(STDIN_FILENO, &c, 1);

			switch (c) {
			case 27: {
				char seq[2];
				read(STDIN_FILENO, &seq[0], 1);
				read(STDIN_FILENO, &seq[1], 1);

				if (seq[0] == '[') {
					switch (seq[1]) {
					case 'A':
						onUp(shell);
						break;

					case 'B':
						onDown(shell);
    					break;

					case 'D':
						onLeft(shell);
						break;

					case 'C':
						onRight(shell);
						break;
					}
				}
				break;
			}
			case 9: // tab
				onTab(shell, tab_count);	
				break;

			case 127: // backspace.
				onBackspace(shell);
				break;

			case '\n':
				cout << '\n';
				go = 0;
				break;

			default:
				cout << c << shell.line.substr(shell.leftright_ptr, shell.line.size() - shell.leftright_ptr);
				cout << "\r" << "\033[" << shell.leftright_ptr+3 << "C"; // brings cursor back to where shell.leftright_ptr is.
				shell.line.insert(shell.line.begin()+shell.leftright_ptr, c);
				shell.leftright_ptr++;
				break;
			}

		}

		if (!shell.line.empty() &&  (shell.history.empty() || shell.line != shell.history.back())) shell.history.push_back(shell.line);
		shell.updown_ptr = shell.history.size();
		// parsing the line.

		vector<string> tokens = get_args(shell.line); 
		string command = tokens[0];
		if (command.empty()) {
			continue;
		}

		vector<string> args(tokens.begin()+1, tokens.end());

		if(command == "exit") break;

		execute_line(command, args);
		shell.line.clear();
		shell.leftright_ptr = 0;
	}
	disableRawMode();
}

