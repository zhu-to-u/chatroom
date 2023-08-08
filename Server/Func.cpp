//#include "../Classes/TcpSocket.hpp"
#include "../Classes/TcpSocket.cpp"
#include "../Classes/UserCommand.hpp"
#include "option.hpp"
#include "wrap.hpp"
#include "../Classes/Redis.hpp"
#include<hiredis/hiredis.h>
#include<string>
#include<random>
#include<fcntl.h>
#include<sys/epoll.h>
#include <csignal>
#include <unordered_set>

using namespace std;
Redis redis;
//extern TcpSocket mysocket;
extern UserCommand Curcommand;

struct Argc_func {
public:
    Argc_func(TcpSocket mysocket, string command_string)
        : mysocket(mysocket), command_string(command_string){}

    TcpSocket mysocket;
    string command_string;
};

void Sign_up(TcpSocket mysocket,UserCommand command);//注册
void Log_in(TcpSocket mysocket,UserCommand command);//登陆
void Log_out(TcpSocket mysocket,UserCommand command);//注销
void FriendList(TcpSocket mysocket,UserCommand command);//展示好友列表
void Add_Friend(TcpSocket mysocket,UserCommand command);//添加好友
void Delete_Friend(TcpSocket mysocket,UserCommand command);//删除好友
void AgreeAddFriend(TcpSocket mysocket,UserCommand command);//同意好友请求
void RefuseAddFriend(TcpSocket mysocket,UserCommand command);//拒绝好友请求
void Block_Friend(TcpSocket mysocket,UserCommand command);//屏蔽好友
void Restore_Friend(TcpSocket mysocket,UserCommand command);//恢复会话
void ViewOnlineStatus(TcpSocket mysocket,UserCommand command);//查看好友在线状态
void UnreadMessage(TcpSocket mysocket,UserCommand command);//未读消息
void ChatWithFriend(TcpSocket mysocket,UserCommand command);//私聊请求
void FriendSendMsg(TcpSocket mysocket,UserCommand command);//发送消息
void ExitChat(TcpSocket mysocket,UserCommand command);//停止聊天
void AddGroup(TcpSocket mysocket,UserCommand command);//加群
void CreateGroup(TcpSocket mysocket,UserCommand command);//创建群聊
void GroupList(TcpSocket mysocket,UserCommand command);//群聊列表
void MemberList(TcpSocket mysocket,UserCommand command);//成员列表
void DeleteGroup(TcpSocket mysocket,UserCommand command);//退出群聊
void DeleteMember(TcpSocket mysocket,UserCommand command);//删除成员
void AddManager(TcpSocket mysocket,UserCommand command);//添加管理员
void DeleteManager(TcpSocket mysocket,UserCommand command);//删除管理员
void DissolveGroup(TcpSocket mysocket,UserCommand command);//解散群聊
void ApplyList(TcpSocket mysocket,UserCommand command);//申请加群列表
void AgreeAddMember(TcpSocket mysocket,UserCommand command);//同意加群
void RefuseAddMember(TcpSocket mysocket,UserCommand command);//拒绝加群


void task(void *arg)
{
    Argc_func *argc_func = static_cast<Argc_func*>(arg);
    UserCommand command;     // Command类存客户端的命令内容
    TcpSocket mysocket = argc_func->mysocket;      // TcpSocket类用于通信
    command.From_Json(argc_func->command_string); 

    switch(command.m_flag){
        case SIGNUP:
            Sign_up(mysocket,command);
            break;
        case LOGIN:
            Log_in(mysocket,command);
            break;
        case LOGOUT:
            Log_out(mysocket,command);
            break;
        case FRIENDLIST:
            FriendList(mysocket,command);
            break;    
        case ADDFRIEND:
            Add_Friend(mysocket,command);
            break;
        case DELETEFRIEND:
            Delete_Friend(mysocket,command);
            break;
        case AGREEADDFRIEND:
            AgreeAddFriend(mysocket,command);
            break;
        case BLOCKFRIEND:
            Block_Friend(mysocket,command);
            break;
        case RESTOREFRIEND:
            Restore_Friend(mysocket,command);
            break;
        case REFUSEADDFRIEND:
            RefuseAddFriend(mysocket,command);
            break;
        case VIEWONLINESTATUS:
            ViewOnlineStatus(mysocket,command);
            break;
        case UNREADMESSAGE:
            UnreadMessage(mysocket,command);
            break;
        case CHATWITHFRIEND:
            ChatWithFriend(mysocket,command);
            break;
        case SENDMSG:
            FriendSendMsg(mysocket,command);
            break;
        case EXITCHAT:
            ExitChat(mysocket,command);
            break;
        case ADDGROUP:
            AddGroup(mysocket,command);
            break;
        case CREATEGROUP:
            CreateGroup(mysocket,command);
            break;
        case GROUPLIST:
            GroupList(mysocket,command);
            break;
        case MEMBERLIST:
            MemberList(mysocket,command);
            break;
        case DELETEGROUP:
            DeleteGroup(mysocket,command);
            break;
        case DELETEMEMBER:
            DeleteMember(mysocket,command);
            break;
        case ADDMANAGER:
            AddManager(mysocket,command);
            break;
        case DELETEMANAGER:
            DeleteManager(mysocket,command);
            break;
        case DISSOLVEGROUP:
            DissolveGroup(mysocket,command);
            break;
        case APPLYLIST:
            ApplyList(mysocket,command);
            break;
        case AGREEADDMEMBER:
            AgreeAddMember(mysocket,command);
            break;
        case REFUSEADDMEMBER:
            RefuseAddMember(mysocket,command);
            break;
    }

    return;
}

