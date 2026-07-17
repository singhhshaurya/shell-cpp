#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>


using namespace std;
using namespace filesystem;

vector<string> split(string s, char delimeter=' '){
	vector<string> ans;
	int ptr = 0; 

	for(int i=0; i<s.size(); i++){
		if(s[i]==delimeter){
			ans.push_back(s.substr(ptr, i-ptr));
			ptr = i+1;
		}	
	}
	ans.push_back(s.substr(ptr, s.size()-ptr));
	return ans;
}


unordered_set<string> builtins = {"echo", "exit", "type", "pwd"};
string curr_directory  = getcwd(nullptr, 0); // posix function to get current directory.



string program_find_in_path(string command){
	string PATH = getenv("PATH"); // gets path from environment.
	// cout << PATH << "\n";

	for(string dir:split(PATH, ':')){
		path candidate = path(dir) / command;

		if (exists(candidate) &&
			is_regular_file(candidate) &&
			access(candidate.c_str(), X_OK) == 0) {
			return candidate.string();
		}
	}
	return "";
}


bool is_executable(string path){
	return access(path.c_str(), X_OK) == 0;
}


void execute_program(string& program, vector<string>& args){
	string path = program_find_in_path(program);

	vector<char*> argv = {program.data()};
	for(int i=0; i<args.size(); i++){
		argv.push_back(args[i].data());
	}
	argv.push_back(nullptr);

	if(path!="" && is_executable(path)) {
		pid_t pid = fork();
		if (pid < 0) {
			std::cerr << "fork() failed\n";     // fork failed

		} else if (pid == 0) {
			// child process
			execv(path.data(), argv.data());
			perror("execv"); // execv() failed
			exit(1); // exit child process with error code

		} else {
			// parent process
			waitpid(pid, nullptr, 0);
			cout << "\033[A";


		}

	}
	else cout << program << ": command not found";

}


	
bool invalid_command(string s){
	return builtins.find(s) == builtins.end();
}

// BUILTIN COMMANDS

int type(vector<string>& args){
	// check if its builtin
	string command = args[0];
	if(builtins.find(command) != builtins.end()){
		cout << command << " is a shell builtin";
		return 1;
	}


	// check if its in path and executable.
	string path = program_find_in_path(command);
	if(path != "" && is_executable(path)){
		cout << command << " is " << path;
		return 1;
	}

	cout << command << ": not found";
	return 1;
}


void echo(vector<string>& args){
	for(int i=0; i<args.size()-1; i++) cout << args[i] << " ";
	cout << args.back();
}


void pwd(vector<string>& args){
	cout << curr_directory;
}



void cd(vector<string>& args){
	if(args.size()==0){
		curr_directory = const_cast<char*>("");

	}else if(args.size()>1){
		cout << "cd: too many arguments";
	}
	else{
		string path = args[0];
		if(path[0]=='/'){ // absolute path
			if(exists(path)){
				curr_directory = path;
			}
			else {
				cout << path << ": no such file or directory" << "\n";
			}
		}
	}
	cout << "\033[A"; // remove the last "\n"

}


// EXECUTION
unordered_map<string, function<void(vector<string>&)>> commands = {{"echo", echo}, {"type", type}, {"pwd", pwd}, {"cd", cd}};
void execute_line(string& command, vector<string>& args){
	if (commands.find(command)!=commands.end()){
		commands[command](args); // execute that 
	}else{
		execute_program(command, args);
	}
}


int main() {
  // Flush after every std::cout / std:cerr
	cout << std::unitbuf;
	cerr << std::unitbuf;

	string line;
	
	while(true){
		vector<string> commands_executed = {};

		// cout << curr_directory << "$ ";
		cout << "$";
		if(!getline(cin, line)) break;
		commands_executed.push_back(line);


		// parsing the line.

		vector<string> tokens = split(line);
		string command = tokens[0];
		if (command.empty()) {
			continue;
		}

		vector<string> args(tokens.begin()+1, tokens.end());

		if(command == "exit") break;


		execute_line(command, args);
		cout << "\n";

	}
}

