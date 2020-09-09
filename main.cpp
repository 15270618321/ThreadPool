#include <iostream>
#include "CThreadPool.h"

enum eOperate
{
	eAddOperate = 0,
	eSubOperate = 1,
};

void HandleMsg(void * pPack)
{
	stThreadTask *pPackMsg = (stThreadTask *)pPack;
	switch (pPackMsg->iObjId)
	{
	case eAddOperate:
	{
		system("color 4");
		printf("\033[1;31;40m this is add operate \033[0m \n");
		break;
	}
	case eSubOperate:
	{
		system("color 6");
		printf("\033[1;33;40m this is sub operate \033[0m \n");
		break;
	}
	default:
		break;
	}
}

//创建线程池
CThreadPool threadPool(100, 5, HandleMsg);

void SetTimer(int iOper,int iTimer)
{
	stThreadTask task;
	task.iObjId = iOper;
	task.iTime = iTimer;
	task.eLoop = NoLoop;
	threadPool.AddTask(task);
}

void SetLoopTimer(int iOper, int iTimer)
{
	stThreadTask task;
	task.iObjId = iOper;
	task.iTime = iTimer;
	task.eLoop = Loop;
	threadPool.AddTask(task);
}

int main()
{
	//创建任务
	SetTimer(eAddOperate, 2000);
	SetTimer(eSubOperate, 4000);

	SetLoopTimer(eAddOperate, 2000);
	SetLoopTimer(eSubOperate, 4000);

	while (true)
	{
		int iTimes = rand()%1000;
		for (int i = 0; i < iTimes; i++)
		{
			SetTimer(eAddOperate, 0);
			SetTimer(eSubOperate, 0);
		}
		
		//Sleep(rand() % 1000);
	}

	system("pause");
	return 0;
}

