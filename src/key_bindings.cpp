#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <filesystem> // directories, navigation, opening files and programs.

#include "shell.h"
#include "helping_functions.h"

using namespace std;

bool prefix_match(vector<string> words){
    for(int i=0; i<words.size(); i++){
        if(words[i].back()=='/') words[i].pop_back(); // remove '/' for directories.
    }
    for(int i=1; i<words.size(); i++){
        if(words[i].compare(0, words[i-1].size(), words[i-1]) != 0) return false;
    }
    return true;
}


bool check_with_complete(Shell& shell){
    string line = shell.line;
    while(!line.empty() && line.back() == ' ') line.pop_back(); 

    for(auto& i: shell.tab_completions){
        if(i.first == line){
            // execute whatever it is.
            string option = i.second.first;
            string command = i.second.second;

            if(option == "-C"){
                string path;
                if(command[0] != '/') path = "./" + command;
                else path = command;
                vector<string> args; // empty for now.
                int pipefd[2];
                pipe(pipefd);

                execute_program(path, args, pipefd); // capture the data in pipe.
                close(pipefd[1]);

                string output = capture_output_from_pipe(pipefd); // read from pipe.
                close(pipefd[0]);
                
                for(char c:output){
                    if(c!='\n') shell.line += c; // all lines are concatenated as a completion candidate.
                }
                shell.line += " ";

                shell.leftright_ptr = shell.line.size();
                cout << '\r' << "$ " << shell.line;
            }
            return 1;
        }
    }
    return 0;
}


void onTab(Shell& shell, int& tab_count){
    // first check for complete builtin ho.
    if(check_with_complete(shell)) return;



    vector<string> completes; // stores the valid answers.
    string prefix;
    vector<string> tokens = split(shell.line, ' ');

    if(tokens.size()==1){ // find the executable.
        prefix = tokens[0];
        auto it = std::lower_bound(shell.all_executables.begin(), shell.all_executables.end(), prefix);
        while (it != shell.all_executables.end()) {
            if (prefix.empty() || it->compare(0, prefix.size(), prefix) == 0) {
                completes.push_back(*it);
                ++it;
            } else {
                break;
            }
        }
    }else{ // find the files and directories.
        string dir = tokens.back();
        prefix = split(dir, '/').back();
        dir.erase(dir.end() - prefix.size(), dir.end());

        if (dir.empty()) dir = ".";

        for (const auto& entry : filesystem::directory_iterator(dir)) {
            string name = entry.path().filename().string();
            if(entry.is_directory()) name += "/"; // add '/' to directories.
            if (name.compare(0, prefix.size(), prefix) == 0){
                completes.push_back(name);
            }
        }
        sort(completes.begin(), completes.end());
    }

    if(completes.empty()) {
        cout << "\x07";
        return;
    }

    string extra = completes[0].substr(prefix.size(), completes[0].size());

    if(completes.size() == 1){
        if(extra.back() == '/') shell.line += extra;
        else shell.line += extra + " ";
        cout << '\r' << "$ "  << shell.line;
        shell.leftright_ptr = shell.line.size();
        tab_count = 0;
    }
    else if(prefix_match(completes)) {
        shell.line += extra;
        if(extra.back() == '/') shell.line.pop_back();
        cout << '\r' << "$ " << shell.line;
        shell.leftright_ptr = shell.line.size();
        tab_count = 0;
    }
    else {
        if(tab_count == 0) {
            cout << "\x07";
            tab_count++;
        } else {
            cout << "\n";
            for(const string& s : completes) cout << s << "  ";
            cout << "\b\b\n$ " << shell.line;
            tab_count = 0;
        }
    }
}


void onUp(Shell& shell){
    cout << "\r" << "\033[K"; // clear line.

    if (shell.history.empty()) {
        shell.line = "";
        shell.leftright_ptr = 0;
    } else if (shell.updown_ptr > 0) {
        shell.line = shell.history[--shell.updown_ptr];
        shell.leftright_ptr = shell.line.size();
    } else {
        shell.line = shell.history[0];
        shell.leftright_ptr = shell.line.size();
    }
    cout << "$ " << shell.line;
}


void onDown(Shell& shell){
    cout << "\r" << "\033[K"; // clear line.

    if (shell.history.empty()) {
        shell.updown_ptr = 0;
        shell.line = "";
        shell.leftright_ptr = 0;
    } else if (shell.updown_ptr < (int)shell.history.size() - 1) {
        shell.line = shell.history[++shell.updown_ptr];
        shell.leftright_ptr = shell.line.size();
    } else {
        shell.updown_ptr = shell.history.size();
        shell.line = "";
        shell.leftright_ptr = 0;
    }
    cout << "$ " << shell.line;
}


void onLeft(Shell& shell){
    if(shell.leftright_ptr > 0){
        shell.leftright_ptr --;
        cout << "\033[D";
    }
}


void onRight(Shell& shell){
    if(shell.leftright_ptr != shell.line.size()) {
        shell.leftright_ptr ++;
        cout << "\033[C";
    }
}


void onBackspace(Shell& shell){
    if(shell.leftright_ptr){
        cout << "\b \b";
        shell.leftright_ptr --;
        shell.line.erase(shell.line.begin() + shell.leftright_ptr);
    }
}
