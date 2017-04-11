#include "10_home_script.h"
CHomeScript::CHomeScript()
{
    
}

CHomeScript::~CHomeScript()
{

}

void CHomeScript::Queue()
{
    stAct newAct;

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "家庭生活服务 项目开始";
    newAct.nDuration = 3;
    arAct.push_back(newAct);

    newAct.nAct = ACT_LISTEN;
    newAct.strTarget = "机器人";
    arAct.push_back(newAct);
    
    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "你好,已经跟着你了,主人";
    newAct.nDuration = 1;
    arAct.push_back(newAct);

    newAct.nAct = ACT_FOLLOW;
    newAct.fFollowDist = 0.5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_LISTEN;
    newAct.strTarget = "渴了";    //事先在地图标记好follow航点,主人走到这里再说关键词"渴了",否则机器人找不回来
    newAct.nDuration = 3;
    arAct.push_back(newAct);

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "好的,主人你想喝点什么";
    newAct.nDuration = 3;
    arAct.push_back(newAct);

    newAct.nAct = ACT_LISTEN;
    newAct.strTarget = "可乐";
    newAct.nDuration = 3;
    arAct.push_back(newAct);

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "好的,这就给您去取";
    newAct.nDuration = 3;
    arAct.push_back(newAct);

    newAct.nAct = ACT_GOTO;
    newAct.strTarget = "kitchen";
    arAct.push_back(newAct);

    newAct.nAct = ACT_GOTO;
    newAct.strTarget = "follow";
    arAct.push_back(newAct);

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "饮料已取回,主人请慢用";
    newAct.nDuration = 3;
    arAct.push_back(newAct);
}