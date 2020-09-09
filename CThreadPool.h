#include <thread>
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <functional>
#include "CMutex.h"

//typedef  void (*CallBackFunc)(void *);

enum TimerLoop
{
	NoLoop	= 0,
	Loop    = 1,
};

struct stThreadTask
{
	unsigned int iObjId;		//任务ID
	unsigned int iObjType;		//任务类型
	unsigned int iTime;			//时间 毫秒级别
	TimerLoop    eLoop;
	void         *pParam1;		//参数1
	void         *pParam2;		//参数2
	void         *pParam3;		//参数3
	void         *pParam4;		//参数4
	void         *pParam5;		//参数5
	stThreadTask()
	{
		iObjId = 0;
		iTime = 0;
		eLoop = NoLoop;
		pParam1 = NULL;
		pParam2 = NULL;
		pParam3 = NULL;
		pParam4 = NULL;
		pParam5 = NULL;
	}
	//直接浅拷贝即可
};

struct stTimerTask
{
	unsigned int iObjId;		//任务ID
	unsigned int iObjType;		//任务类型
	unsigned int iTime;			//时间 毫秒级别
	TimerLoop    eLoop;			//是否循环
	void         *pParam1;		//参数1
	void         *pParam2;		//参数2
	void         *pParam3;		//参数3
	void         *pParam4;		//参数4
	void         *pParam5;		//参数5
	unsigned int iTimeBak;		//时间 毫秒级别
	stTimerTask()
	{
		iObjId = 0;
		iTime = 0;
		eLoop = NoLoop;
		pParam1 = NULL;
		pParam2 = NULL;
		pParam3 = NULL;
		pParam4 = NULL;
		pParam5 = NULL;
	}
	//直接浅拷贝即可
};

class CThread;
class CTimeProcess;
class CThreadPool
{

public:
	CThreadPool(unsigned int iThreadCount, unsigned int iAddThreadCount, std::function<void(void*)> callBack);
	~CThreadPool();
	void AddTask(stThreadTask);
	void HandleMsg(void *task);
	static CThreadPool* GetInterface();
	DEFINE_MUTEX
private:
	unsigned int m_iThreadCount;		//当前线程数量
	unsigned int m_iLessThreadCount;	//最低线程数量
	unsigned int m_iAddThreadCount;		//增加线程数量
	unsigned int m_iCusThread;
	std::vector<CThread *> m_ThreadVec;	//线程
	CTimeProcess *m_TimeProcess;
};

class CThread
{
public:
	CThread(std::function<void(void*)>);
	~CThread();

	//线程函数的默认执行函数
	virtual void Run();
	virtual void Start();
	virtual void DispatchTaskMsg(stThreadTask);
	virtual void AddTask(stThreadTask task);

	DEFINE_MUTEX
protected:
	std::deque<stThreadTask> m_taskDeque;			//任务队列
	std::thread				*m_pThread;				//保存当前的线程指针
	bool					 m_bStart;				//线程状态  是否开启
	int						 m_iTaskCount;			//任务数量
	std::function<void(void*)> m_pFun;		        //线程回调
};

//简易定时器
class CTimeProcess : public CThread
{
public:
	CTimeProcess(unsigned int iSleepTime, std::function<void(void*)> pFunc);
	~CTimeProcess();

	virtual void Run();//重写线程函数
	virtual void AddTask(stThreadTask);
	void ClearTimer(unsigned int);
	int  GetObjTime(unsigned int);
		
	DEFINE_MUTEX
private:
	std::list<stTimerTask> m_taskList;
	unsigned int m_iSleepTime;
};