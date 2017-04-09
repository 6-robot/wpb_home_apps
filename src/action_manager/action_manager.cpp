#include "action_manager.h"
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <waterplus_map_tools/GetWaypointByName.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
static ros::Publisher spk_pub;
static string strToSpeak = "";
static string strKeyWord = "";
static ros::ServiceClient cliGetWPName;
static waterplus_map_tools::GetWaypointByName srvName;

CActionManager::CActionManager()
{
    nCurActIndex = 0;
    nCurActCode = -1;
    strListen = "";
}

CActionManager::~CActionManager()
{

}

void CActionManager::Init()
{
    ros::NodeHandle n;
    cliGetWPName = n.serviceClient<waterplus_map_tools::GetWaypointByName>("/waterplus/get_waypoint_name");
    spk_pub = n.advertise<std_msgs::String>("/xfyun/tts", 20);
}

static int nLastActCode = -1;
bool CActionManager::Main()
{
    int nNumOfAct = arAct.size();
    if(nCurActIndex >= nNumOfAct)
    {
        return false;
    }
    int nKeyWord = -1;
    nCurActCode = arAct[nCurActIndex].nAct;
    switch (nCurActCode)
	{
	case ACT_GOTO:
		if (nLastActCode != ACT_GOTO)
		{
			string strGoto = arAct[nCurActIndex].strTarget;
            printf("[ActMgr] %d - Goto %s",nCurActIndex,strGoto.c_str());
            srvName.request.name = strGoto;
            if (cliGetWPName.call(srvName))
            {
                std::string name = srvName.response.name;
                float x = srvName.response.pose.position.x;
                float y = srvName.response.pose.position.y;
                ROS_INFO("Get_wp_name: name = %s (%.2f,%.2f)", strGoto.c_str(),x,y);

                MoveBaseClient ac("move_base", true);
                if(!ac.waitForServer(ros::Duration(5.0)))
                {
                    ROS_INFO("The move_base action server is no running. action abort...");
                }
                else
                {
                    move_base_msgs::MoveBaseGoal goal;
                    goal.target_pose.header.frame_id = "map";
                    goal.target_pose.header.stamp = ros::Time::now();
                    goal.target_pose.pose = srvName.response.pose;
                    ac.sendGoal(goal);
                    ac.waitForResult();
                    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
                        ROS_INFO("Arrived at %s!",strGoto.c_str());
                    else
                        ROS_INFO("Failed to get to %s ...",strGoto.c_str() );
                }
                
            }
            else
            {
                ROS_ERROR("Failed to call service GetWaypointByName");
            }
            nCurActIndex ++;
        }
		break;

	case ACT_FIND_OBJ:
		if (nLastActCode != ACT_FIND_OBJ)
		{
            printf("[ActMgr] %d - Find %s\n",nCurActIndex,arAct[nCurActIndex].strTarget.c_str());
            nCurActIndex ++;
		}
		break;

	case ACT_GRAB:
		if (nLastActCode != ACT_GRAB)
		{
            printf("[ActMgr] %d - Grab %s\n",nCurActIndex,arAct[nCurActIndex].strTarget.c_str());
            nCurActIndex ++;
		}
		break;

	case ACT_PASS:
		if (nLastActCode != ACT_PASS)
		{
            printf("[ActMgr] %d - Pass %s\n",nCurActIndex,arAct[nCurActIndex].strTarget.c_str());
            nCurActIndex ++;
		}
		break;

	case ACT_SPEAK:
		if (nLastActCode != ACT_SPEAK)
		{
            printf("[ActMgr] %d - Speak %s\n",nCurActIndex,arAct[nCurActIndex].strTarget.c_str());
            strToSpeak = arAct[nCurActIndex].strTarget;
            std_msgs::String rosSpeak;
            rosSpeak.data = strToSpeak;
            spk_pub.publish(rosSpeak);
            strToSpeak = "";
            nCurActIndex ++;
		}
		break;

	case ACT_LISTEN:
		if (nLastActCode != ACT_LISTEN)
		{
            printf("[ActMgr] %d - Listen %s\n",nCurActIndex,arAct[nCurActIndex].strTarget.c_str());
            strListen = "";
            strKeyWord = arAct[nCurActIndex].strTarget;
		}
        nKeyWord = strListen.find(strKeyWord);
        if(nKeyWord >= 0)  nCurActIndex ++;
		break;

	case ACT_QUESTION:
		if (nLastActCode != ACT_QUESTION)
		{
            printf("[ActMgr] %d - answer a question \n",nCurActIndex);
            nCurActIndex ++;
		}
		break;

	default:
		break;
	}
	nLastActCode = nCurActCode;
    return true;
}

void CActionManager::Reset()
{
    strToSpeak = "";
    nCurActIndex = 0;
	nLastActCode = 0;
    arAct.clear();
}

string CActionManager::GetToSpeak()
{
    string strRet = strToSpeak;
    strToSpeak = "";
    return strRet;
}

string ActionText(stAct* inAct)
{
    string ActText = "";
    if(inAct->nAct == ACT_GOTO)
    {
        ActText = "去往地点 ";
        ActText += inAct->strTarget;
    }
    if(inAct->nAct == ACT_FIND_OBJ)
    {
        ActText = "搜索物品 ";
        ActText += inAct->strTarget;
    }
    if(inAct->nAct == ACT_GRAB)
    {
        ActText = "抓取物品 ";
        ActText += inAct->strTarget;
    }
    if(inAct->nAct == ACT_PASS)
    {
        ActText = "把物品递给 ";
        ActText += inAct->strTarget;
    }
    if(inAct->nAct == ACT_SPEAK)
    {
        ActText = "说话 ";
        ActText += inAct->strTarget;
    }
    if(inAct->nAct == ACT_LISTEN)
    {
        ActText = "听取关键词 ";
        ActText += inAct->strTarget;
    }
    if(inAct->nAct == ACT_QUESTION)
    {
        ActText = "回答问题 ";
    }
    return ActText;
}

void CActionManager::ShowActs()
{
    printf("\n*********************************************\n");
    printf("显示行为队列:\n");
    int nNumOfAct = arAct.size();
    stAct tmpAct;
    for(int i=0;i<nNumOfAct;i++)
    {
        tmpAct = arAct[i];
        string act_txt = ActionText(&tmpAct);
        printf("行为 %d : %s\n",i+1,act_txt.c_str());
    }
    printf("*********************************************\n\n");
}
