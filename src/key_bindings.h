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

void createTrie(Shell& shell);

void onUp(Shell& shell);

void onDown(Shell& shell);

void onLeft(Shell& shell);

void onRight(Shell& shell);

void onTab(Shell& shell, int& tab_count);

void onBackspace(Shell& shell);


