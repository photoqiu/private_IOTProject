#ifndef _INIFILE_H
#define _INIFILE_H

#include <map>
#include <vector>
#include <string>
#include <string.h>

using namespace std;
namespace inifile
{
const int RET_OK  = 0;
const int RET_ERR = -1;
const string delim = "\n";
struct IniItem {
    string key;
    string value;
    string comment;
};
struct IniSection {
    typedef vector<IniItem>::iterator iterator;
    iterator begin() {
        return items.begin();
    }
    iterator end() {
        return items.end();
    }

    string name;
    string comment;
    vector<IniItem> items;
};

class IniFile
{
public:
    IniFile();
    ~IniFile() {
        release();
    }

public:
    typedef map<string, IniSection *>::iterator iterator;

    iterator begin() {
        return sections_.begin();
    }
    iterator end() {
        return sections_.end();
    }
public:
    /* 打开并解析一个名为fname的INI文件a*/
    int load(const string &fname);
    /*将内容保存到当前文件a*/
    int save();
    /*将内容另存到一个名为fname的文件a*/
    int saveas(const string &fname);

    /*获取section段第一个键为key的值,并返回其string型的值b*/
    string getStringValue(const string &section, const string &key, int &ret);
    /*获取section段第一个键为key的值,并返回其int型的值b*/
    int getIntValue(const string &section, const string &key, int &ret);
    /*获取section段第一个键为key的值,并返回其double型的值b*/
    double getDoubleValue(const string &section, const string &key, int &ret);

    /*获取section段第一个键为key的值,并将值赋到value中c*/
    int getValue(const string &section, const string &key, string &value);
    /*获取section段第一个键为key的值,并将值赋到value中,将注释赋到comment中c*/
    int getValue(const string &section, const string &key, string &value, string &comment);

    /*获取section段所有键为key的值,并将值赋到values的vector中d*/
    int getValues(const string &section, const string &key, vector<string> &values);
    /*获取section段所有键为key的值,并将值赋到values的vector中,,将注释赋到comments的vector中d*/
    int getValues(const string &section, const string &key, vector<string> &value, vector<string> &comments);

    bool hasSection(const string &section) ;
    bool hasKey(const string &section, const string &key) ;

    /* 获取section段的注释s*/
    int getSectionComment(const string &section, string &comment);
    /* 设置section段的注释s*/
    int setSectionComment(const string &section, const string &comment);
    /*获取注释标记符列表s*/
    void getCommentFlags(vector<string> &flags);
    /*设置注释标记符列表s*/
    void setCommentFlags(const vector<string> &flags);

    /*同时设置值和注释d*/
    int setValue(const string &section, const string &key, const string &value, const string &comment = "");
    /*删除段f*/
    void deleteSection(const string &section);
    /*删除特定段的特定参数f*/
    void deleteKey(const string &section, const string &key);
public:
    /*去掉str后面的c字符f*/
    static void trimleft(string &str, char c = ' ');
    /*去掉str前面的c字符f*/
    static void trimright(string &str, char c = ' ');
    /*去掉str前面和后面的空格符,Tab符等空白符f*/
    static void trim(string &str);
    /*将字符串str按分割符delim分割成多个子串f*/
private:
    IniSection *getSection(const string &section = "");
    void release();
    int getline(string &str, FILE *fp);
    bool isComment(const string &str);
    bool parse(const string &content, string &key, string &value, char c = '=');
    void print();

private:
    map<string, IniSection *> sections_;
    string fname_;
    vector<string> flags_;
};
}
#endif