void Sign_up(TcpSocket mysocket,UserCommand command)//注册选项
{
    while(1)
    {
        //string uid=to_string((rand()+111111)%100000);

        random_device rd;
        mt19937 gen(rd()); // 使用 Mersenne Twister 引擎作为随机数生成器
        uniform_int_distribution<int> dis(0, 99999); // 定义一个均匀分布的随机整数分布

        string uid = to_string(dis(gen));
        cout << "生成的随机 uid 为：" << uid << endl;

        if(redis.sismember("用户uid集合",uid)){//随机到一个未注册的uid
            //cout << "生成的随机 uid 为：" << uid << endl;
            continue;
        }else{
            redis.saddvalue("用户uid集合", uid);
            redis.hsetValue(uid, "账号", uid);
            redis.hsetValue(uid, "昵称", command.m_nickname);
            redis.hsetValue(uid, "密码", command.m_option[0]);
            //redis.hsetValue(uid, "在线状态", "-1");
            redis.hsetValue(uid, "性别", "未知");
            redis.hsetValue(uid, "其他信息", "无");
            redis.hsetValue(uid, "通知套接字", "-1");
            redis.hsetValue(uid, "聊天对象", "无");
            redis.hsetValue(uid + "的未读消息", "通知消息", "0");
            redis.hsetValue(uid + "的未读消息", "好友申请", "0");
            redis.hsetValue(uid + "的未读消息", "群聊消息", "0");
           

            mysocket.SendMsg(uid);
            cout<<"用户"<<uid<<"注册成功"<<endl;
            return;
        }
    }
}

void Log_in(TcpSocket mysocket,UserCommand command)//登陆选项
{
    //从数据库跳去对应的数据进行核对并回复结果
    if(!redis.sismember("用户uid集合",command.m_uid)){//帐号不存在返回错误
        mysocket.SendMsg("nonexisent");
    }else{
        //如果帐号存在进行密码比对
        string pwd=redis.gethash(command.m_uid,"密码");
        if(pwd!=command.m_option[0])
        {
            mysocket.SendMsg("discorrect");

        }else{ 
            redis.saddvalue("在线用户列表",command.m_uid);
            //密码正确，可以登陆改变其在线状态
            //redis.hsetValue(command.m_uid,"在线状态",to_string(mysocket.getfd()));
            redis.hsetValue("fd-uid表",to_string(mysocket.getfd()),command.m_uid);
            redis.hsetValue(command.m_uid,"聊天对象","0");
            redis.hsetValue(command.m_uid,"通知套接字","-1");
           mysocket.SendMsg("ok");
           cout<<"用户"<<command.m_uid<<"登陆成功"<<endl;
        }
    }

    return;
}

//销毁账户
void Log_out(TcpSocket mysocket,UserCommand command)//功能已实现
{
    // 获取该用户的好友列表
    vector<string> friendList = redis.getFriendList(command.m_uid, "的好友列表");

    for (const string& friendID : friendList) {
        // 更新其他用户的好友列表，将该用户从好友列表中移除
        redis.removeMember(friendID+"的好友列表", command.m_uid);
    }

    //移除该用户的好友列表
    redis.delKey(command.m_uid+"的好友列表");
    //移除该用户的屏蔽列表
    redis.delKey(command.m_uid+"的屏蔽列表");

    //删除uid当中存储的特殊的信息
    redis.delKey(command.m_uid);//这一步成功执行
    redis.delKey(command.m_uid+"的未读消息");//这一步成功执行
    redis.delKey(command.m_uid+"的通知消息");

    //这一步已实现，要删除集合中特定元素的值
    redis.sremValue("用户uid集合", command.m_uid); //从用户uid集合中移除
    mysocket.SendMsg("ok");

}


