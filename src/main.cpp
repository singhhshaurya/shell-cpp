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


vector<string> get_args(string& command){
	bool single_quotes_closed = 1;
	bool double_quotes_closed = 1;
	bool back_slash = 0;
	vector<string> args;

	string curr;
	for(char c:command){
		if(back_slash){ 
			curr += c; // no special meaning just add that shi
			back_slash = 0;
		}
		// backslashes
		else if(c == '\\' && single_quotes_closed) back_slash = 1;
		

		// quotation marks
		else if(c=='\'' && double_quotes_closed) single_quotes_closed = !single_quotes_closed;
		else if(c=='\"' && single_quotes_closed) double_quotes_closed = !double_quotes_closed;

		// space
		else if(c == ' ' && single_quotes_closed && double_quotes_closed){
			if(curr != "") args.push_back(curr);
			curr = "";
		}


		else curr += c;

	}
	args.push_back(curr);
	return args;

}

const string remove_line = "\033[A";
const string backspace = "\b";

unordered_set<string> builtins = {"echo", "exit", "type", "pwd"};
path curr_directory  = getcwd(nullptr, 0); // posix function to get current directory. use chdir() to change it. 


string get_directory(){
	if(curr_directory == "" or curr_directory == "/") return getenv("HOME");
	return curr_directory;
}

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

	string output_path;
	int output = 0;

	for(int i=0; i<args.size(); i++){
		if(args[i] == ">" || args[i] == "1>") {
			output = 1;
			continue;
		}

		if(output) output_path += args[i];
		else argv.push_back(args[i].data());
	}
	argv.push_back(nullptr);

	// executing the program.

	if(path!="" && is_executable(path)) {
		pid_t pid = fork();
		if (pid < 0) {
			std::cerr << "fork() failed\n";     // fork failed

		} else if (pid == 0) {
			// child process
			if(output){
				int fd = open(output_path.data(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
				dup2(fd, STDOUT_FILENO); // STDOUT_FILENO is basically 1.
				close(fd);
			}

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
	string output_file;
	string output;
	bool add_to_output = 0;

	for(string s:args){
		if(s == ">" or s == "1>") {
			add_to_output = 1;
			continue;
		}

		if(add_to_output) output_file += s;
		else{
			output += s + " ";
		}
	} 

	if(output_file != ""){
		ofstream file(output_file);
		file << output;
		cout << remove_line;

	}else{
		cout << output << backspace;
	}
}


void pwd(vector<string>& args){
	if(curr_directory == "" or curr_directory == "/"){
		cout << getenv("HOME");
	}
	else cout << curr_directory.string();
}



void cd(vector<string>& args){
	if(args.size()==0){
		curr_directory = const_cast<char*>("");

	}else if(args.size()>1){
		cout << "cd: too many arguments" << "\n";
	}
	else{
		string path = args[0];
		if(path[0]=='/'){ // absolute path
			if(exists(path)){
				curr_directory = path;
			}
			else {
				cout << "cd: " << path << ": No such file or directory" << "\n";
			}
		}
		else{
			filesystem::path temp_curr_directory = curr_directory;
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
						cout << "cd: " << path << ": No such file or directory" << "\n";
						break;
					}
				}
			}
			curr_directory = temp_curr_directory;
			chdir(curr_directory.string().data()); // changes the current working directory of the process to the new directory.

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

		// cout << get_directory() << "$ ";
		cout << "$ ";
		if(!getline(cin, line)) break;
		commands_executed.push_back(line);


		// parsing the line.

		vector<string> tokens = get_args(line); 
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

