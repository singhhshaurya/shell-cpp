#include <iostream>
#include <string>

using namespace std;


bool valid_command(string s){
  return false;
}

int main() {
  // Flush after every std::cout / std:cerr
  cout << std::unitbuf;
  cerr << std::unitbuf;


  cout << "$ ";

  string a;
  cin >> a;

  if(!valid_command(a)) cout << a << ": command not found";

  cout << "\n";
  
}

