#ifndef HOME_STRUCTS
#define HOME_STRUCTS
#include<string>

using namespace std;

#define ACT_GOTO		1
#define ACT_FIND_OBJ	2
#define ACT_GRAB		3
#define ACT_PASS		4
#define ACT_SPEAK		5
#define ACT_LISTEN		6
#define ACT_QUESTION	13

typedef struct stAct
{
	int nAct;
	string  strTarget;
	float nDuration;
}stAct;


#endif // HOME_STRUCTS