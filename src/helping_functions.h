#pragma once

#include <vector>
#include <string>
#include "shell.h"

using namespace std;


std::vector<std::string> split(std::string s, char delimeter=' ');

vector<string> get_args(string& command);

string get_directory(Shell& shell);

bool is_executable(string path);

string program_find_in_path(string command);

string capture_output_from_pipe(int* pipe);


void execute_program(string& path, vector<string>& args, int* pipe = NULL);


int create_file(string path, int append = 0);

bool is_executable(string path);

void enableRawMode();

void disableRawMode();

void get_all_executables(Shell& shell);

vector<string> fetch_option_flags(vector<string>& args);