//已实现
//如果好友列表可以展示在线状态，那么就没有必要有查看好友在线状态的选项
void FriendList(TcpSocket mysocket,UserCommand command)
{
    if(!redis.exists(command.m_uid+"的好友列表"))
    {
        //cout<<"1"<<endl;
        mysocket.SendMsg("none");
        return;
    }
    // 好友数量不为0，就遍历好友列表，根据在线状态发送要展示的内容
    
    vector<string> friendList = redis.getFriendList(command.m_uid,"的好友列表");
    

    for (const string& friendID : friendList) {
        if (!redis.sismember(command.m_uid + "的屏蔽列表", friendID)) {
            if (redis.sismember("在线用户列表",friendID)) {
                mysocket.SendMsg(L_GREEN+friendID+NONE);
            } else {
                mysocket.SendMsg(friendID);
            }
        }
    }

    mysocket.SendMsg("end");//展示完毕

}

void Add_Friend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.sismember("用户uid集合",command.m_recvuid))//如果没有找到该用户返回错误
    {
        mysocket.SendMsg("none");//该用户不存在
        return;
    }
    //遍历好友列表，判断帐号是否已经是自己的好友
    int num=redis.getListCount(command.m_uid,"的好友列表");//获得好友数量
    if(num!=0){
        vector<string> friendlist=redis.getFriendList(command.m_uid,"的好友列表");

        auto it=find(friendlist.begin(),friendlist.end(),command.m_recvuid);
        if(it!=friendlist.end())
        {
            mysocket.SendMsg("exist");//好友已存在
            return;
        }
    }
    //如果自己的系统消息里有对方发来的未处理的好友申请，就不能向对方发送好友请求
    if(redis.hexists(command.m_uid+"收到的好友申请",command.m_recvuid))
    {
        mysocket.SendMsg("apply");
        return;
        
    }
    //如果已经向对方发送过好友申请未被处理，则不能再次发送申请
    if(redis.hexists(command.m_recvuid+"收到的好友申请",command.m_uid))
    {
        mysocket.SendMsg("handle");
        return;
    }

    //如果以上情况都没有发生，就在对方的好友申请里更新自己的好友申请
    string apply="来自"+command.m_uid+"的好友申请:"+command.m_option[0];
    redis.hsetValue(command.m_recvuid+"收到的好友申请",command.m_uid,apply);
    //对方未读消息加1
    string nums=redis.gethash(command.m_recvuid+"的未读消息","好友申请");
    redis.hsetValue(command.m_recvuid+"的未读消息","好友申请",to_string(stoi(nums)+1));

    //给好友发送实时通知
    if(redis.sismember("在线用户列表",command.m_recvuid))
    {
        //cout<<"1"<<endl;
        string friend_fd=redis.gethash(command.m_recvuid,"通知套接字");
        TcpSocket friendsocket(stoi(friend_fd));
        string msg="您收到来自"+command.m_uid+"的好友申请";
        friendsocket.SendMsg(L_RED+msg+NONE);
    }

    mysocket.SendMsg("ok");
}

void Delete_Friend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");//不存在该好友
        return;
    }else if(redis.removeMember(command.m_uid+"的好友列表",command.m_option[0]))
    {
        redis.removeMember(command.m_option[0]+"的好友列表",command.m_uid);
        mysocket.SendMsg("ok");//成功将该好友从好友列表中删除
        return;
    }
}

