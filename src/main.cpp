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
string PATH = getenv("PATH"); // gets path from environment.

// string PATH = "/usr/bin:/usr/local/bin";
bool invalid_command(string s){
	return builtins.find(s) == builtins.end();
}


int type(string& command){
	if(builtins.find(command) != builtins.end()){
		cout << command << " is a shell builtin";
		return 1;
	}

	for(string path:split(PATH, ':')){
		if(!is_directory(path)) continue;
		for (auto& entry : directory_iterator(path)) {
			if (entry.is_regular_file()){
				string filename = entry.path().filename().string();
				if(filename == command && 
				access(path.c_str(), X_OK) == 0) {

					cout << command << " is " << path << "/" << command;
					return 1;
				}
			}
		}
	}
	cout << command << ": not found";
	return 1;
}


void echo(string& s){
	cout << s;
}



unordered_map<string, function<void(string&)>> commands = {{"echo", echo}, {"type", type}};
void execute_line(string& command, string& args){

	commands[command](args); // execute that 

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

		string args;
		if(command.size()==line.size()) args = "";
		else args = line.substr(command.size()+1, line.size() - command.size()-1);


		if(command == "exit") break;
		if(invalid_command(command)) cout << command << ": command not found";
		else execute_line(command, args);


		cout << "\n";

	}
}

