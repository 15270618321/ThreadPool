#include "CMutex.h"


CMutex::CMutex()
{
	m_hMutex = CreateMutex(NULL, FALSE, NULL);
}

CMutex::~CMutex()
{
	if (m_hMutex)
	{
		CloseHandle(m_hMutex);
	}
}

CMutex::CMutex(const CMutex &mutex)
{
	//Ç³¿½±´
	m_hMutex = mutex.m_hMutex;
}

CMutex& CMutex::operator = (const CMutex &mutex)
{
	//Ç³¿½±´
	m_hMutex = mutex.m_hMutex;
	return *this;
}

void CMutex::Lock()
{
	WaitForSingleObject(m_hMutex, INFINITE);
}

void CMutex::Unlock()
{
	ReleaseMutex(m_hMutex);
}

void CMutex::Release()
{
	if (m_hMutex)
		CloseHandle(m_hMutex);
}