void AgreeAddFriend(TcpSocket mysocket,UserCommand command)//同意好友申请
{
    //查看通知消息里有没有他的申请
    if(!redis.hexists(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }

    //没有上述情况正常同意

    if(redis.removeMember(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        string nownum = redis.gethash(command.m_uid + "的未读消息", "好友申请");
        redis.hsetValue(command.m_uid + "的未读消息", "好友申请", (to_string(stoi(nownum) - 1)));
        //完善同意者信息
        string nickname=redis.gethash(command.m_option[0],"昵称");
        //cout<<nickname<<endl;
        redis.hsetValue(command.m_uid+"的好友列表",command.m_option[0],nickname);
        redis.rpushValue(command.m_uid+"和"+command.m_option[0]+"的聊天记录","-------------------");

        //完善申请者信息
        //command.m_nickname不可以正确输出
        redis.hsetValue(command.m_option[0]+"的好友列表",command.m_uid,command.m_nickname);
        redis.rpushValue(command.m_option[0]+"和"+command.m_uid+"的聊天记录","-------------------");

        redis.rpushValue(command.m_option[0]+"的通知消息",command.m_uid+"通过了您的好友申请");

        // 申请者未读消息中的通知消息数量+1
        string num1 = redis.gethash(command.m_option[0] + "的未读消息", "通知消息");
        redis.hsetValue(command.m_option[0] + "的未读消息", "通知消息", to_string(stoi(num1)+1));

        //给好友发送实时通知
        if(redis.sismember("在线用户列表",command.m_option[0]))
        {
            string friend_fd=redis.gethash(command.m_option[0],"通知套接字");
            TcpSocket friendsocket(stoi(friend_fd));
            friendsocket.SendMsg(L_RED+command.m_uid+"通过了您的好友申请,快去和ta聊天吧"+NONE);
        }

        mysocket.SendMsg("ok");
        return;

    }
}


void RefuseAddFriend(TcpSocket mysocket,UserCommand command)//拒绝好友申请
{
    //查看通知消息里有没有他的申请
    if(!redis.hexists(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }
    //为什么不能正常进if语句
    //对不起我有病，我把宏定义的函数搞反了
    if(redis.removeMember(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        //cout<<"1"<<endl;
        //拒绝者未读消息好友申请
        string nownum = redis.gethash(command.m_uid + "的未读消息", "好友申请");
        redis.hsetValue(command.m_uid + "的未读消息", "好友申请", (to_string(stoi(nownum) - 1)));
        
        
        
        // 申请者未读消息中的通知消息数量+1
        string num1 = redis.gethash(command.m_option[0] + "的未读消息", "通知消息");
        redis.hsetValue(command.m_option[0] + "的未读消息", "通知消息", to_string(stoi(num1)+1));
        redis.rpushValue(command.m_option[0]+"的通知消息",command.m_uid+"拒绝了您的好友申请");

        //给好友发送实时通知
        if(redis.sismember("在线用户列表",command.m_option[0]))
        {
            string friend_fd=redis.gethash(command.m_option[0],"通知套接字");
            TcpSocket friendsocket(stoi(friend_fd));
            friendsocket.SendMsg(L_RED+command.m_uid+"拒绝了您的好友申请"+NONE);
        }

        mysocket.SendMsg("ok");
    }
    

}


void Block_Friend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");//不存在该好友
        return;
    }
    // 使用 Redis Set 的 SADD 命令将好友添加到屏蔽列表中
    bool success = redis.saddvalue(command.m_uid + "的屏蔽列表", command.m_option[0]);

    if (success)
    {
        mysocket.SendMsg("ok"); // 屏蔽成功
    }
    else
    {
        mysocket.SendMsg("handled"); // 已屏蔽过该好友
    }
}

void Restore_Friend(TcpSocket mysocket,UserCommand command)
{
     // 使用 Redis Set 的 SISMEMBER 命令检查好友是否在用户的屏蔽列表中
    bool isBlocked = redis.sismember(command.m_uid + "的屏蔽列表", command.m_option[0]);

    if (!isBlocked)
    {
        mysocket.SendMsg("no"); // 该好友未被屏蔽
        return;
    }

    // 使用 Redis Hash 的 HEXISTS 命令检查好友是否存在于用户的好友列表中
    bool isFriend = redis.hexists(command.m_uid + "的好友列表", command.m_option[0]);

    if (!isFriend)
    {
        mysocket.SendMsg("none"); // 不存在该好友
        return;
    }

    // 使用 Redis Set 的 SREM 命令将好友从屏蔽列表中移除
    bool success = redis.sremValue(command.m_uid + "的屏蔽列表", command.m_option[0]);

    if (success)
    {
        //cout<<"1"<<endl;
        mysocket.SendMsg("ok"); // 好友成功解除屏蔽
    }
    else
    {
        mysocket.SendMsg("error"); // 解除屏蔽失败
    }
    return;
}

void ViewOnlineStatus(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");
        return;
    }else if(!redis.sismember("在线用户列表",command.m_option[0]))
    {
        mysocket.SendMsg("no");
        return;
    }else{
        mysocket.SendMsg("ok");
        return;
    }
}

void UnreadMessage(TcpSocket mysocket,UserCommand command)
{
    string response;
    int num=stoi(redis.gethash(command.m_uid+"的未读消息","通知消息"));
    int num1=stoi(redis.gethash(command.m_uid+"的未读消息","好友申请"));
    int num2=num+num1;
    if(num2==0)
    {
        mysocket.SendMsg("no");
    }else{
        response="您有"+to_string(num2)+"条未读消息:\n";

        //获取通知消息具体内容
        for(int i=0;i<num;++i)
        {
            string notify=command.m_uid+"的通知消息";
            string notification=redis.lindexValue(notify,i);
            response+="通知"+to_string(i+1)+":"+notification+"\n";
        }

        //获取好友申请的具体内容
        vector<string> fieldNames = redis.getFriendList(command.m_uid,"收到的好友申请");
        for (size_t i = 0; i < fieldNames.size(); ++i) {
            // 使用字段名获取好友申请的具体内容
            string friendRequest = redis.gethash(command.m_uid + "收到的好友申请", fieldNames[i]);
            response += "好友申请" + to_string(i + 1) + ":" + friendRequest + "\n";
        }
    }

    response+="请尽快处理\n";

    mysocket.SendMsg(L_YELLOW+response+NONE);

    // 删除通知消息
    redis.delKey(command.m_uid + "的通知消息");
    // 清零未读消息的通知消息
    redis.hsetValue(command.m_uid + "的未读消息", "通知消息", "0");
}

void ChatWithFriend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_recvuid))
    {
        mysocket.SendMsg("nofind");
        exit(0);
    }
    mysocket.SendMsg("ok");
    //如果存在该好友就打印历史聊天记录
    vector<string>history=redis.lrangeAll(command.m_uid+"和"+command.m_recvuid+"的聊天记录");
    for(const string& msg:history)
    {
        mysocket.SendMsg(msg);
    }

    //设置其聊天对象
    redis.hsetValue(command.m_uid,"聊天对象",command.m_recvuid);

    mysocket.SendMsg("历史聊天记录展示完毕");

    
}

