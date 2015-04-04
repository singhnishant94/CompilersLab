#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <fstream>
#include <vector>
using namespace std;

typedef vector<string> vs;

int main(){
  ifstream file;
  file.open("dataAst.txt");
  string s;
  ofstream hfile;
  hfile.open("headers.h");
  
  ofstream dfile;
  dfile.open("definitions.cpp");
  dfile<<"#include \"headers.h\""<<endl;
  dfile<<"#include <iostream>"<<endl;
  dfile<<"using namespace std;"<<endl<<endl;
  hfile<<"#include <vector>\nusing namespace std;\n\n";

  while (getline(file, s)){
    string base = s;
    
    hfile<<"class "<<base<<" : public abstract_node {"<<endl;
    hfile<<"}\n"<<endl;
    
    while (getline(file, s)){
      if (s[0] == '0') break;
      string cls;
      char* ptr = strtok((char*)s.c_str(), " ");
      cls = (string)ptr;
      vs temp;
      ptr = strtok(NULL, " ");
      while (ptr != NULL){
	temp.push_back((string)ptr);
	ptr = strtok(NULL, " ");
      }
      
      int l = temp.size();
      hfile<<"class "<<cls<<" : public "<<base<<" {"<<endl;
      int count = 1;
      hfile<<"\t";
      hfile<<"protected:"<<endl;
      for(int i = 0; i < l; i++){
	hfile<<"\t";     hfile<<"\t";
	hfile<<temp[i]<<"* node"<<count<<";"<<endl;
	count++;
      }
      
      hfile<<"\n\t";
      hfile<<"public:"<<endl;
      hfile<<"\t";
      
      hfile<<cls<<"(";
      count = 1;
      for(int i = 0; i < l - 1; i++){
	hfile<<temp[i]<<"* node"<<count<<", ";
	count++;
      }
      
      hfile<<temp[l - 1]<<"* node"<<count<<");"<<endl;
      
      hfile<<"void print();"<<endl;

      dfile<<cls<<" :: "<<cls<<"(";
      count = 1;
      for(int i = 0; i < l - 1; i++){
	dfile<<temp[i]<<"* node"<<count<<", ";
	count++;
      }
      
      dfile<<temp[l - 1]<<"* node"<<count<<") {"<<endl;
      count = 1;
      for (int i = 0; i < l; i++){
	dfile<<"\t";
	dfile<<"this.node"<<count<<" = node"<<count<<endl;
	count++;
      }
      dfile<<"}"<<endl<<endl;
      
      dfile<<"void "<<cls<<" :: print(){\n}"<<endl<<endl;
      
      hfile<<"};\n"<<endl;
    }
  }
  
  file.close();
}
