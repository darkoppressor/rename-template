/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#include "main.h"
#include "string_stuff.h"

#include <iostream>
#include <vector>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

int main(int argc,char* args[]){
    String_Stuff string_stuff;

    //Can this even happen?
    if(argc<=0){
        cout<<"Error: Did not receive the program name\n";

        return 1;
    }
    else if(argc!=3){
        string program_name=args[0];

		cout<<program_name<<" - convert a Cheese Engine template directory into a new project\n";
        cout<<"Usage: "<<program_name<<" project-directory project-name\n";
        cout<<"project-name should be 1 or 2 words, all lower case, dashed\n";
        cout<<"project-name should consist only of letters, numbers, and optionally a '-'\n";
        cout<<"project-name must not exceed 255 characters\n";

        return 0;
    }

    string project_directory=args[1];

    if(project_directory.length()==0){
        cout<<"Error: The project-directory argument has a length of 0\n";

        return 1;
    }

    if(boost::algorithm::ends_with(project_directory,"/") || boost::algorithm::ends_with(project_directory,"\\")){
        project_directory.erase(project_directory.begin()+project_directory.length()-1);
    }

    string name=args[2];

    name=name.substr(0,255);

    bool dash_detected=false;
    for(int i=0;i<name.length();i++){
        if(name[i]=='-'){
            if(!dash_detected){
                dash_detected=true;
            }
            else{
                name.erase(name.begin()+i);
                i--;
            }
        }
        else if(!((name[i]>='A' && name[i]<='Z') || (name[i]>='a' && name[i]<='z') || (name[i]>='0' && name[i]<='9'))){
            name.erase(name.begin()+i);
            i--;
        }
    }

    vector<string> name_parts;
    boost::algorithm::split(name_parts,name,boost::algorithm::is_any_of("-"));

    if(name_parts.size()<=0){
        cout<<"Error: The project-name argument resolved to 0 name parts\n";

        return 1;
    }

    bool two_part_name=false;
    if(name_parts.size()>=2){
        two_part_name=true;
    }

    string name_lower_case_dashed=string_stuff.lower_case(name_parts[0]);
    if(two_part_name){
        name_lower_case_dashed+="-"+string_stuff.lower_case(name_parts[1]);
    }

    string name_lower_case=string_stuff.lower_case(name_parts[0]);
    if(two_part_name){
        name_lower_case+=string_stuff.lower_case(name_parts[1]);
    }

    string name_caps_dashed=string_stuff.first_letter_capital(string_stuff.lower_case(name_parts[0]));
    if(two_part_name){
        name_caps_dashed+="-"+string_stuff.first_letter_capital(string_stuff.lower_case(name_parts[1]));
    }

    string name_caps_spaced=string_stuff.first_letter_capital(string_stuff.lower_case(name_parts[0]));
    if(two_part_name){
        name_caps_spaced+=" "+string_stuff.first_letter_capital(string_stuff.lower_case(name_parts[1]));
    }

    if(!boost::filesystem::is_directory(project_directory)){
        cout<<"Error: No such directory: "<<project_directory<<"\n";

        return 1;
    }

    if(!boost::algorithm::ends_with(project_directory,"template")){
        cout<<"Error: Project directory '"<<project_directory<<"' is not named 'template'\n";

        return 1;
    }

    cout<<"Removing "<<project_directory<<"/.git\n";
    boost::filesystem::remove_all(project_directory+"/.git");

    if(!replace_in_file(project_directory+"/Template","Template",name_caps_dashed)){
        return 1;
    }
    if(!rename_file(project_directory+"/Template",project_directory+"/"+name_caps_dashed)){
        return 1;
    }

    if(!replace_in_file(project_directory+"/project.cbp","template",name_lower_case_dashed)){
        return 1;
    }
    if(!replace_in_file(project_directory+"/project.cbp","Template",name_caps_dashed)){
        return 1;
    }

    if(!replace_in_file(project_directory+"/data/engine","template",name_lower_case_dashed)){
        return 1;
    }

    if(!replace_in_file(project_directory+"/development/Template.app/Contents/Info.plist","Template",name_caps_spaced)){
        return 1;
    }
    if(!replace_in_file(project_directory+"/development/Template.app/Contents/Info.plist","template",name_lower_case_dashed)){
        return 1;
    }
    if(!rename_file(project_directory+"/development/Template.app",project_directory+"/development/"+name_caps_spaced+".app")){
        return 1;
    }

    if(!replace_in_file(project_directory+"/development/android/src/org/cheeseandbacon/template/Game.java","template",name_lower_case)){
        return 1;
    }
    if(!rename_file(project_directory+"/development/android/src/org/cheeseandbacon/template",project_directory+"/development/android/src/org/cheeseandbacon/"+name_lower_case_dashed)){
        return 1;
    }

    if(!replace_in_file(project_directory+"/development/android/AndroidManifest.xml","template",name_lower_case)){
        return 1;
    }

    if(!replace_in_file(project_directory+"/development/android/build.xml","Template",name_caps_dashed)){
        return 1;
    }

    if(!replace_in_file(project_directory+"/development/android/res/values/strings.xml","Template",name_caps_spaced)){
        return 1;
    }

    string git_init="git init "+project_directory+"/";
    cout<<"Performing "<<git_init<<"\n";
    system(git_init.c_str());

    string renamed_project_directory=project_directory;
    boost::algorithm::replace_last(renamed_project_directory,"template",name_lower_case_dashed);

    if(!rename_file(project_directory,renamed_project_directory)){
        return 1;
    }

    return 0;
}

bool rename_file(string target,string replacement){
    if(!boost::filesystem::exists(target)){
        cout<<"Error: No such file: "<<target<<"\n";

        return false;
    }

    cout<<"Renaming "<<target<<" to "<<replacement<<"\n";

    boost::filesystem::rename(target,replacement);

    return true;
}

bool replace_in_file(string filename,string target,string replacement){
    if(!boost::filesystem::exists(filename)){
        cout<<"Error: No such file: "<<filename<<"\n";

        return false;
    }

    cout<<"Renaming all occurrences of "<<target<<" to "<<replacement<<" in "<<filename<<"\n";

    vector<string> file_data;

    ifstream file(filename.c_str());

    if(file.is_open()){
        while(!file.eof()){
            string line="";

            getline(file,line);

            file_data.push_back(line);
        }
    }
    else{
        cout<<"Error: Failed to open "+filename+" for updating (input phase)\n";

        file.close();
        file.clear();

        return false;
    }

    file.close();
    file.clear();

    for(int i=0;i<file_data.size();i++){
        boost::algorithm::replace_all(file_data[i],target,replacement);
    }

    ofstream file_save(filename.c_str());

    if(file_save.is_open()){
        for(int i=0;i<file_data.size();i++){
            file_save<<file_data[i];

            if(i<file_data.size()-1){
                file_save<<"\n";
            }
        }
    }
    else{
        cout<<"Error: Failed to open "+filename+" for updating (output phase)\n";

        file_save.close();
        file_save.clear();

        return false;
    }

    file_save.close();
    file_save.clear();

    return true;
}
