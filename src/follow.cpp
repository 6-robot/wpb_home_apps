/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2017-2020, Waterplus http://www.6-robot.com
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the WaterPlus nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  FOOTPRINTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/
/* @author Zhang Wanjie                                             */
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sound_play/SoundRequest.h>
#include "xfyun_waterplus/IATSwitch.h"
#include "wpb_home_tutorials/Follow.h"

#define STATE_READY     0
#define STATE_FOLLOW    1
#define STATE_WAIT      2

static ros::Publisher spk_pub;
static ros::ServiceClient follow_start;
static ros::ServiceClient follow_stop;
static ros::ServiceClient follow_resume;
static ros::ServiceClient clientIAT;
static xfyun_waterplus::IATSwitch srvIAT;
static int nState = STATE_READY;
static int nWaitCnt = 10;           //倒计时时间

static void Speak(std::string inStr)
{
    sound_play::SoundRequest sp;
    sp.sound = sound_play::SoundRequest::SAY;
    sp.command = sound_play::SoundRequest::PLAY_ONCE;
    sp.arg = inStr;
    spk_pub.publish(sp);
}

void KeywordCB(const std_msgs::String::ConstPtr & msg)
{
    //ROS_WARN("[KeywordCB] - %s",msg->data.c_str());
    int nFindIndex = 0;
    nFindIndex = msg->data.find("Follow me");
    if( nFindIndex >= 0 )
    {
        //ROS_WARN("[KeywordCB] - 开始跟随");
        Speak("OK, Let's go.'");
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

    nFindIndex = msg->data.find("Stop follow");
    if( nFindIndex >= 0 )
    {
        //ROS_WARN("[KeywordCB] - 停止跟随");
        Speak("OK, I will stay here for 10 seconds.'");
        wpb_home_tutorials::Follow srv_stop;
        if (follow_stop.call(srv_stop))
        {
            ROS_WARN("[KeywordCB] - stop following!");
            nWaitCnt = 10;
            nState = STATE_WAIT;
            //识别完毕,关闭语音识别
            srvIAT.request.active = false;
            clientIAT.call(srvIAT);
            usleep(3*1000*1000);
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
    ros::Subscriber sub_sr = n.subscribe("/xfyun/iat", 10, KeywordCB);
    spk_pub = n.advertise<sound_play::SoundRequest>("/robotsound", 20);

    clientIAT = n.serviceClient<xfyun_waterplus::IATSwitch>("xfyun_waterplus/IATSwitch");
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
            ROS_INFO("[main] waiting downcount ... %d ", nWaitCnt);
            std::ostringstream stringStream;
            stringStream << nWaitCnt;
            std::string retStr = stringStream.str();
            Speak(retStr);
            nWaitCnt --;
            if(nWaitCnt <= 0)
            {
                Speak("OK, Move on.'");
                wpb_home_tutorials::Follow srv_resume;
                if (follow_resume.call(srv_resume))
                {
                    ROS_WARN("[main] - continue!");
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