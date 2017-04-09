#ifndef WP_ACTION_MANAGER_H
#define WP_ACTION_MANAGER_H
#include "struct.h"
#include <vector>

class CActionManager
{
public:
	CActionManager();
	~CActionManager();

    vector<stAct> arAct;
	int nCurActIndex;
	int nCurActCode;
	std::string strListen;

	bool Main();
	void Init();
	void Reset();
	string GetToSpeak();
	void ShowActs();
};

#endif // WP_ACTION_MANAGER_H