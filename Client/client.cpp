#include "Menu.hpp"
#include "Sign.cpp"
#include "../Server/wrap.hpp"
#include "../Classes/TcpSocket.hpp"
#include "../Classes/UserCommand.hpp"
#include "../Server/option.hpp"
 
 extern TcpSocket mysocket;
 UserCommand Curcommand;

void Log_out();//注销函数
int FriendManage();//好友管理函数
int GroupManage();//群聊管理函数
int FriendList();//展示好友列表
int Add_Friend();//添加好友
int Delete_Friend();//删除好友
int AgreeAddFriend();//同意好友申请
int RefuseAddFriend();//拒绝好友申请
int Block_Friend();//屏蔽好友
int Restore_Friend();//恢复好友会话
int View_OnlineStatus();//查看好友在线状态
int ChatWithFriend();//聊天



int main()
{
    //UserCommand Curcommand;

    mysocket.ConnectToHost("127.0.0.1", 9999);

    int ret = Login();
    if (ret == 1)
    {
        while (1)
        {
            Func_menu();

            int option; // 选项

            cout << "请输入您的选项：" << endl;
            cin >> option;

            if (cin.eof()) // 检查是否到达文件结尾即有ctrl+d信号的出现
            {
                cout << "Reached the end of the input" << endl;
                return 0;
            }

            cin.clear(); // 清除输入流的错误状态
            cin.sync();  // 清空输入缓冲区

            if (option == 1)
            {
                // 跳转到好友管理界面
                FriendManage();
            }
            else if (option == 2)
            {
                // 跳转到群聊管理界面
                GroupManage();
            }
            else if (option == 3)
            {
                // 注销账户，主要是将redis中uid集合中的该uid删除
                Log_out();
                exit(0);
            }
            else
            {
                cout << "输入错误,请重新输入:" << endl;
                continue;
            }
        }
    }
}

int FriendManage()
{
    while(1)
    {
        Friend_menu();
        int option;

        cout << "请输入您的选项：" << endl;
        cin >> option;

        if (cin.eof()) // 检查是否到达文件结尾即有ctrl+d信号的出现
        {
            cout << "Reached the end of the input" << endl;
            return 0;
        }

        cin.clear(); // 清除输入流的错误状态
        cin.sync();  // 清空输入缓冲区

        switch (option)
        {
            case 1:
                FriendList();
                break;
            case 2:
                Add_Friend();
                break;
            case 3:
                Delete_Friend();
                break;
            case 4:
                AgreeAddFriend();
                break;
            case 5:
                RefuseAddFriend();
                break;
            case 6:
                Block_Friend();
                break;
            case 7:
                Restore_Friend();
                break;
            case 8:
                View_OnlineStatus();
                break;
            case 9:
                ChatWithFriend();
                break;
            default:
                cout<<"输入错误,请重新输入："<<endl;
                continue;
        }
    }
    
    return 0;
}

int GroupManage()
{
    while(1)
    {
        Group_menu();//群聊功能菜单

        int option; // 选项

        cout << "请输入您的选项：" << endl;
        cin >> option;

        if (cin.eof()) // 检查是否到达文件结尾即有ctrl+d信号的出现
        {
            cout << "Reached the end of the input" << endl;
            return 0;
        }

        cin.clear(); // 清除输入流的错误状态
        cin.sync();  // 清空输入缓冲区

        if(option==1)
        {
            //查看群聊列表
            break;
        }else if(option==2)
        {
            //创建群聊
            break;
        }else if(option==3)
        {
            //添加群聊
            break;
        }else{
            cout<<"输入错误,请重新输入"<<endl;
        }

    }
    

}

void Log_out() // 注销功能函数
{
    UserCommand command1(Curcommand.m_uid,"",LOGOUT,{""});
    int ret=mysocket.SendMsg(command1.To_Json());
    if(ret==0||ret==-1)
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="ok")
    {
        cout<<"您已成功注销账户,感谢您的使用"<<endl;
        return;
    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}

