#include <iostream>
#include <string>

using namespace std;


bool valid_command(string s){
	return false;
}



int main() {
  // Flush after every std::cout / std:cerr
	cout << std::unitbuf;
	cerr << std::unitbuf;

	string line;
	
	while(true){
		cout << "$ ";

		if(!getline(cin, line)) break;
		
		if(line == "exit") break;

		if(!valid_command(line)) cout << line << ": command not found";
		cout << "\n";


	}

}