void FriendSendMsg(TcpSocket mysocket,UserCommand command)//发送消息
{
    //将新的消息加入消息队列
    string newmsg="我:"+command.m_option[0];
    redis.rpushValue(command.m_uid+"和"+command.m_recvuid+"的聊天记录",newmsg);


    //展示消息
    string my_recvfd=redis.gethash(command.m_uid,"通知套接字");
    TcpSocket my_socket(stoi(my_recvfd));
    my_socket.SendMsg(L_WHITE + newmsg + NONE);


    //没有被好友屏蔽
    if(redis.sismember(command.m_recvuid+"的屏蔽列表",command.m_uid))
    {
        string msg="您的消息已发出,但被对方拒收了";
        my_socket.SendMsg(L_RED+msg+NONE);
        mysocket.SendMsg("no");
        return;
        
    }
    string uid=command.m_uid;
    string msg1=uid+":"+command.m_option[0];
    redis.rpushValue(command.m_recvuid+"和"+command.m_uid+"的聊天记录",msg1);
    

    //好友此时在线并且在和我聊天
    if(redis.sismember("在线用户列表",command.m_recvuid)&&(redis.gethash(command.m_recvuid,"聊天对象")==command.m_uid))
    {
        string fr_recvfd=redis.gethash(command.m_recvuid,"通知套接字");
        TcpSocket fr_socket(stoi(fr_recvfd));
        fr_socket.SendMsg(L_GREEN+msg1+NONE);

    }else if(!redis.sismember("在线用户列表",command.m_recvuid))//好友不在线
    {
        string num = redis.gethash(command.m_recvuid + "的未读消息", "通知消息");
        redis.hsetValue(command.m_recvuid + "的未读消息", "通知消息", to_string(stoi(num)+1));
        redis.rpushValue(command.m_recvuid+"的通知消息",command.m_uid+"给您发来了一条消息");

    }else{
        string fr_recvfd=redis.gethash(command.m_recvuid,"通知套接字");
        TcpSocket fr_socket(stoi(fr_recvfd));
        fr_socket.SendMsg(L_RED+command.m_uid+"给您发来了一条消息"+NONE);
    }


    mysocket.SendMsg("ok");
    return;

}

void ExitChat(TcpSocket mysocket,UserCommand command)
{
    redis.hsetValue(command.m_uid,"聊天对象","0");
    redis.hsetValue(command.m_recvuid,"聊天对象","0");
    mysocket.SendMsg("ok");
    return ;
}

