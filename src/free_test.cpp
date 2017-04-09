#include <ros/ros.h>
#include <std_msgs/String.h>
#include "6_free_script.h"

static CFreeScript free_script;

void KeywordCB(const std_msgs::String::ConstPtr & msg)
{
    //ROS_WARN("[KeywordCB] - %s",msg->data.c_str());
    string strListen = msg->data;
    free_script.strListen = strListen;
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "free_script_2017");
    ROS_INFO("[main] free_script_2017");
    free_script.Init();
    free_script.Queue();
    free_script.ShowActs();

    ros::NodeHandle n;
    ros::Subscriber sub_sr = n.subscribe("/xfyun/iat", 10, KeywordCB);
    ros::Rate r(10);
    while(ros::ok())
    {
        free_script.Main();
        ros::spinOnce();
        r.sleep();
    }

    return 0;
}