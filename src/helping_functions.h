#pragma once

#include <vector>
#include <string>

using namespace std;
bool getCursorPosition(int &row, int &col);

std::vector<std::string> split(std::string s, char delimeter=' ');
vector<string> get_args(string& command);
