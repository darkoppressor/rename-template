/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef main_h
#define main_h

#include <string>

int main(int argc,char* args[]);

void rename_file(std::string target,std::string replacement);

void replace_in_file(std::string filename,std::string target,std::string replacement);

#endif
