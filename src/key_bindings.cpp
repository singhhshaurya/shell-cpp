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

bool prefix_match(vector<string>& words){
    for(int i=1; i<words.size(); i++){
        if(words[i].compare(0, words[i-1].size(), words[i-1]) != 0) return false;
    }
    return true;
}

void onTab(Shell& shell, int& tab_count){
    vector<string> completes;
    string prefix = shell.line;

    auto it = std::lower_bound(shell.all_executables.begin(), shell.all_executables.end(), prefix);
    while (it != shell.all_executables.end()) {
        if (prefix.empty() || it->compare(0, prefix.size(), prefix) == 0) {
            completes.push_back(*it);
            ++it;
        } else {
            break;
        }
    }

    if(completes.empty()) {
        cout << "\x07";
        return;
    }

    if(completes.size() == 1){
        cout << '\r' << "$ " << completes[0] << " ";
        shell.line = completes[0] + " ";
        shell.leftright_ptr = shell.line.size();
        tab_count = 0;
    }
    else if(prefix_match(completes)) {
        cout << '\r' << "$ " << completes[0];
        shell.line = completes[0];
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
