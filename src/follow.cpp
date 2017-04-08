#include <ros/ros.h>
#include <std_msgs/String.h>
#include "wpb_home_tutorials/Follow.h"

#define STATE_READY     0
#define STATE_FOLLOW    1
#define STATE_WAIT      2

static ros::Publisher spk_pub;
static ros::ServiceClient follow_start;
static ros::ServiceClient follow_stop;
static ros::ServiceClient follow_resume;
static int nState = STATE_READY;
static int nWaitDelay = 0;      //停止等待时间
static int nWaitCnt = 0;

void KeywordCB(const std_msgs::String::ConstPtr & msg)
{
    //ROS_WARN("[KeywordCB] - %s",msg->data.c_str());
    int nFindIndex = 0;
    nFindIndex = msg->data.find("跟");
    if( nFindIndex >= 0 )
    {
        ///xfei/iat std_msgs/String -- "跟着我"
        //ROS_WARN("[KeywordCB] - 开始跟随");
        wpb_home_tutorials::Follow srv_start;
        srv_start.request.thredhold = 0.7;
        if (follow_start.call(srv_start))
        {
            ROS_WARN("[KeywordCB] - follow start !");
            nState = STATE_FOLLOW;
        }
        else
        {
            ROS_WARN("[KeywordCB] - follow start failed...");
        }
    }

    nFindIndex = msg->data.find("停");
    if( nFindIndex >= 0 )
    {
        ///xfei/iat std_msgs/String -- "停下来"
        //ROS_WARN("[KeywordCB] - 停止跟随");
        wpb_home_tutorials::Follow srv_stop;
        if (follow_stop.call(srv_stop))
        {
            ROS_WARN("[KeywordCB] - stop following!");
            nWaitDelay = 8;
            nWaitCnt = 0;
            nState = STATE_WAIT;
        }
        else
        {
            ROS_WARN("[KeywordCB] - failed to stop following...");
        }
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "wpb_home_follow_2017");

    ros::NodeHandle n;
    ros::Subscriber sub_sr = n.subscribe("/xfei/iat", 10, KeywordCB);

    follow_start = n.serviceClient<wpb_home_tutorials::Follow>("wpb_home_follow/start");
    follow_stop = n.serviceClient<wpb_home_tutorials::Follow>("wpb_home_follow/stop");
    follow_resume = n.serviceClient<wpb_home_tutorials::Follow>("wpb_home_follow/resume");

    ROS_INFO("[main] wpb_home_follow_2017");
    ros::Rate r(1);
    while(ros::ok())
    {
        ros::spinOnce();
        r.sleep();
        if(nState == STATE_WAIT)
        {
            ROS_INFO("[main] waiting ... %d / %d", nWaitCnt, nWaitDelay);
            nWaitCnt ++;
            if(nWaitCnt >= nWaitDelay)
            {
                wpb_home_tutorials::Follow srv_resume;
                if (follow_resume.call(srv_resume))
                {
                    ROS_WARN("[main] - continue!");
                    nWaitDelay = 5;
                    nWaitCnt = 0;
                    nState = STATE_FOLLOW;
                }
                else
                {
                    ROS_WARN("[main] - failed to continue...");
                    nWaitCnt = 0;
                }
            }
        }
    }

    return 0;
}