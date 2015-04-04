#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <fstream>
using namespace std;

int main(){
  ifstream file;
  file.open("grammar");
  string s;
  while (getline(file, s)){
    cout<<s<<endl;
    while (getline(file, s)){
      if (s == "") break;
    }
  }
}
