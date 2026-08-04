#include <iostream>
#include <cstdlib> // for exit(), getenv(), system(), setenv().
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

void exit0(vector<string>& args){
	shell.terminate_shell = 1;
}

void type(vector<string>& args){
	// check if its builtin
	string command = args[0];
	if(shell.aliases.find(command)!=shell.aliases.end()) {
		cout << command << " is aliased to `" << shell.aliases[command] <<"'" <<endl;
		return;
	}
	if(shell.builtins.find(command) != shell.builtins.end()){
		cout << command << " is a shell builtin" << endl;
		return;
	}


	// check if its in path and executable.
	string path = program_find_in_path(shell, command);
	if(path != "" && is_executable(path)){
		cout << command << " is " << path << endl;
		return;
	}

	cout << command << ": not found" << endl;
	return;
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
		if(!output.empty()) output.pop_back();
		cout << output << endl;
	}
}


void pwd(vector<string>& args){
	if(shell.curr_directory == "" or shell.curr_directory == "/"){
		cout << shell.variables["HOME"] << endl;
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
	else if(option_flag == "-r"){
		string command = args[1];
		shell.tab_completions.erase(command);
	}
	else{
		string command = args[1];
		string name = args[2];
		
		shell.tab_completions[name] = {option_flag, command};
	}
}


void jobs(vector<string>& args){	
	reap_finished_jobs(shell, false); // after execution but before next command.

	Job curr_job;
	for(int i=0; i<shell.background_jobs.size(); i++){
		curr_job = shell.background_jobs[i];
		cout << "[" << curr_job.job_no << "]"; 

		if(i == shell.background_jobs.size()-1) cout << "+";
		else if(i == shell.background_jobs.size()-2) cout << "-";
		else cout << " ";

		cout << "  " << curr_job.status;
		for(int i=0; i<24-curr_job.status.size(); i++) cout << " ";
		cout << curr_job.command;
		if(curr_job.status != "Running") cout << "\b\b  \b\b";
		cout << "\n";
	}

	int ptr = 0;
	while(ptr < shell.background_jobs.size()){
		if(shell.background_jobs[ptr].status!="Running") shell.background_jobs.erase(shell.background_jobs.begin()+ptr);
		else ptr++;
	}

}


void history(vector<string>& args){
	int count = -1;
	if(args.size()) {
		if(is_digits(args[0])) count = stoi(args[0]);
		else if(args[0] == "-r" || args[0]=="-w" || args[0]=="-a"){
			string path;
			if(args.size()==1) path = shell.variables["HISTFILE"];
			else path = args[1];

			if(args[0]=="-r") read_history(shell, path);
			else if(args[0]=="-w"){
				ofstream file(path);  
				for(string s:shell.history) file << s <<"\n";
			}
			else if(args[0]=="-a"){
				ofstream file(path, ios::app);
				for(int i = shell.history_last_appended; i<shell.history.size(); i++){
					file << shell.history[i] << "\n";
				}
				shell.history_last_appended = shell.history.size();
			}
		}
		else if(args[0] == "-c"){
			shell.history.clear();
			shell.updown_ptr = 0;
		}
		else{
			cout << "history: " << args[0] << ": numeric agent required\n";
			return;
		}

	}
	else count = shell.history.size();

	for(int i=shell.history.size()-count; i<shell.history.size(); i++){
		cout << "    " << i+1 << "  " << shell.history[i] << "\n";
	}
	
}


void declare(vector<string>& args){
    if(args[0] == "-p"){
		for(int i=1; i<args.size(); i++){
			if(shell.variables.find(args[i]) != shell.variables.end()){
				cout << "declare -" << (shell.exported_vars.find(args[i])!=shell.exported_vars.end()?"x":"-") << " " <<
				args[i] << "=\"" << shell.variables[args[i]] << "\"" << endl;
			}else{
				cout << "declare: " << args[i] <<": not found" << endl;
			}
		}
	}else{
		for(int i=0; i<args.size(); i++) {
			vector<string> tokens = split(args[i], '=', 1);
			string key=tokens[0]; 
			string value= tokens[1];
			if(!valid_variable_name(key)) {
				cout << "declare: `" << args[i] <<"\': not a valid identifier" << endl;
			}
			else{
				shell.variables[key] = value;
			}
		}
	}
}


void alias(vector<string>& args){
	for(string s:args){
		if(s.find("=")==string::npos){
			if(shell.aliases.find(s)!=shell.aliases.end()){
				cout << "alias " << s <<"='" << shell.aliases[s] << "'" << endl;
			}else{
				cout << "alias: " << s << ": not found";
			}
		}
		else{
			vector<string> tokens = split(s, '=', 1);
			string name = tokens[0]; string value = tokens[1];
			shell.aliases[name] = value;
		}
	}
}
// EXECUTION

void get_commands(vector<string>& tokens);


void execute_command(string& command, vector<string>& args, int* pipe_out=NULL, int* pipe_in=NULL, bool background=false){
	if(shell.aliases.find(command)!= shell.aliases.end()){
		vector<string> tokens = get_args(shell, shell.aliases[command]); 
		get_commands(tokens); // gets different commands and executes them. 
	}
	else if (shell.commands.find(command)!=shell.commands.end()){ // builtin.
		shell.commands[command](args); // execute that 
	}else{
		string path;
		if(split(command, '/').size()>1){
			path = command;
			if(!exists(path)) {
				cout << command << ": No such file or directory" << endl;
				return;
			}
			else if(is_directory(path)) {
				cout << command << ": Is a directory" << endl;
				return;
			}
		}
		else path = program_find_in_path(shell, command);

		if(path == "") cout << command << ": command not found\n";
		else {
			pid_t pid;
			if(args.size() && args.back() == "&"){
				args.pop_back();
				int curr_jobs = shell.background_jobs.size();
				pid = execute_program(shell, path, args, pipe_out, pipe_in, true); // background = true.

				auto& job = shell.background_jobs.back();
				cout << "[" << job.job_no << "] " << job.process_id << "\n"; 
			}
			else{
				pid = execute_program(shell, path, args, pipe_out, pipe_in, background);
			}

			if(pid==-1)  cout << command << ": Permission denied" << endl;
		}
	}
}


void get_commands(vector<string>& tokens){
	string command;
	vector<string> args;

	bool piped = false;
	int pipefd[2];
	pipe(pipefd);

	pid_t pid1;
	pid_t pid2;
	tokens.push_back("&&");
	vector<pair<pid_t, pair<int, int>>> processes_running;

	for(string s:tokens){
		if(command.empty()) command = s;
		else{
			if(s == "&&" || s == "|"){ 
				if(s == "|" || (s == "&&" && !processes_running.empty())){
					int pipe_out[2];
					if(s == "&&"){
						pipe_out[1] = STDOUT_FILENO;
					}
					else pipe(pipe_out);

					int pipe_in[2];
					if(processes_running.empty()) {
						pipe_in[0] = STDIN_FILENO;
					}
					else{
						pipe_in[0] = processes_running.back().second.first;
						pipe_in[1] = processes_running.back().second.second;
					}
					pid_t pid = fork();

					if(pid == 0){
						dup2(pipe_in[0], STDIN_FILENO);
						dup2(pipe_out[1], STDOUT_FILENO);
						execute_command(command, args);
						_exit(0);
					}
		
					if(pipe_in[0] != STDIN_FILENO) close(pipe_in[0]);
					if(pipe_out[1] != STDOUT_FILENO) close(pipe_out[1]);

					processes_running.push_back({pid, {pipe_out[0], pipe_out[1]}});

					if(s == "&&"){
						for (auto &i : processes_running) {
							waitpid(i.first, nullptr, 0);
						}
						processes_running.clear();
					
					}
				}
				else if(s == "&&") {
					execute_command(command, args);
				}

				command.clear(); args.clear();
			}
			else args.push_back(s);
		}
	}
}



int main() {
	shell.builtins = {"echo", "exit", "type", "pwd", "complete", "jobs", "history", "alias", "export", "declare", "alias"};
	shell.commands = {{"echo", echo}, {"type", type}, {"pwd", pwd}, {"cd", cd}, {"complete", complete}, {"jobs", jobs}, {"exit", exit0},
					  {"history", history}, {"declare", declare}, {"alias", alias}}; // add all the builtins.

	for(string s:shell.exported_vars){
		shell.variables[s] = env_or_default(s.data()); // default is "" for now.
	}

    // Flush after every std::cout / std:cerr
    // # REPL  Read-Eval-Print Loop
	
	cout << std::unitbuf;
	cerr << std::unitbuf;

	enableRawMode();
	get_all_executables(shell);
	read_history(shell); // get history from previous sessions.


	int TERMINAL_OUT = dup(STDOUT_FILENO);
	int TERMINAL_IN = dup(STDIN_FILENO);
	ofstream history_write_file(shell.variables["HISTFILE"], ios::app);  


	while(!shell.terminate_shell){
		if(shell.incomplete_command) cout << "\r> ";
		// else cout << get_directory(shell) << "$ ";
		else cout << "\r$ ";

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

		if(!is_command_complete(shell)){
			shell.line += "\n";
			shell.prev_line += shell.line;
			shell.line.clear();
			shell.leftright_ptr = 0;
			shell.incomplete_command = 1;
		}
		else{
			shell.line = shell.prev_line + shell.line;
			shell.incomplete_command = 0;
			shell.prev_line.clear();
		}

		vector<string> tokens = get_args(shell, shell.line); 
		if(tokens.empty()){
			shell.line.clear();
			shell.leftright_ptr = 0;
			continue;
		}

		if (!shell.line.empty() &&  (shell.history.empty() || shell.line != shell.history.back())) {
			shell.history.push_back(shell.line);
			history_write_file << shell.line << endl;
		}
		shell.updown_ptr = shell.history.size();
		// parsing the line.

		
		disableRawMode();
		get_commands(tokens); // gets different commands and executes them. 
		enableRawMode();

		reap_finished_jobs(shell); // after execution but before next command.

		shell.line.clear();
		shell.leftright_ptr = 0;
	}
	disableRawMode();
}

