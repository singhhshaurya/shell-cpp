#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <unordered_set>

#include "shell.h"
using namespace std;

class TrieNode{
public:
    bool isLeaf = 0;
    unordered_map<char, TrieNode*> children;

};

TrieNode* trieHead = new TrieNode();

void createTrie(Shell& shell){
    for(string s:shell.builtins){
        TrieNode* curr = trieHead;
        for(char c:s){
            if(curr->children.find(c)==curr->children.end()){
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];

        }
        curr -> isLeaf = 1;
    }
}


void onTab(Shell& shell, int& tab_count){
    TrieNode* curr = trieHead;
    int find = 1;
    for(char c:shell.line){
        if(curr->children.find(c) == curr->children.end()){
            find = 0;
            break;
        }
        curr = curr->children[c];
    }
    vector<string> completes;
    string word;

    vector<pair<string, TrieNode*>> stack  = {{shell.line, curr}};

    if(!find) {
        cout << "empty";
        return; // do nothing at all.
    }
    while(!stack.empty()){
        word = stack.back().first;
        curr = stack.back().second;
        stack.pop_back();

        if(curr->isLeaf) completes.push_back(word);
        for(auto& i: curr->children){
            stack.push_back({word + i.first, i.second});
        }
    }
    if(completes.size() == 1){
        cout << '\r' << "$ " << completes[0] << " ";
        shell.line = completes[0] + " ";
        shell.leftright_ptr = shell.line.size();
        tab_count = 0;

    }else{
        if(tab_count == 0) tab_count ++;
        else{
            cout << "\n";
            for(string s:completes) cout << s << "        ";
            cout << "\n$ ";
            tab_count = 0;
            shell.line = "";
            shell.leftright_ptr = 0;
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
