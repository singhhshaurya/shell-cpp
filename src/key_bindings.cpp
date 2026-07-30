#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
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


vector<string> check_with_complete(Shell& shell){
    vector<string> tokens = split(shell.line);
    string prefix = tokens[0]; // command to check. first word ho   ga.
    vector<string> completes;


    while(!prefix.empty() && prefix.back() == ' ') prefix.pop_back(); 
    setenv("COMP_LINE", shell.line.data(), 1);  // 1 means overwrite.
    setenv("COMP_POINT", to_string(shell.line.size()).data(), 1); 

    for(auto& i: shell.tab_completions){
        if(i.first == prefix){
            // execute whatever it is.
            string option = i.second.first;
            string command = i.second.second;


            if(option == "-C"){
                string path;
                if(command[0] != '/') path = "./" + command; // make it relative.
                else path = command;

                if(tokens.size() == 1) tokens.insert(tokens.end(), {"", ""});
                else if(tokens.size() == 2){
                    if(tokens.back() == "") tokens.push_back("");
                    else tokens.push_back(tokens[0]);
                }
                else swap(tokens[1], tokens[2]);

                prefix = tokens[1]; // word to be completed.

                int pipefd[2];
                pipe(pipefd);
                int flag = execute_program(path, tokens, pipefd); // capture the data in pipe. flag = 0 if path not executable.
                close(pipefd[1]);
                if(flag == 0) return completes; // path not found bekar. return empty completes.

                string output = capture_output_from_pipe(pipefd); // read from pipe.
                close(pipefd[0]);

                // if(prefix == "") cout << "\n" << output << "\n";
                vector<string> candidates = split(output, '\n'); // return candidates in completes back to onTab function for checking.
                for(string s:candidates){
                    if (!s.empty() && s.compare(0, prefix.size(), prefix) == 0){
                        completes.push_back(s);
                    }
                }
            }
        }
    }
    sort(completes.begin(), completes.end());
    return completes;
}


void onTab(Shell& shell, int& tab_count){
    // first check for complete builtin ho.
    vector<string> tokens = split(shell.line, ' ');
    string prefix;

    vector<string> completes = check_with_complete(shell); // check completor builtin se kuch mile agar.

    if(completes.empty()){
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
            prefix = split(dir, '/').back(); // change prefix.
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
    }
    else prefix = tokens.back(); // because completion me last wala word hoga na.

    if(completes.empty()) {
        cout << "\x07";
        return;
    }

    string extra = completes[0].substr(prefix.size(), completes[0].size());

    if(completes.size() == 1){
        if(extra != "" && extra.back() == '/') shell.line += extra;
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
            for(string s : completes) cout << s << "  ";
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