int main()
{

    int lfd=0,cfd=0,efd=0; 
    char *buf;

    struct sockaddr_in saddr,caddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(9999);
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);

    //创建套接字
    lfd=Socket(AF_INET,SOCK_STREAM,0);

    //设置端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //绑定地址
    Bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    
    //设置上限数
    Listen(lfd,128);

    struct epoll_event event;
    struct epoll_event resevent[1024];
    int size = sizeof(resevent) / sizeof(struct epoll_event);
    

    efd=epoll_create(100);//创建红黑树
    event.events=EPOLLIN|EPOLLET;//ET边缘触发

    event.data.fd=lfd;
    int ret=epoll_ctl(efd,EPOLL_CTL_ADD,lfd,&event);
    if(ret==-1)
    {
        perr_exit("epoll_ctl");
    }
       
    while(1)
    {
        int num=epoll_wait(efd,resevent,size,-1);
        for(int i=0;i<num;++i)
        {
            //取出当前文件描述符
            int curfd=resevent[i].data.fd;
            //判断文件描述符是否用于监听
            if(curfd==lfd)
            {
                //建立新的连接
                int cfd=Accept(curfd,NULL,NULL);

                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                // 新得到的文件描述符添加到epoll模型中, 下一轮循环的时候就可以被检测了
                event.events = EPOLLIN|EPOLLET;    // 读缓冲区是否有数据
                event.data.fd = cfd;
                ret = epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &event);
                if(ret == -1)
                {
                    perror("epoll_ctl-accept");
                    exit(0);
                }

                //redis.hsetValue("fd-uid表",to_string(curfd),"-1");
                //cout<<"客户端连接成功，套接字为:"<<curfd<<endl;
            }else{

                TcpSocket mysocket(curfd);
                
                //处理通信的文件描述符
                //接收数据
                char *buf;
               // memset(buf,0,sizeof(buf));用就出现段错误

                int ret=recvMsg(curfd,&buf);
                //cout<<ret<<endl;
                if(ret<=0)
                {
                    cerr<<"error receiving data ."<<endl;
                    string uid =redis.gethash("fd-uid表",to_string(curfd)); // 获取客户端的用户ID
                    redis.sremValue("在线用户列表",uid);
                    redis.hsetValue(uid,"通知套接字","-1");
                    redis.hsetValue("fd-uid表",to_string(curfd),"-1");
                    close(curfd);
                    continue;
                }

                buf[ret]='\0';
                //cout<<buf<<endl;
                string command_string=buf;

                UserCommand command;
                command.From_Json(command_string);
                cout<<"Recrived request:"<<command_string<<endl;
                //创建任务，处理客户端请求

                
                if(command.m_flag==RECV)
                {
                    redis.hsetValue(command.m_uid, "通知套接字", to_string(curfd));
                    //redis.hsetValue("fd-uid表", to_string(curfd), command.m_uid+"(通)");
                }else{
                    Argc_func argc_func(TcpSocket(curfd),command_string);
                    task(&argc_func);
                }
                
            }
        }
        
    } 
    
}

void CreateGroup(TcpSocket mysocket,UserCommand command)//创建群聊
{
    //判断好友列表里有没有该好友
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }

    //有的话进行下一步，随机到未被注册到的群聊uid
    while(1)
    {
        //随机数这样好像不太行，写完之后可以换一种生成随机数的方法
        random_device rd;
        mt19937 gen(rd()); // 使用 Mersenne Twister 引擎作为随机数生成器
        uniform_int_distribution<int> dis(10000000, 99999999); // 定义一个均匀分布的随机整数分布

        string groupuid = to_string(dis(gen));
        cout << "生成的随机群 uid 为：" << groupuid << endl;

        if(redis.sismember("群聊集合",groupuid))
        {
            continue;
        }else{
            redis.saddvalue("群聊集合",groupuid);
            //补充群聊详细信息
            redis.hsetValue(groupuid+"的基本信息","群号",groupuid);
            redis.hsetValue(groupuid+"的基本信息","群名称",groupuid);
            redis.hsetValue(groupuid+"的基本信息","群介绍","无");
            redis.hsetValue(groupuid+"群成员列表",command.m_uid,"群主");
            redis.hsetValue(command.m_uid+"的群聊列表",groupuid,"群主");
            redis.rpushValue(groupuid+"的群聊消息","这是一个新的群聊,欢迎大家的加入");

            string num = redis.gethash(command.m_option[0] + "的未读消息", "通知消息");
            redis.hsetValue(command.m_option[0] + "的未读消息", "通知消息", to_string(stoi(num)+1));
            redis.rpushValue(command.m_option[0]+"的通知消息",command.m_uid+"邀请您加入群聊"+groupuid);

            redis.hsetValue(groupuid+"群成员列表",command.m_option[0],"群成员");
            redis.hsetValue(command.m_option[0]+"的群聊列表",groupuid,"群成员");

            if(redis.sismember("在线用户列表",command.m_option[0]))
            {
                string friend_fd=redis.gethash(command.m_option[0],"通知套接字");
                TcpSocket friendsocket(stoi(friend_fd));
                friendsocket.SendMsg(L_BLUE+command.m_uid+"邀请您加入新的群聊"+NONE);
            }
        }
        mysocket.SendMsg(groupuid);
        return;
    }
    

}

