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
#include <cstdio>


using namespace std;
using namespace filesystem;

bool getCursorPosition(int &row, int &col)
{
    termios oldt, raw;

    if (tcgetattr(STDIN_FILENO, &oldt) == -1)
        return false;

    raw = oldt;
    raw.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1)
        return false;

    // Ask terminal for cursor position
    cout << "\033[6n" << std::flush;

    char buf[32];
    int i = 0;

    while (i < (int)sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;

        if (buf[i] == 'R')
            break;

        ++i;
    }

    buf[++i] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return sscanf(buf, "\033[%d;%dR", &row, &col) == 2;
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