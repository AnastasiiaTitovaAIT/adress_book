#include<iostream>
#include<fstream>
#include<cstdio>
#include<string>
#include<cstring>
#include<sys/types.h>
#include<dirent.h>
#include<map>
#include<sys/stat.h>
#include<vector>
#include <sstream>
#include<stdlib.h>
#include<list>
using namespace std;
struct contact {
    string lastname;
    string firstname;
    string middlename;
    string phone;
    string email;
};
class adr{
    map<string,string> listlog; //формат имясписка-имяфайла
    map<string, int> dirlog; //формат имяфайла-наличие файла в директории (1 -есть, 0 - нет)
    map<string, int> conflog;//имя файла- наличие в конфигурационном файле
    vector<contact> contactlist;
public:
    string dir;
    string namecfg;
    fstream config;
    int check_cfg();
    void check_dir();
    void exit_book();
    string if_free(string &name);
    map<string, string>::iterator if_exists(string &name);
    string new_list(vector<string> &x);
    void create_vect(fstream &x);
    void show_lists(vector<string> &x);
    void rename_list(vector<string> &x);
    void make_list_copy(vector<string> &x);
    void delete_list(vector<string> &x);
    void new_contact(vector<string> &x);
    void delete_contact(vector<string> &x);
    void show_contacts(vector<string> &x);
    void edit_contact(vector<string> &x);
    void find_contact(vector<string> &x);
    void make_contact_copy(vector<string> &x);
    void move_contact(vector<string> &x);
};
int main(int argc, char *argv [])
{
    adr book;
    book.dir = argv[1];
    if(book.check_cfg()) { return 0;};
    book.check_dir();
    string command;
    while(command.compare("exit")) { //считывание команды
        cout<<">";
        getline(cin, command);
        vector<string> words;
        stringstream ist(command);
        string tmp;
        while (ist>>tmp)
            words.push_back(tmp);
        if(!words[0].compare("new_list")) book.new_list(words);
        if(!words[0].compare("show_lists")) book.show_lists(words);
        if(!words[0].compare("rename_list")) book.rename_list(words);
        if(!words[0].compare("make_list_copy")) book.make_list_copy(words);
        if(!words[0].compare("delete_list")) book.delete_list(words);
        if(!words[0].compare("new_contact")) book.new_contact(words);
        if(!words[0].compare("delete_contact")) book.delete_contact(words);
        if(!words[0].compare("show_contacts")) book.show_contacts(words);
        if(!words[0].compare("edit_contact")) book.edit_contact(words);
        if(!words[0].compare("find_contact")) book.find_contact(words);
        if(!words[0].compare("make_contact_copy")) book.make_contact_copy(words);
        if(!words[0].compare("move_contact")) book.move_contact(words);
        words.clear();
    }
    book.exit_book();
    return 0;
}
void adr::create_vect(fstream &iff) {
    contactlist.clear();
    while(!iff.eof()) {
        contact read;
        getline(iff, read.lastname, ',');
        getline(iff, read.firstname, ',');
        getline(iff, read.middlename, ',');
        getline(iff, read.phone, ',');
        getline(iff, read.email);
        if(read.lastname.empty()) { continue;}
        contactlist.push_back(read);
    }
    return;
}
int cmp (const string &s1, const string &s2){
    unsigned int i = 0;
    while (i < s1.length() && i < s2.length()) {
        if (tolower(s1[i]) < tolower(s2[i])) {
            return -1;
        } else if (tolower(s1[i]) > tolower(s2[i])) {
            return 1;
        }
        i++;
    }
    if (s1.length() == s2.length()) {
        return 0;
    } else if (i == s1.length()) {
        return -1;
    }
    return 1;

}
string adr::if_free(string &name){
    auto it = listlog.begin();
    while(it != listlog.end()){
        if(!cmp(it->first, name)){
            cout << "Error: Contact list with the name: '"<<name<<"' already exists."<<endl;
            cout<<"Please type another name"<<endl<<">";
            getline(cin, name);
            it = listlog.begin();
            continue;
        }
        it++;
    }
    return name;
}
map<string, string>::iterator adr::if_exists(string &name){
    map<string, string>::iterator it;
    while(1){
        it = listlog.begin();
        while(cmp(it->first, name)){ it++; };
        if(it == listlog.end()){
            cout<<"Warning! Contact list with the name '"<<name<<"' doesn't exist."<<endl;
            cout<<"Please type another name"<<endl<<">";
            getline(cin, name);
        } else { break; }
    }
    return it;
}
void adr::exit_book(){
    config.open(namecfg.c_str(), std::fstream::in | std::fstream::out| std::fstream::trunc);
    auto it = listlog.begin();
    while(it!=listlog.end()){
        config<<it->first<<"-"<<it->second<<endl;
        it++;
    }
    config.close();
    return;
}
int adr::check_cfg(void) {
    namecfg = dir;
    namecfg.append("ab.cfg");
    config.open(namecfg.c_str(), std::fstream::in | std::fstream::out);
    if(!config.is_open()){
        cout<<"Error: Can't open the configuration file"<<endl;
        return 1;
    }
    string tmp;
    while(!config.eof()) {
        if(!getline(config, tmp)) { break; }
        auto found = tmp.find("-");
        pair<map<string,string>::iterator,bool> ret;
        ret = listlog.insert(pair<string,string>(tmp.substr(0,found),tmp.substr(found + 1)));
        if (ret.second==false) {
            cout << "Error: Two contact lists with the same name: '"<< ret.first->first<<"'."<<endl;
            config.close();
            return 1;
        }
        conflog.insert(pair<string, int>(tmp.substr(found + 1),1));
        dirlog.insert(pair<string, int>(tmp.substr(found + 1),0));
    }
    config.close();
    return 0;
}
void adr::check_dir(void){
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(dir.c_str());
    while ((dp = readdir(dirp)) != NULL) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") || !strcmp(dp->d_name, "ab.cfg")) {
            continue;
        }
        struct stat buf;
        string curfilen = dir;
        string tmp;
        curfilen.append(dp->d_name);
        if (lstat(curfilen.c_str(), &buf) < 0) { continue; }
        if (S_ISREG(buf.st_mode)) {
            tmp = dp->d_name;
            if(conflog.find(tmp) == conflog.end()) { //если нет ключ "имя файла"
                cout<<"File "<<tmp<<" is not used in the configuration file."<<endl;//сообщение об ошибке
                continue;
            } else {
                dirlog[tmp] = 1;
            }
        }
    }
    auto lit = listlog.begin();
    while(lit != listlog.end()) {
        if (dirlog[lit->second] == 0) {
            cout<<"There is no file for '"<<lit->first<<"' contact list"<<endl;//нет файла контактов в папке
        }
        lit++;
    }
    (void)closedir(dirp);
    return;
}
string adr::new_list(vector<string> &par) {
    string lname, fname("list");
    int i = 1;
    if (par.size() == 1) {
        cout<<"Please type the new contact list's name"<<endl<<">";
        getline(cin, lname);
    } else if (par.size() > 2) {
        cout<<"Warning! Wrong command format."<<endl;
        return NULL;
    } else {
        lname = par[1];
    }
    while(dirlog.find(fname + to_string(dirlog.size()+i)) != dirlog.end()){
        i++;
    }
    lname = if_free(lname);
    fname.append(to_string(dirlog.size()+i));
    dirlog.insert(pair<string, int>(fname,1));
    listlog.insert(pair<string, string>(lname, fname));
    conflog.insert(pair<string, int>(lname, 1));
    fstream nlist;
    nlist.open(dir + fname, std::fstream::in | std::fstream::out | std::fstream::app);
    nlist.close();
    return fname;
}
void adr::show_lists(vector<string> &par) {
    if (par.size() > 1) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    auto it = listlog.begin();
    while(it!=listlog.end()){
        cout<<it->first<<" - "<<it->second<<endl;
        it++;
    }
    return;
}
void adr::rename_list(vector<string> &par) {
    string prevname, newname;
    if (par.size() > 3) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if (par.size() == 1) {
        cout<<"Please type the name of the list to be renamed."<<endl<<">";
        getline(cin, prevname);
        cout<<"Please type the new name of the list."<<endl<<">";
        getline(cin, newname);
    } else {
        prevname = par[1];
        newname = par[2];
    }
    auto it = if_exists(prevname);
    newname = if_free(newname);
    string tmp = it->second;
    listlog.erase(it);
    listlog[newname] = tmp;
    return;
}
void adr::make_list_copy(vector<string> &par) {
    string sourcename, copyname, tmp;
    vector<string> contacts;
    if (par.size() > 3) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if (par.size() == 1) {
        cout<<"Please type the name of the source list."<<endl<<">";
        getline(cin, sourcename);
        cout<<"Please type the name of the new list."<<endl<<">";
        getline(cin, copyname);
    } else {
        sourcename = par[1];
        copyname = par[2];
    }
    auto found = if_exists(sourcename);
    vector<string> parameters = {"new_list", copyname};
    string filename = new_list(parameters);
    fstream copyfile;
    copyfile.open(dir+ filename, std::fstream::out | std::fstream::trunc);
    fstream source;
    source.open(dir + found->second, std::fstream::in);
    while(!source.eof()){
        if(!getline(source, tmp)) break;
        contacts.push_back(tmp);
    }
    auto it = contacts.begin();
    while(it!=contacts.end()) {
        copyfile<<*it<<endl;
        it++;
    }
    source.close();
    copyfile.close();
    return;
}
void adr::delete_list(vector<string> &par){
    string name;
    if (par.size() > 2) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if (par.size() == 1) {
        cout<<"Please type the name of the list to be deleted"<<endl<<">";
        getline(cin, name);
    } else {
        name = par[1];
    }
    auto found = if_exists(name);
    name = found->second;
    dirlog.erase(name);
    conflog.erase(name);
    name = dir + found->second;
    listlog.erase(found);
    remove(name.c_str());
    return;
}
void adr::new_contact(vector<string> &par){
    string listname, tmp;
    contact new_rec;
    ostringstream oss;
    if(par.size() != 7 && par.size() != 1){
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if(par.size() == 7) {
        listname = par[1];
        new_rec.lastname = par[2];
        new_rec.firstname = par[3];
        new_rec.middlename = par[4];
        new_rec.phone = par[5];
        new_rec.email = par[6];
    } else {
        cout<<"Please type the name of the contact list."<<endl<<">";
        getline(cin, listname);
        auto fit = if_exists(listname);
        listname = fit->first;
        cout<<"Please type the last name of the new contact."<<endl<<">";
        getline(cin, new_rec.lastname);
        cout<<"Please type the first name of the new contact."<<endl<<">";
        getline(cin, new_rec.firstname);
        cout<<"Please type the middle name of the new contact."<<endl<<">";
        getline(cin, new_rec.middlename);
        cout<<"Please type the phone number of the new contact."<<endl<<">";
        getline(cin, new_rec.phone);
        cout<<"Please type the e-mail of the new contact."<<endl<<">";
        getline(cin, new_rec.email);
    }
    fstream iff;
    iff.open(dir + listlog[listname], std::fstream::in | std::fstream::out);
    create_vect(iff);
    iff.close();
    auto it = contactlist.begin();
    while(it != contactlist.end()){
        if(cmp(new_rec.lastname, it->lastname) < 0 ||
!cmp(new_rec.lastname, it->lastname) && cmp(new_rec.firstname, it->firstname) < 0 ||
!cmp(new_rec.lastname, it->lastname) && !cmp(new_rec.firstname, it->firstname)
           && cmp(new_rec.middlename, it->middlename) <= 0 ){
            break;
        }
        it++;
    }
    contactlist.insert(it, new_rec);
    it = contactlist.begin();
    ofstream off;
    off.open(dir + listlog[listname], std::fstream::out | std::fstream::trunc);
    while(it != contactlist.end()){
        off<<it->lastname<<","<<it->firstname<<","<<it->middlename<<","<<it->phone<<","<<it->email<<endl;
        it++;
    }
    off.close();
    contactlist.clear();
    return;
}
void adr::show_contacts(vector<string> &par){
    string name;
    if (par.size() > 2) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if (par.size() == 1) {
        cout<<"Please type the name of the list to be shown"<<endl<<">";
        getline(cin, name);
    } else {
        name = par[1];
    }
    fstream iff;
    auto found = if_exists(name);
    iff.open(dir + found->second, std::fstream::in | std::fstream::out);
    create_vect(iff);
    iff.close();
    auto it = contactlist.begin();
    int i = 1;
    while(it != contactlist.end()) {
        cout<<i<<" "<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
        it++;
        i++;
    }
    iff.close();
    contactlist.clear();
    return;
}
void adr::delete_contact(vector<string> &par){
    string name, tmp;
    int i;
    if (par.size() > 3 && par.size() == 2) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if (par.size() == 1) {
        cout<<"Please type the name of the contact list"<<endl<<">";
        getline(cin, name);
        cout<<"Please type the number of the record to be deleted."<<endl<<">";
        getline(cin, tmp);
        i = stoi(tmp);

    } else {
        name = par[1];
        i = stoi(par[2]);
    }
    fstream ff;
    auto found = if_exists(name);
    ff.open(dir + found->second, std::fstream::in | std::fstream::out);
    create_vect(ff);
    ff.close();
    auto cit = contactlist.begin();
    cit+= i - 1;
    while (cit >= contactlist.end()){
        cout<<"Warning! Incorrect contact number, please type another number."<<endl<<">";
        getline(cin, tmp);
        i = stoi(tmp);
        cit = contactlist.begin();
        cit+= i - 1;
    }
    contactlist.erase(cit);
    ofstream off;
    off.open(dir + listlog[name], std::fstream::out | std::fstream::trunc);
    cit = contactlist.begin();
    while(cit != contactlist.end()){
        off<<cit->lastname<<","<<cit->firstname<<","<<cit->middlename<<","<<cit->phone<<","<<cit->email<<endl;
        cit++;
    }
    off.close();
    contactlist.clear();
    return;
}
void adr::edit_contact(vector<string> &par){
    string name,tmp;
    int i;
    vector<string> sh_del, for_new;
    map<string,string>::iterator found;
    if(par.size() != 8 && par.size() != 1) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if(par.size() == 1){
        cout<<"Please type the name of the contact list"<<endl<<">";
        getline(cin, name);
        found = if_exists(name);
        sh_del = {" ", found->first};
        for_new = {" ", found->first};
        show_contacts(sh_del);
        cout<<"Please type the number of the record to be edited."<<endl<<">";
        getline(cin,tmp);
        i = stoi(tmp);
        fstream iff;
        iff.open(dir + found->second, std::fstream::in | std::fstream::out);
        create_vect(iff);
        iff.close();
        auto cit = contactlist.begin();
        cit+= i - 1;
        while (cit >= contactlist.end()){
            cout<<"Warning! Incorrect contact number, please type another number."<<endl<<">";
            getline(cin, tmp);
            i = stoi(tmp);
            cit = contactlist.begin();
            cit+= i - 1;
        }
        sh_del.push_back(to_string(i));
        cout<<"Please type the last name."<<endl<<">";
        getline(cin, tmp);
        if(tmp.empty()) { for_new.push_back(cit->lastname);}
        else { for_new.push_back(tmp);}
        cout<<"Please type the first name."<<endl<<">";
        getline(cin, tmp);
        if(tmp.empty()) { for_new.push_back(cit->firstname);}
        else { for_new.push_back(tmp);}
        cout<<"Please type the middle name."<<endl<<">";
        getline(cin, tmp);
        if(tmp.empty()) { for_new.push_back(cit->middlename);}
        else { for_new.push_back(tmp);}
        cout<<"Please type the phone number."<<endl<<">";
        getline(cin, tmp);
        if(tmp.empty()) { for_new.push_back(cit->phone);}
        else { for_new.push_back(tmp);}
        cout<<"Please type the e-mail."<<endl<<">";
        getline(cin, tmp);
        if(tmp.empty()) { for_new.push_back(cit->email);}
        else { for_new.push_back(tmp);}
        contactlist.clear();
    } else {
        name = par[1];
        found = if_exists(name);
        name = found->first;
        fstream iff;
        iff.open(dir + found->second, std::fstream::in | std::fstream::out);
        create_vect(iff);
        i = stoi(par[2]);
        iff.close();
        auto cit = contactlist.begin();
        cit+= i - 1;
        while (cit >= contactlist.end()){
            cout<<"Warning! Incorrect contact number, please type another number."<<endl<<">";
            getline(cin, tmp);
            i = stoi(tmp);
            cit = contactlist.begin();
            cit+= i - 1;
        }
        for_new.push_back(name);
        if(par[3].empty()){
            for_new.push_back(cit->lastname);
        }else { for_new.push_back(par[3]);}
        if(par[4].empty()){
            for_new.push_back(cit->firstname);
        }else { for_new.push_back(par[4]);}
        if(par[5].empty()){
            for_new.push_back(cit->middlename);
        }else { for_new.push_back(par[5]);}
        if(par[6].empty()){
            for_new.push_back(cit->phone);
        }else { for_new.push_back(par[6]);}
        if(par[7].empty()){
            for_new.push_back(cit->email);
        }else { for_new.push_back(par[3]);}
        contactlist.clear();
    }
    delete_contact(sh_del);
    new_contact(for_new);
    return;
}
void adr::make_contact_copy(vector<string> &par){
    string sname, destname, tmp;
    vector<string> parameter;
    map<string,string>::iterator sf;
    int i;
    if (par.size() != 4 && par.size() != 1) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if (par.size() == 1) {
        cout<<"Please type the name of the source contact list"<<endl<<">";
        getline(cin, sname);
        sf = if_exists(sname);
        sname = sf->first;
        parameter = {" ", sname};
        show_contacts(parameter);
        cout<<"Please type the number of the record to be copied."<<endl<<">";
        getline(cin, tmp);
        i = stoi(tmp);
        cout<<"Please type the name of the destination contact list"<<endl<<">";
        getline(cin, destname);
        auto df = if_exists(destname);
        destname = df->first;
    } else {
        sname = par[1];
        i = stoi(par[2]);
        destname = par[3];
    }
    fstream ffsour;
    ffsour.open(dir + sf->second, std::fstream::in | std::fstream::out);
    create_vect(ffsour);
    ffsour.close();
    parameter.clear();
    parameter.push_back(" ");
    parameter.push_back(destname);
    parameter.push_back(contactlist[i-1].lastname);
    parameter.push_back(contactlist[i-1].firstname);
    parameter.push_back(contactlist[i-1].middlename);
    parameter.push_back(contactlist[i-1].phone);
    parameter.push_back(contactlist[i-1].email);
    new_contact(parameter);
    contactlist.clear();
    return;
}
void adr::move_contact(vector<string> &par){
    string sname, destname, tmp;
    vector<string> parameter;
    map<string,string>::iterator sf;
    int i;
    if (par.size() != 4 && par.size() != 1) {
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if (par.size() == 1) {
        cout<<"Please type the name of the source contact list"<<endl<<">";
        getline(cin, sname);
        sf = if_exists(sname);
        sname = sf->first;
        parameter = {" ", sname};
        show_contacts(parameter);
        cout<<"Please type the number of the record to be moved."<<endl<<">";
        getline(cin, tmp);
        i = stoi(tmp);
        cout<<"Please type the name of the destination contact list"<<endl<<">";
        getline(cin, destname);
        auto df = if_exists(destname);
        destname = df->first;
    } else {
        sname = par[1];
        i = stoi(par[2]);
        destname = par[3];
    }
    fstream ffsour;
    ffsour.open(dir + sf->second, std::fstream::in | std::fstream::out);
    create_vect(ffsour);
    ffsour.close();
    parameter.clear();
    parameter.push_back(" ");
    parameter.push_back(destname);
    parameter.push_back(contactlist[i-1].lastname);
    parameter.push_back(contactlist[i-1].firstname);
    parameter.push_back(contactlist[i-1].middlename);
    parameter.push_back(contactlist[i-1].phone);
    parameter.push_back(contactlist[i-1].email);
    new_contact(parameter);
    contactlist.clear();
    parameter.clear();
    parameter.push_back(" ");
    parameter.push_back(sname);
    parameter.push_back(to_string(i));
    delete_contact(parameter);
    return;
}
void adr::find_contact(vector<string> &par){
    string name, lfind, ffind, mfind;
    map<string,string>::iterator flist;
    if(par.size() != 5 && par.size() != 1){
        cout<<"Warning! Wrong command format."<<endl;
        return;
    }
    if(par.size() == 1){
        cout<<"Please type the name of the contact list"<<endl<<">";
        getline(cin, name);
        flist = if_exists(name);
        name = flist->first;
        fstream ff;
        ff.open(dir + flist->second, std::fstream::in | std::fstream::out);
        create_vect(ff);
        ff.close();
        cout<<"Please type the last name to be searched"<<endl<<">";
        getline(cin, lfind);
        cout<<"Please type the first name to be searched"<<endl<<">";
        getline(cin, ffind);
        cout<<"Please type the middle name to be searched"<<endl<<">";
        getline(cin, mfind);
    } else {
        name = par[1];
        flist = if_exists(name);
        name = flist->first;
        fstream ff;
        ff.open(dir + flist->second, std::fstream::in | std::fstream::out);
        create_vect(ff);
        ff.close();
        lfind = par[2];
        ffind = par[3];
        mfind = par[4];
    }
    auto it = contactlist.begin();
    while(it != contactlist.end()){
        if(!lfind.empty() && !ffind.empty() && !mfind.empty()){
            if(!cmp(it->lastname, lfind) && !cmp(it->firstname, ffind) && !cmp(it->middlename, mfind))
            {
                cout<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
            }
        }
        if(!lfind.empty() && !ffind.empty() && mfind.empty()){
            if(!cmp(it->lastname, lfind) && !cmp(it->firstname, ffind))
            {
                cout<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
            }
        }
        if(!lfind.empty() && ffind.empty() && mfind.empty()){
            if(!cmp(it->lastname, lfind))
            {
                cout<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
            }
        }
        if(lfind.empty() && !ffind.empty() && !mfind.empty()){
            if(!cmp(it->middlename, mfind) && !cmp(it->firstname, ffind))
            {
                cout<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
            }
        }
        if(lfind.empty() && ffind.empty() && !mfind.empty()){
            if(!cmp(it->middlename, mfind))
            {
                cout<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
            }
        }
        if(lfind.empty() && !ffind.empty() && mfind.empty()){
            if(!cmp(it->firstname, ffind))
            {
                cout<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
            }
        }
        if(!lfind.empty() && ffind.empty() && !mfind.empty()){
            if(!cmp(it->lastname, lfind) && !cmp(it->middlename, mfind))
            {
                cout<<it->lastname<<" "<<it->firstname<<" "<<it->middlename<<" "<<it->phone<<" "<<it->email<<endl;
            }
        }
        it++;
    }
    contactlist.clear();
    return;
}