void GroupList(TcpSocket mysocket,UserCommand command)
{
    if(!redis.exists(command.m_uid+"的群聊列表"))
    {
        mysocket.SendMsg("none");
        return;
    }

    vector<string> grouplist=redis.getFriendList(command.m_uid,"的群聊列表");

    for(const string& groupid:grouplist){
        mysocket.SendMsg(groupid);
    }

    mysocket.SendMsg("end");
}

void AddGroup(TcpSocket mysocket,UserCommand command)
{
    if(redis.hexists(command.m_option[0]+"群成员列表",command.m_uid))
    {
        mysocket.SendMsg("handled");
        return;
    }else if(!redis.sismember("群聊集合",command.m_option[0]))
    {
        mysocket.SendMsg("none");
        return;
    }else if(redis.hexists(command.m_option[0]+"的申请加群列表",command.m_uid)){
        mysocket.SendMsg("had");
    }else
    {
        //int num=redis.getListCount(command.m_option[0],"的群成员列表");

        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员")
            {
                string apply=command.m_uid+"申请加群"+command.m_option[0];
                string num=redis.gethash(memberid+"的未读消息","群聊消息");
                redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.rpushValue(memberid+"群聊消息",apply);

                if(redis.sismember("在线用户列表",memberid))
                {
                    string member_fd=redis.gethash(memberid,"通知套接字");
                    TcpSocket membersocket(stoi(member_fd));
                    membersocket.SendMsg(L_BLUE+apply+NONE);
                }
            }
        }

        mysocket.SendMsg("ok");
        return;
    }
}

void MemberList(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");
        return;
    }else {
        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            mysocket.SendMsg(memberid);//一直循环打印
        }
        mysocket.SendMsg("end");
    }
}

void DeleteGroup(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");
        return;
    }

    //将群聊从用户的群聊列表里删除
    redis.removeMember(command.m_uid+"的群聊列表",command.m_option[0]);
    redis.removeMember(command.m_option[0]+"群成员列表",command.m_uid);
    mysocket.SendMsg("ok");
}

void DeleteMember(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_recvuid+"群成员列表",command.m_uid))
    {
        mysocket.SendMsg("none");
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_uid)=="群成员")
    {
        mysocket.SendMsg("no");
        return;
    }

    redis.removeMember(command.m_option[0]+"的群聊列表",command.m_recvuid);
    redis.removeMember(command.m_recvuid+"群成员列表",command.m_option[0]);

    vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

    for(const string& memberid:memberlist)
    {
        if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员")
        {
            string apply=command.m_uid+"将"+command.m_option[0]+"移除群聊"+command.m_recvuid;
            string num=redis.gethash(memberid+"的未读消息","群聊消息");
            redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
            redis.rpushValue(memberid+"群聊消息",apply);

            if(redis.sismember("在线用户列表",memberid))
            {
                string member_fd=redis.gethash(memberid,"通知套接字");
                TcpSocket membersocket(stoi(member_fd));
                membersocket.SendMsg(L_BLUE+apply+NONE);
            }
        }
    }

    mysocket.SendMsg("ok");
    return;

}

void AddManager(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_recvuid))
    {
        mysocket.SendMsg("none");
        return;
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_uid)!="群主")
    {
        mysocket.SendMsg("no");
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_option[0])=="群管理员")
    {
        mysocket.SendMsg("handled");
    }

    redis.hsetValue(command.m_recvuid+"群成员列表",command.m_option[0],"群管理员");

    if(redis.sismember("在线用户列表",command.m_option[0]))
    {
        string member_fd=redis.gethash(command.m_option[0],"通知套接字");
        TcpSocket membersocket(stoi(member_fd));
        membersocket.SendMsg(L_BLUE+command.m_uid+"将你添加为群聊"+command.m_recvuid+"的群管理员"+NONE);
    }

    mysocket.SendMsg("ok");
}

