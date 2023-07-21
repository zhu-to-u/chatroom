/*处理命令的属性和序列化（将对象转换为 JSON 字符串）*/

#include<iostream>
#include<string>
#include<vector>
#include<nlohmann/json.hpp>

using namespace std;
using json=nlohmann::json;

struct UserCommand{
public:
    UserCommand()=default;
    ~UserCommand()=default;
    UserCommand(string uid,string nickname,int flag,vector<string> option)
    {
        string m_uid=uid;
        string m_nickname=nickname;
        int m_flag=flag;
        vector<string> m_option=option;

    }

    string m_uid;//用户ID，默认为空字符串
    int m_flag=0;//用户发送操作者的类别
    vector<string> m_option;//命令的具体内容，动态数组

    void From_Json(string js)//从Json字符串中解析数据并填充到UserCommand结构体中
    {
        json jn=json::parse(js);
        jn.at("uid").get_to(m_uid);
        jn.at("flag").get_to(m_flag);
        jn.at("option").get_to(m_option);
    }

    string  To_Json()
    {
        json jn=json{
            {"uid",m_uid},
            {"flag",m_flag},
            {"option",m_option},
        };

        return jn.dump();//json格式转为json字符串格式
    }
};