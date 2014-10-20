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
    else if(argc==1){
        string program_name=args[0];
        cout<<"Usage: "<<program_name<<" project-name\n";
        cout<<"project-name should be 1 or 2 words, all lower case, dashed\n";
        cout<<"project-name should consist only of letters, numbers, and optionally a '-'\n";
        cout<<"project-name must not exceed 255 characters\n";

        return 0;
    }

    string name=args[1];

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
        cout<<"Error: The argument resolved to 0 name parts\n";

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

    boost::filesystem::remove_all("../.hg");

    replace_in_file("../Template","Template",name_caps_dashed);
    rename_file("../Template","../"+name_caps_dashed);

    replace_in_file("../project.cbp","template",name_lower_case_dashed);
    replace_in_file("../project.cbp","Template",name_caps_dashed);

    replace_in_file("../data/engine","template",name_lower_case_dashed);

    replace_in_file("Template.app/Contents/Info.plist","Template",name_caps_spaced);
    replace_in_file("Template.app/Contents/Info.plist","template",name_lower_case_dashed);
    rename_file("Template.app",name_caps_spaced+".app");

    replace_in_file("android/src/org/cheeseandbacon/template/Game.java","template",name_lower_case);
    rename_file("android/src/org/cheeseandbacon/template","android/src/org/cheeseandbacon/"+name_lower_case_dashed);

    replace_in_file("android/AndroidManifest.xml","template",name_lower_case);

    replace_in_file("android/build.xml","Template",name_caps_dashed);

    replace_in_file("android/res/values/strings.xml","Template",name_caps_spaced);

    system("hg init ../");

    return 0;
}

void rename_file(string target,string replacement){
    boost::filesystem::rename(target,replacement);
}

void replace_in_file(string filename,string target,string replacement){
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
    }

    file.close();
    file.clear();

    for(int i=0;i<file_data.size();i++){
        boost::algorithm::replace_first(file_data[i],target,replacement);
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
    }

    file_save.close();
    file_save.clear();
}
