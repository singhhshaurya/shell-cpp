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


using namespace std;
class Shell {
public:
    string line;
    int updown_ptr = 0;
	int leftright_ptr = 0;
    unordered_set<string> builtins;
	filesystem::path curr_directory  = getcwd(nullptr, 0); // posix function to get current directory. use chdir() to change it. 
	unordered_map<string, function<void(vector<string>&)>> commands;
    vector<string> all_executables; // kept sorted.

    // for keeping history.
    vector<string> history;
    unordered_map<string, pair<string, string>> tab_completions;
	char backspace = '\b';
};

