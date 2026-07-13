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

unordered_set<string> builtins = {"echo", "exit", "type"};
// string PATH = "/usr/bin:/usr/local/bin:/tmp/pig:/tmp/owl:/tmp/dog:";

string program_find_in_path(string command){
	string PATH = getenv("PATH"); // gets path from environment.

	for(string path:split(PATH, ':')){
		if(!is_directory(path)) continue;
		for (auto& entry : directory_iterator(path)) {
			if (entry.is_regular_file()){
				auto filename = entry.path().filename();
				if(filename.string() == command) {
					return path + "/" + filename.string();
					
				}
			}
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




unordered_map<string, function<void(vector<string>&)>> commands = {{"echo", echo}, {"type", type}};
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
		cout << "$ ";

		if(!getline(cin, line)) break;

		
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

