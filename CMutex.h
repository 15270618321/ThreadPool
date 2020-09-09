#include <iostream>
#include <windows.h>

//∫Í∂®“Â

#define DEFINE_MUTEX \
	private:\
		CMutex m_Lock;

#define SAFE_LOCK \
		m_Lock.Lock();

#define SAFE_UNLOCK \
		m_Lock.Unlock();

#define SAFE_RELEASE_LOCK \
	    m_Lock.Release();

class CMutex
{
public:
	CMutex();
	~CMutex();
	
	CMutex(const CMutex &mutex);
	CMutex& operator=(const CMutex &mutex);

	void Lock();
	void Unlock();
	void Release();
private:
	HANDLE					m_hMutex;
};