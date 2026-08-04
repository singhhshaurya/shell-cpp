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


vector<string> split(string s, char delimeter=' ', int count=-1){
	vector<string> ans;
	int ptr = 0; 

	for(int i=0; i<s.size(); i++){
		if(s[i]==delimeter){
			ans.push_back(s.substr(ptr, i-ptr));
			ptr = i+1;
			count --;
		}	
		if(count == 0){
			ans.push_back(s.substr(ptr, s.size()-ptr));
			return ans;
		}
	}
	ans.push_back(s.substr(ptr, s.size()-ptr));
	for(int i=0; i<count; i++) ans.push_back("");
	return ans;
}


bool is_digits(string& s){
	for(char c:s){
		if(!isdigit(c)) return 0;
	}
	return 1;
}


bool valid_variable_name(string& s){
	if(!(isalpha(s[0]) || s[0] == '_')) return false;

	for(int i=1; i<s.size(); i++){
		if(!(isalpha(s[i]) || isdigit(s[i]) || s[i]=='_')) return false;
	}
	return true;

}
string normalise_command_line(string& line){
	string output;
	int ptr = 0;
	while(ptr < line.size()){
		if(line[ptr] == ' '){
			output.push_back(' ');
			while(ptr < line.size() && line[ptr] == ' ') ptr ++;
		}
		else{
			output.push_back(line[ptr]);
			ptr++;
		}
	}
	while(output.back() == ' ') output.pop_back();

	return output;
}


void exchange_variables(Shell& shell, string& arg){
	string var_name;
	int variable = 0;
	int bracket = 0;
	arg += " ";
	int ptr = 0;

	int i = 0;
	while(i < arg.size()){
		char c = arg[i];
		if(variable && (c == '$' || c == ' ' || (bracket && c == '}' ))){
			string value;
			if(shell.variables.find(var_name) != shell.variables.end()) value = shell.variables[var_name];
			else value = "";

			arg = arg.substr(0, ptr) + value + arg.substr(i+bracket, arg.size()-i-bracket);
			i += value.size() - (i-ptr);
			variable = 0;
			bracket = 0;
			var_name.clear();
		}
		if(c == '$') {
			ptr = i;
			variable = 1;
		}
		else if(c == '{') {
			bracket = 1;
		}
		else if(variable) var_name += c;
		i++;
	}
	if(arg.back() == ' ') arg.pop_back();
}


bool is_command_complete(Shell& shell){
	string command = shell.prev_line+shell.line;
	bool single_quotes_closed = 1;
	bool double_quotes_closed = 1;
	bool back_slash = 0;
	for(char c:command){
		if(back_slash) back_slash = 0;
		// backslashes
		else if(c == '\\' && single_quotes_closed) back_slash = 1;
		
		// quotation marks
		else if(c=='\'' && double_quotes_closed) single_quotes_closed = !single_quotes_closed;
		else if(c=='\"' && single_quotes_closed) double_quotes_closed = !double_quotes_closed;
	}
	return single_quotes_closed && double_quotes_closed;
}

vector<string> get_args(Shell& shell, string& command){
	bool single_quotes_closed = 1;
	bool double_quotes_closed = 1;
	bool back_slash = 0;
	bool arrow = 0;
	bool variable = 0;
	string variable_name = "";

	vector<string> args;
	command.push_back(' ');

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
			// finding variables and replacing them.
			exchange_variables(shell, curr);

			if(curr != "") args.push_back(curr);
			curr = "";
		}
		else curr += c;

		if(c == '$') variable = 1;

	}

	return args;
}


vector<string> fetch_option_flags(vector<string>& args){
    vector<string> option_flags;
    for(string s:args){
        if(s[0] == '-') option_flags.push_back(s);
    }
    return option_flags;
}


string capture_output_from_pipe(int* pipe){
    string output;
    char buf[1024];
    ssize_t n;

    while ((n = read(pipe[0], buf, sizeof(buf))) > 0) {
        output.append(buf, n);
    }
    return output;
}



string env_or_default(const char* name, const string& def = ""){
    const char* p = getenv(name);
    return p ? p : def;
}


bool is_executable(string path){
    return access(path.c_str(), X_OK) == 0;
}


string get_directory(Shell& shell){
	if(shell.curr_directory == "" or shell.curr_directory == "/") return shell.variables["HOME"];
	return shell.curr_directory;
}


string program_find_in_path(Shell& shell, string command){
	string PATH = shell.variables["PATH"]; // gets path from environment.
	// cout << PATH << "\n";

	for(string dir:split(PATH, ':')){
		path candidate = path(dir) / command;

		if (exists(candidate) &&
			is_regular_file(candidate) &&
			is_executable(candidate)) {
			return candidate.string();
		}
	}
	return "";
}


