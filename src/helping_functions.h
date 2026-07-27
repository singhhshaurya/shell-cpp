#pragma once

#include <vector>
#include <string>
#include "shell.h"

using namespace std;

bool getCursorPosition(int &row, int &col);

std::vector<std::string> split(std::string s, char delimeter=' ');

vector<string> get_args(string& command);

int create_file(string path, int append = 0);

bool is_executable(string path);

void enableRawMode();

void disableRawMode();

void get_all_executables(Shell& shell);

