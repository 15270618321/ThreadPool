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
	unsigned int iObjId;		//����ID
	unsigned int iObjType;		//��������
	unsigned int iTime;			//ʱ�� ���뼶��
	TimerLoop    eLoop;
	void         *pParam1;		//����1
	void         *pParam2;		//����2
	void         *pParam3;		//����3
	void         *pParam4;		//����4
	void         *pParam5;		//����5
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
	//ֱ��ǳ��������
};

struct stTimerTask
{
	unsigned int iObjId;		//����ID
	unsigned int iObjType;		//��������
	unsigned int iTime;			//ʱ�� ���뼶��
	TimerLoop    eLoop;			//�Ƿ�ѭ��
	void         *pParam1;		//����1
	void         *pParam2;		//����2
	void         *pParam3;		//����3
	void         *pParam4;		//����4
	void         *pParam5;		//����5
	unsigned int iTimeBak;		//ʱ�� ���뼶��
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
	//ֱ��ǳ��������
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
	unsigned int m_iThreadCount;		//��ǰ�߳�����
	unsigned int m_iLessThreadCount;	//����߳�����
	unsigned int m_iAddThreadCount;		//�����߳�����
	unsigned int m_iCusThread;
	std::vector<CThread *> m_ThreadVec;	//�߳�
	CTimeProcess *m_TimeProcess;
};

class CThread
{
public:
	CThread(std::function<void(void*)>);
	~CThread();

	//�̺߳�����Ĭ��ִ�к���
	virtual void Run();
	virtual void Start();
	virtual void DispatchTaskMsg(stThreadTask);
	virtual void AddTask(stThreadTask task);

	DEFINE_MUTEX
protected:
	std::deque<stThreadTask> m_taskDeque;			//�������
	std::thread				*m_pThread;				//���浱ǰ���߳�ָ��
	bool					 m_bStart;				//�߳�״̬  �Ƿ���
	int						 m_iTaskCount;			//��������
	std::function<void(void*)> m_pFun;		        //�̻߳ص�
};

//���׶�ʱ��
class CTimeProcess : public CThread
{
public:
	CTimeProcess(unsigned int iSleepTime, std::function<void(void*)> pFunc);
	~CTimeProcess();

	virtual void Run();//��д�̺߳���
	virtual void AddTask(stThreadTask);
	void ClearTimer(unsigned int);
	int  GetObjTime(unsigned int);
		
	DEFINE_MUTEX
private:
	std::list<stTimerTask> m_taskList;
	unsigned int m_iSleepTime;
};