int FriendList()
{
    UserCommand command1(Curcommand.m_uid,"",FRIENDLIST,{""});//展示好友列表
    int ret = mysocket.SendMsg(Curcommand.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    while (1)
    {
        string Friend = mysocket.RecvMsg();
        if (Friend == "end")
        {
            cout << "好友列表展示完毕" << endl;
            break;
        }
        else if (Friend == "none")
        {
            cout << "您当前还没有好友" << endl;
            return 0;
        }
        else if (Friend == "close")
        {
            cout << "服务器端已关闭" << endl;
            exit(0);
        }
        else
        {
            cout << Friend << endl;
        }
    }

    return 1;
}

int Add_Friend()
{
    string recv_uid;//接收者的uid,或者说要添加好友的uid
    string option;//发送的验证消息
    cout<<"您想添加的好友的uid为:"<<endl; 
    cin>>recv_uid;

    cout<<"您想发送的验证消息为"<<endl;
    cin.ignore();
    getline(cin,option);

    //cout<< ":"<< Curcommand.m_uid<<endl;
    UserCommand command1(Curcommand.m_uid,recv_uid,ADDFRIEND,{option});//将发送者和接收者的uid以及发送者发送的验证消息打包
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    string recv = mysocket.RecvMsg();
    if (recv == "close")
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    else if (recv == "ok")
    {
        cout << "好友添加申请已发送,等待对方通过" << endl;
        return 1;
    }
    else if (recv == "exist")
    {
        cout << "该用户已经是您的好友,无需反复添加" << endl;
        return 0;
    }
    /*else if (recv == "apply")
    {
        cout << "您的系统消息中存在对方发送的好友申请,请先回复" << endl;
    }*/
    /*else if (recv == "handled")
    {
        cout << "您已经向该用户发送过好友申请,请等待回复" << endl;
    }*/
    else if(recv=="none")
    {
        cout << "该用户不存在" << endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Delete_Friend()
{
    string deleteuid;//想删除的好友的uid

    cout<<"请输入您想删除的好友的uid:"<<endl;
    cin>>deleteuid;

    UserCommand command1(Curcommand.m_uid,"",DELETEFRIEND,{deleteuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv = mysocket.RecvMsg();
    if (recv == "close")
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    else if (recv == "ok")
    {
        cout << "您已成功删除该好友" << endl;
        return 1;
    }
    else if (recv == "none")
    {
        cout << "未找到该好友" << endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int AgreeAddFriend()
{
    string agreeuid;//同意好友申请的好友的uid
    cout<<"您想同意加好友申请的好友uid为:"<<endl;
    cin>>agreeuid;

    UserCommand command1(Curcommand.m_uid,"",AGREEADDFRIEND,{agreeuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv = mysocket.RecvMsg();
    if (recv == "close")
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    else if (recv == "ok")
    {
        cout << "已通过" << Curcommand.m_uid << "的好友申请" << endl;
        return 1;
    }else
    {
        cout << "其他错误" << endl;
        return 0;
    }
}

int RefuseAddFriend()
{
    string refuseuid;//拒绝好友申请的好友的uid
    cout<<"您想拒绝加好友申请的uid为:"<<endl;
    cin>>refuseuid;

    UserCommand command1(Curcommand.m_uid,"",REFUSEADDFRIEND,{refuseuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="ok")
    {
        cout<<"已拒绝"<<Curcommand.m_uid<<"的好友申请"<<endl;
        return 1;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Block_Friend()//屏蔽好友
{
    string blockuid;//屏蔽好友
    cout<<"您想屏蔽该好友uid为:"<<endl;
    cin>>blockuid;

    UserCommand command1(Curcommand.m_uid,"",BLOCKFRIEND,{blockuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="ok")
    {
        cout<<"您已屏蔽该好友"<<endl;
        return 1;

    }else if(recv=="none")
    {
        cout<<"未找到该好友"<<endl;
        return 0;
    }else if(recv=="handled")
    {
        system("clear");
        cout<<"您已屏蔽过该好友,无需重复操作"<<endl;
    }else
    {
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Restore_Friend()
{
    string restoreuid;//恢复好友会话
    cout<<"您想屏蔽该好友uid为:"<<endl;
    cin>>restoreuid;

    UserCommand command1(Curcommand.m_uid,"",RESTOREFRIEND,{restoreuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="ok")
    {
        cout<<"已恢复与该好友的会话"<<endl;
        return 1;
    }else if(recv=="none")
    {
        cout<<"未找到该好友"<<endl;
        return 0;
    }else if(recv=="no")
    {
        system("clear");
        cout<<"该好友未被屏蔽"<<endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int View_OnlineStatus()
{
    string viewuid;//屏蔽好友
    cout<<"您想查看在线状态的好友uid为:"<<endl;
    cin>>viewuid;

    UserCommand command1(Curcommand.m_uid,"",VIEWONLINESTATUS,{viewuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="ok")
    {
        cout<<"用户"<<Curcommand.m_uid<<"现在在线,快去找ta聊天吧"<<endl;
        return 1;
    }else if(recv=="no")
    {
        cout<<"用户"<<Curcommand.m_uid<<"现在不在线,有事请留言"<<endl;
        return 1;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int ChatWithFriend()
{

}