pid_t execute_program(Shell& shell, string& path, vector<string>& args, int* pipe_out=NULL, int* pipe_in =NULL, bool background=false){
    string program = split(path, '/').back();
	vector<char*> argv = {program.data()};

	string output_path;
	int output = 0;

	string error_path;
	int error = 0;
	int fd;

	for(int i=0; i<args.size(); i++){
		if(args[i] == ">" || args[i] == "1>" || args[i] == "1>>" || args[i] == ">>") {
			if(output){
				create_file(output_path, output-1);
				output_path = "";
			}
			output = 1;
			error = 0;
			if(args[i] == "1>>" || args[i] == ">>") output = 2;
			continue;
		}
		if(args[i] == "2>" || args[i] == "2>>"){
			if(error){
				create_file(error_path, error-1);
				error_path = "";
			}
			error = 1;
			output = 0;
			if(args[i] == "2>>") error = 2;
			continue;
		}

		if(output) output_path += args[i];
		else if(error) error_path += args[i];
		else argv.push_back(args[i].data());
	}
	argv.push_back(nullptr);

	// executing the program.
	pid_t pid = -1;

	if(path!="" && is_executable(path)) {
		pid = fork();
		if (pid < 0) {
			std::cerr << "fork() failed\n";     // fork failed

		} else if (pid == 0) {
			// child process

			if(pipe_in != NULL){
				close(pipe_in[1]);                    // Close write end, read karna isliye.
                dup2(pipe_in[0], STDIN_FILENO);      // stdin <- pipe
                close(pipe_in[0]);
			}
            if(pipe_out != NULL){
                close(pipe_out[0]);                    // Close read end
                dup2(pipe_out[1], STDOUT_FILENO);      // stdout -> pipe
                close(pipe_out[1]);
            }
			else if(output){ // put stout to file not termninal.
				output --;
				int fd = create_file(output_path, output); // append me output = 2 tha. it will handle both > and >> .
				dup2(fd, STDOUT_FILENO); // STDOUT_FILENO is basically 1.
				close(fd);
			}

			if(error){
				error --;
				// int fd = open(error_path.data(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
				int fd = create_file(error_path, error);
				dup2(fd, STDERR_FILENO);
				close(fd);
			}

			execv(path.data(), argv.data());

			perror("execv"); // execv() failed
			exit(1); // exit child process with error code
			// child process doesnt go out of this.

		} else {
			// parent processrun
			if(background) {
				string command = normalise_command_line(shell.history.back());
				shell.background_jobs.push_back(Job{(int)shell.background_jobs.size()+1, pid, "Running", command}); // add the background job.
			}
			else waitpid(pid, nullptr, 0); // nullptr means we don't care about the exit status of the child process. 
										   //waitpid() will block until the child process terminates.
										   // 0 means we want to wait for the child process to terminate normally (not due to a signal).
		}

	}
    return pid;

}


void reap_finished_jobs(Shell &shell, bool output=true) {
    int status;
    pid_t pid;
	Job curr_job;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		for(int i=0; i<shell.background_jobs.size(); i++){
			if(shell.background_jobs[i].process_id == pid){
				if(!output){
					shell.background_jobs[i].status = "Done";
					break;
				}
				
				curr_job = shell.background_jobs[i];
				cout << "[" << curr_job.job_no << "]"; 

				if(i == shell.background_jobs.size()-1) cout << "+";
				else if(i == shell.background_jobs.size()-2) cout << "-";
				else cout << " ";

				cout << "  Done                    ";
				cout << curr_job.command << "\b\b  \b\b\n";
				shell.background_jobs.erase(shell.background_jobs.begin() + i);
				break;

			}
		}
    }
}


void add_executables(Shell& shell, string path){
    try {
        for (const auto& entry : filesystem::directory_iterator(path)) {

            if (!is_executable(entry.path()))
                continue;

            string name = entry.path().filename().string();
            auto it = lower_bound(shell.all_executables.begin(), shell.all_executables.end(), name);
            if (it == shell.all_executables.end() || *it != name) {
                shell.all_executables.insert(it, name);
            }
        }
    }
    catch (const filesystem::filesystem_error&){};
}


void read_history(Shell& shell, string path = ""){

	if(path.empty()) {
		path = shell.variables["HISTFILE"];
	}
	ifstream fin(path);
	string command;
	while(getline(fin, command)){
		shell.history.push_back(command);
	}
	shell.updown_ptr = shell.history.size();
}



// run this only once in beginning. 
void get_all_executables(Shell& shell){ 
    
    for(string s:shell.builtins) shell.all_executables.push_back(s);

    string PATH = shell.variables["PATH"]; // gets path from environment.
    string name;
    
    for(string dir:split(PATH, ':')){
        if (dir.empty())
            continue;
        try{
            for (const auto& entry : filesystem::directory_iterator(dir)) {

                if (!is_executable(entry.path()))
                    continue;

                name = entry.path().filename().string();

                // Avoid duplicates from multiple PATH directories
                shell.all_executables.push_back(name);
            }
        }catch (const filesystem::filesystem_error&) {
            // Ignore invalid or inaccessible PATH entries
        }
    }
    sort(shell.all_executables.begin(), shell.all_executables.end());
    auto it = unique(shell.all_executables.begin(), shell.all_executables.end());
    shell.all_executables.erase(it, shell.all_executables.end());

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
