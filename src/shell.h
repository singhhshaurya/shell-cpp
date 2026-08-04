#pragma once // this line ensures that the header file is included only once in a single compilation.

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <set>
#include <filesystem> // directories, navigation, opening files and programs.
#include <unistd.h> // standard posix functions.
#include <fstream>


using namespace std;

struct Job{
    int job_no;
    pid_t process_id;
    string status;
    string command;
};



class Shell {
public:
    string prev_line;
    string line;
    int updown_ptr = 0;
	int leftright_ptr = 0;
    bool incomplete_command = 0;
    bool terminate_shell = 0;
    unordered_set<string> builtins;
	filesystem::path curr_directory  = getcwd(nullptr, 0); // posix function to get current directory. use chdir() to change it. 
	unordered_map<string, function<void(vector<string>&)>> commands;
    vector<string> all_executables; // kept sorted.

    unordered_map<string, string> variables = {{"PWD", curr_directory},{"OLDPWD", ""}, 
                                               {"HISTFILE", getenv("HISTFILE") ? getenv("HISTFILE") : "src/history"}};
    
    unordered_set<string> exported_vars = {"PATH", "HOME", "LOGNAME", "USER", "LANG"}; // added in main.

    // for keeping history.
    // const string HISTORY_PATH = "src/history";
    vector<string> history;
    int history_last_appended = 0;
    unordered_map<string, pair<string, string>> tab_completions;
    unordered_map<string, string> aliases;
    vector<Job> background_jobs; // {job_id and process_id}
	char backspace = '\b';
    string remove_line = "\033[A";
};

