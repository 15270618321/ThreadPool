#include <afx.h>
#include <exception>
#include "CThreadPool.h"


CThreadPool::CThreadPool(unsigned int iThreadCount, unsigned int iAddThreadCount, std::function<void(void*)> callBack)
{
	m_iLessThreadCount = iThreadCount;
	m_iAddThreadCount = iAddThreadCount;

	for (int i = 0; i < iThreadCount; i++)
	{
		CThread *pThread = NULL;
		try
		{
			pThread = new CThread(callBack);
			pThread->Start();
		}
		catch (CException* e)
		{
			printf("CThreadPool error \n");
		}
		
		if (NULL != pThread)
		{
			m_ThreadVec.push_back(pThread);
			m_iThreadCount++;
		}
	}

	//初始化计时器
	std::function<void(void*)> pFuncTemp = std::bind(&CThreadPool::HandleMsg, this, std::placeholders::_1);
	m_TimeProcess = new CTimeProcess(100, pFuncTemp);
	m_TimeProcess->Start();
	m_iCusThread = 0;
}

CThreadPool::~CThreadPool()
{
	std::vector<CThread*>::iterator itBegin = m_ThreadVec.begin();
	std::vector<CThread*>::iterator itEnd = m_ThreadVec.end();

	for (; itBegin != itEnd; itBegin++)
	{
		if (NULL == *itBegin)
		{
			continue;
		}

		delete *itBegin;
	}

	m_iLessThreadCount = 0;
	m_iAddThreadCount = 0;
	m_iThreadCount = 0;

	if (NULL != m_TimeProcess)
	{
		delete m_TimeProcess;
	}
}

void CThreadPool::AddTask(stThreadTask task)
{
	m_TimeProcess->AddTask(task);
}

void CThreadPool::HandleMsg(void* task)
{
	stThreadTask *pTask = (stThreadTask *)task;
	//分配任务
	SAFE_LOCK
	if (m_iCusThread >= m_iThreadCount)
	{
		m_iCusThread = 0;
	}

	if (0 == m_iThreadCount)
	{
		SAFE_UNLOCK;
		return;
	}

	m_ThreadVec[m_iCusThread++]->AddTask(*pTask);
	SAFE_UNLOCK;
}


void ThreadRun(CThread *pThread)
{
	if (NULL == pThread)
	{
		return;
	}
	pThread->Run();
}


CThread::CThread(std::function<void(void*)> pFun)
{
	m_pFun = pFun;
}

CThread::~CThread()
{
	if (NULL != m_pThread)
	{
		delete m_pThread;
	}

	m_pThread = NULL;
}

void CThread::Start()
{
	m_bStart = true;
	m_pThread = new std::thread(ThreadRun, this);
	m_pThread->detach();
}

void CThread::Run()
{
	while (m_bStart)
	{
		SAFE_LOCK
		if (m_taskDeque.empty())
		{
			SAFE_UNLOCK;
			Sleep(100);
		}
		else
		{
			stThreadTask task = m_taskDeque.front();
			m_taskDeque.pop_front();
			m_iTaskCount--;
			SAFE_UNLOCK
			DispatchTaskMsg(task);
		}
	}
}

void CThread::AddTask(stThreadTask task)
{
	SAFE_LOCK
	m_taskDeque.push_back(task);
	m_iTaskCount++;
	SAFE_UNLOCK
}

void CThread::DispatchTaskMsg(stThreadTask task)
{
	SAFE_LOCK
	if (NULL != m_pFun)
	{
		this->m_pFun((void *)&task);
	}
	SAFE_UNLOCK;
}

CTimeProcess::CTimeProcess(unsigned int iSleepTime, std::function<void(void*)> pFunc) :CThread(pFunc)
{
	m_iSleepTime = iSleepTime;
	m_taskList.clear();
}

CTimeProcess::~CTimeProcess()
{

}

void CTimeProcess::Run()
{
	while (true)
	{
		SAFE_LOCK;
		if (m_taskList.empty())
		{
			SAFE_UNLOCK;
			Sleep(m_iSleepTime);
			
		}
		std::list<stTimerTask>::iterator itBegin = m_taskList.begin();
		std::list<stTimerTask>::iterator itEnd = m_taskList.end();

		for (; itBegin != itEnd;)
		{
			if (itBegin->iTime > m_iSleepTime)
			{
				itBegin->iTime -= m_iSleepTime;
				itBegin++;
			}
			else
			{
				stThreadTask taskTemp;
				memcpy(&taskTemp, &*itBegin, sizeof(taskTemp));
				SAFE_UNLOCK;
				this->DispatchTaskMsg(taskTemp);
				SAFE_LOCK;
				if (Loop == itBegin->eLoop)
				{
					itBegin->iTime = itBegin->iTimeBak;
				}
				else
				{	
					m_taskList.erase(itBegin++);
				}
			}
		}

		SAFE_UNLOCK;
		Sleep(m_iSleepTime);
	}
}

void CTimeProcess::AddTask(stThreadTask task)
{
	stTimerTask taskTemp;
	memcpy(&taskTemp, &task, sizeof(task));
	taskTemp.iTimeBak = taskTemp.iTime;
	SAFE_LOCK
	m_taskList.push_back(taskTemp);
	SAFE_UNLOCK;
}

void CTimeProcess::ClearTimer(unsigned int iObjId)
{
	SAFE_LOCK
	std::list<stTimerTask>::iterator itBegin = m_taskList.begin();
	std::list<stTimerTask>::iterator itEnd = m_taskList.end();

	for (; itBegin != itEnd; itBegin++)
	{
		if (itBegin->iObjId == iObjId)
		{
			m_taskList.erase(itBegin++);
		}
		else
		{
			itBegin++;
		}
	}
	SAFE_UNLOCK
}

int CTimeProcess::GetObjTime(unsigned int iObjId)
{
	SAFE_LOCK;
	std::list<stTimerTask>::iterator itBegin = m_taskList.begin();
	std::list<stTimerTask>::iterator itEnd = m_taskList.end();

	for (; itBegin != itEnd; itBegin++)
	{
		if (itBegin->iObjId == iObjId)
		{
			return itBegin->iTime;
		}
		else
		{
			itBegin++;
		}
	}
	SAFE_UNLOCK;
	return -1;
}