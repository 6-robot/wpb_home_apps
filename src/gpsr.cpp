#include <ros/ros.h>
#include <std_msgs/String.h>
#include <vector>
#include "action_manager.h"
#include "xfyun_waterplus/IATSwitch.h"

using namespace std;

static CActionManager action_manager;
static ros::ServiceClient clientIAT;

//有限状态机
#define STATE_READY     0
#define STATE_WAIT_ENTR 1
#define STATE_GOTO_CMD  2
#define STATE_WAIT_CMD  3
#define STATE_ACTION    4

static int nState = STATE_WAIT_CMD;

//识别结果
static string result_placement;     //去往地点
static string result_object;        //抓取物品
static string result_person;        //交互对象人
static string result_action;        //其他行为

//识别关键词
static vector<string> arKWPlacement;
static vector<string> arKWObjece;
static vector<string> arKWPerson;
static vector<string> arKWAction;
void Init_keywords()
{
    //地点关键词
    arKWPlacement.push_back("liveing room");
    arKWPlacement.push_back("kitchen");
    arKWPlacement.push_back("bedroom");

    //物品关键词
    arKWObjece.push_back("water");
    arKWObjece.push_back("chips");
    arKWObjece.push_back("milk");

    //人名关键词
    arKWPerson.push_back("jack");
    arKWPerson.push_back("tom");

    //其他行为
    arKWAction.push_back("team name");
    arKWAction.push_back("your name");
}

string FindWord(string inSentence, vector<string> & arWord)
{
    string strRes = "";
	int nNum = arWord.size();
	for (int i = 0; i < nNum; i++)
	{
		int tmpIndex = inSentence.find(arWord[i]);
		if (tmpIndex >= 0)
		{
			strRes = arWord[i];
			break;
		}
	}
	return strRes;
}

static ros::Publisher spk_pub;
void KeywordCB(const std_msgs::String::ConstPtr & msg)
{
    //ROS_WARN("[GPSR KeywordCB] - %s",msg->data.c_str());
    string strListen = msg->data;

    if(nState == STATE_WAIT_CMD)
    {
        bool bAction = false;
        //[1]从听到的句子里找地点
        string placement = FindWord(strListen,arKWPlacement);
        if(placement.length() > 0)
        {
            printf("句子里包含地点 - %s \n",placement.c_str());
            stAct newAct;
            newAct.nAct = ACT_GOTO;
            newAct.strTarget = placement;
            action_manager.arAct.push_back(newAct);
            bAction = true;
        }

        //[2]从听到的句子里找物品
        string object = FindWord(strListen,arKWObjece);
        if(object.length() > 0)
        {
            printf("句子里包含物品 - %s \n",object.c_str());
            stAct newAct;
            newAct.nAct = ACT_FIND_OBJ;
            newAct.strTarget = object;
            action_manager.arAct.push_back(newAct);
            bAction = true;
        }

        //[3]从听到的句子里找人名
        string person = FindWord(strListen,arKWPerson);
        if(person.length() > 0)
        {
            printf("句子里包含人名 - %s \n",person.c_str());
            stAct newAct;
            newAct.nAct = ACT_PASS;
            newAct.strTarget = person;
            action_manager.arAct.push_back(newAct);
            bAction = true;
        }

        //[4]从听到的句子里找其他行为
        string action = FindWord(strListen,arKWAction);
        if(action.length() > 0)
        {
            printf("句子里包含其他行为 - %s \n",action.c_str());
            stAct newAct;
            newAct.nAct = ACT_SPEAK;
            //根据行为来确定说话内容
            if(action == "team name")
            {
                newAct.strTarget = "The team name is Shanghai University";
            }
            if(action == "your name")
            {
                newAct.strTarget = "My name is Robot";
            }
            action_manager.arAct.push_back(newAct);
            bAction = true;
        }

        if(bAction == true)
        {
            action_manager.ShowActs();
            //识别完毕,关闭语音识别
            xfyun_waterplus::IATSwitch srvIAT;
            srvIAT.request.active = false;
            clientIAT.call(srvIAT);
        }
    }

    if(nState == STATE_ACTION && action_manager.nCurActCode == ACT_LISTEN)
    {
        action_manager.strListen = strListen;
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "wpb_home_gpsr_2017");
    Init_keywords();

    ros::NodeHandle n;
    ros::Subscriber sub_sr = n.subscribe("/xfyun/iat", 10, KeywordCB);
    clientIAT = n.serviceClient<xfyun_waterplus::IATSwitch>("xfyun_waterplus/IATSwitch");

    ROS_INFO("[main] wpb_home_gpsr_2017");
    ros::Rate r(10);
    while(ros::ok())
    {
        if(nState == STATE_WAIT_ENTR)
        {
            //等待开门,一旦检测到开门,便去往发令地点
        }
        if(nState == STATE_ACTION)
        {
            action_manager.Main();
        }
        ros::spinOnce();
        r.sleep();
    }

    return 0;
}