#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>

using namespace std;

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





void echo(string& s){
	cout << s;
}

unordered_map<string, function<void(string&)>> commands = {{"echo", echo}};




void execute_line(string& command, string& args){
	commands[command](args); // execute that 


}

bool invalid_command(string s){
	return commands.find(s) == commands.end();
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
		string args;

		if(command.size()==line.size()) args = "";
		else args = line.substr(command.size()+1, line.size() - command.size()-1);


		if(command == "exit") break;
		if(invalid_command(command)) cout << command << ": command not found";
		else execute_line(command, args);


		cout << "\n";




	}

	return 1;

}