void DeleteManager(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_recvuid))
    {
        mysocket.SendMsg("none");
        return;
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_uid)!="群主")
    {
        mysocket.SendMsg("no");
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_option[0])!="群管理员")
    {
        mysocket.SendMsg("handled");
    }

    redis.hsetValue(command.m_recvuid+"群成员列表",command.m_option[0],"群成员");

    if(redis.sismember("在线用户列表",command.m_option[0]))
    {
        string member_fd=redis.gethash(command.m_option[0],"通知套接字");
        TcpSocket membersocket(stoi(member_fd));
        membersocket.SendMsg(L_BLUE+command.m_uid+"将你在"+command.m_recvuid+"的群管理员身份移除"+NONE);
    }

    mysocket.SendMsg("ok");
}

void DissolveGroup(TcpSocket mysocket,UserCommand command)
{
    if(redis.gethash(command.m_recvuid+"群成员列表",command.m_uid)!="群主")
    {
        mysocket.SendMsg("no");
        return;
    }

    redis.sremValue("群聊集合",command.m_option[0]);//将群聊从群聊集合中删除
    redis.delKey(command.m_option[0]);//删除群聊信息

    vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

    for(const string& memberid:memberlist)
    {
        redis.removeMember(memberid+"的群聊列表",command.m_option[0]);
        string apply=command.m_uid+"(群主)将"+command.m_option[0]+"群聊解散";
        string num=redis.gethash(memberid+"的未读消息","群聊消息");
        redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
        redis.rpushValue(memberid+"群聊消息",apply);

        if(redis.sismember("在线用户列表",memberid))
        {
            string member_fd=redis.gethash(memberid,"通知套接字");
            TcpSocket membersocket(stoi(member_fd));
            membersocket.SendMsg(L_BLUE+apply+NONE);
        }
        
    }

}

void ApplyList(TcpSocket mysocket,UserCommand command)
{
    if(redis.gethash(command.m_option[0]+"群成员列表",command.m_uid)!="群成员")
    {
        vector<string> memberlist=redis.getFriendList(command.m_option[0],"的申请加群列表");

        for(const string& memberid:memberlist)
        {
            mysocket.SendMsg(memberid);
        }
        mysocket.SendMsg("end");
    }else
    {
        mysocket.SendMsg("no");
    }
}

void AgreeAddMember(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_recvuid+"的申请加群列表",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }else{
        redis.removeMember(command.m_recvuid+"的申请加群列表",command.m_option[0]);
        redis.hsetValue(command.m_recvuid+"群成员列表",command.m_option[0],"群主");
        redis.hsetValue(command.m_option[0]+"的群聊列表",command.m_recvuid,"群成员");

        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员"&&command.m_uid!=memberid)
            {
                string apply=command.m_uid+"同意了"+command.m_option[0]+"的加群申请";
                string num=redis.gethash(memberid+"的未读消息","群聊消息");
                redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.rpushValue(memberid+"群聊消息",apply);

                if(redis.sismember("在线用户列表",memberid))
                {
                    string member_fd=redis.gethash(memberid,"通知套接字");
                    TcpSocket membersocket(stoi(member_fd));
                    membersocket.SendMsg(L_BLUE+apply+NONE);
                }
            }
        }
    }
    mysocket.SendMsg("ok");
    if(redis.sismember("在线用户列表",command.m_option[0]))
    {
        string member_fd=redis.gethash(command.m_option[0],"通知套接字");
        TcpSocket membersocket(stoi(member_fd));
        membersocket.SendMsg(L_BLUE+command.m_recvuid+":您的加群申请已被通过"+NONE);
    }
}

void RefuseAddMember(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_recvuid+"的申请加群列表",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }else{
        redis.removeMember(command.m_recvuid+"的申请加群列表",command.m_option[0]);
        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员"&&command.m_uid!=memberid)
            {
                string apply=command.m_uid+"拒绝了"+command.m_option[0]+"的加群申请";
                string num=redis.gethash(memberid+"的未读消息","群聊消息");
                redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.rpushValue(memberid+"群聊消息",apply);

                if(redis.sismember("在线用户列表",memberid))
                {
                    string member_fd=redis.gethash(memberid,"通知套接字");
                    TcpSocket membersocket(stoi(member_fd));
                    membersocket.SendMsg(L_BLUE+apply+NONE);
                }
            }
        }
        mysocket.SendMsg("ok");
        if(redis.sismember("在线用户列表",command.m_option[0]))
        {
            string member_fd=redis.gethash(command.m_option[0],"通知套接字");
            TcpSocket membersocket(stoi(member_fd));
            membersocket.SendMsg(L_BLUE+command.m_recvuid+":您的加群申请被拒绝"+NONE);
        }
    }
}