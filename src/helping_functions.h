#pragma once

#include <vector>
#include <string>
#include "shell.h"

using namespace std;


std::vector<std::string> split(std::string s, char delimeter=' ', int count=-1);

bool is_digits(string& s);

string normalise_command_line(string& line);

vector<string> get_args(Shell& shell, string& command);

string get_directory(Shell& shell);

bool is_executable(string path);

string program_find_in_path(Shell& shell, string command);

string capture_output_from_pipe(int* pipe);


pid_t execute_program(Shell& shell, string& path, vector<string>& args, int* pipe_out=NULL, int* pipe_in =NULL, bool background=false);

void reap_finished_jobs(Shell &shell, bool output=true);

int create_file(string path, int append = 0);

bool is_executable(string path);

void enableRawMode();

void disableRawMode();

void get_all_executables(Shell& shell);

vector<string> fetch_option_flags(vector<string>& args);

void read_history(Shell& shell, string path="");

string env_or_default(const char* name, const string& def = "");

bool valid_variable_name(string& s);

bool is_command_complete(Shell& shell);
