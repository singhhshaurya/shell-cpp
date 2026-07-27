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

#include <termios.h>
#include "shell.h"

using namespace std;
using namespace filesystem;


int create_file(string path, int append = 0){
    int fd;
    if(append){
        fd = open(path.data(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    }else{
        fd = open(path.data(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    return fd;
}

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
	bool arrow = 0;
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
		else if (c == '>'){
			if(arrow){
				args.back()+='>';
				arrow = 0;
                continue;
			}

			if(curr == "1" || curr == "2") args.push_back(curr+">");
			else{
				if(curr!="") args.push_back(curr);
				args.push_back(">");
			}
			curr = "";
			arrow = 1;
		}
		else if(c == ' ' && single_quotes_closed && double_quotes_closed){
			if(curr != "") args.push_back(curr);
			curr = "";
		}


		else curr += c;

	}
	args.push_back(curr);
	return args;
}

bool is_executable(string path){
	return access(path.c_str(), X_OK) == 0;
}


termios orig; // this will hold the original terminal attributes so that we can restore them later.

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig); // get the current terminal attributes and store them in orig.

    termios raw = orig;
    raw.c_lflag &= ~(ICANON | ECHO); // disable canonical mode (ICANON) and echoing (ECHO). 
                                     //This means input will be available immediately (not line-buffered) and will not be echoed to the terminal.

                                     
    raw.c_cc[VMIN] = 1; // Set the minimum number of bytes that must be available before read() returns. 
                       // In this case, it is set to 1, meaning read() will return as soon as at least one byte is available. 
    raw.c_cc[VTIME] = 0; // Set the timeout value for read() in deciseconds. 
                       // In this case, it is set to 0, meaning read() will wait indefinitely for input.

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // set the terminal attributes to the new raw mode settings. 
                                              // TCSAFLUSH means to apply the changes after flushing the input and output buffers. 

}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig); // restore the original terminal attributes when raw mode is disabled.
}

void get_all_executables(Shell& shell){

    for(string s:shell.builtins) shell.all_executables.push_back(s);
    
    string PATH = getenv("PATH"); // gets path from environment.
    unordered_set<string> seen;

    for(string dir:split(PATH, ':')){
        if (dir.empty())
            continue;

        try {
            for (const auto& entry : filesystem::directory_iterator(dir)) {

                if (!is_executable(entry.path()))
                    continue;

                string name = entry.path().filename().string();

                // Avoid duplicates from multiple PATH directories
                if (seen.insert(name).second)
                    shell.all_executables.push_back(name);
            }
        }
        catch (const filesystem::filesystem_error&) {
            // Ignore invalid or inaccessible PATH entries
        }
    }
}