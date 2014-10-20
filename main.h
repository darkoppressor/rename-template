#ifndef main_h
#define main_h

#include <string>

int main(int argc,char* args[]);

void rename_file(std::string target,std::string replacement);

void replace_in_file(std::string filename,std::string target,std::string replacement);

#endif
