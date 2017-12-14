#pragma once

class CCpuSmooth
{
	typedef struct __CPU_TIME
	{
		FILETIME	ftIdleTime;
		FILETIME	ftKernelTime;
		FILETIME	ftUserTime;

		__CPU_TIME()
		{
			memset(&ftIdleTime, 0, sizeof(FILETIME));
			memset(&ftKernelTime, 0, sizeof(FILETIME));
			memset(&ftUserTime, 0, sizeof(FILETIME));
		}

		LONGLONG IdleTimeDiff(FILETIME preidle)
		{
			LARGE_INTEGER llPre, llCur;
			llPre.HighPart = preidle.dwHighDateTime;
			llPre.LowPart = preidle.dwLowDateTime;

			llCur.HighPart = ftIdleTime.dwHighDateTime;
			llCur.LowPart = ftIdleTime.dwLowDateTime;

			return (llCur.QuadPart - llPre.QuadPart);
		}

		LONGLONG KernelTimeDiff(FILETIME preKernel)
		{
			LARGE_INTEGER llPre, llCur;
			llPre.HighPart = preKernel.dwHighDateTime;
			llPre.LowPart = preKernel.dwLowDateTime;

			llCur.HighPart = ftKernelTime.dwHighDateTime;
			llCur.LowPart = ftKernelTime.dwLowDateTime;

			return (llCur.QuadPart - llPre.QuadPart);
		}

		LONGLONG UserTimeDiff(FILETIME preUser)
		{
			LARGE_INTEGER llPre, llCur;
			llPre.HighPart = preUser.dwHighDateTime;
			llPre.LowPart = preUser.dwLowDateTime;

			llCur.HighPart = ftUserTime.dwHighDateTime;
			llCur.LowPart = ftUserTime.dwLowDateTime;

			return (llCur.QuadPart - llPre.QuadPart);
		}
	}CPU_TIME, *PCPU_TIME;

public:
	CCpuSmooth() 
	{
		GetSystemTimes(&m_PreCpuTime.ftIdleTime, &m_PreCpuTime.ftKernelTime, &m_PreCpuTime.ftUserTime);
		GetSystemTimes(&m_PreCpuTimeForProc.ftIdleTime, &m_PreCpuTimeForProc.ftKernelTime, &m_PreCpuTimeForProc.ftUserTime);
	}
	~CCpuSmooth() {}

	void Sleep(DWORD dwMillSecond)
	{
		::Sleep(dwMillSecond);
	}

	// 总CPU百分比超过dwPercent，返回TRUE
	BOOL IsCpuHigh(DWORD dwPercent)
	{
		BOOL bHigh = FALSE;

		do
		{
			CPU_TIME cpuTime;
			if (!GetSystemTimes(&cpuTime.ftIdleTime, &cpuTime.ftKernelTime, &cpuTime.ftUserTime))
			{
				break;
			}
			
			LONGLONG idle = cpuTime.IdleTimeDiff(m_PreCpuTime.ftIdleTime);
			LONGLONG kernel = cpuTime.IdleTimeDiff(m_PreCpuTime.ftKernelTime);
			LONGLONG user = cpuTime.IdleTimeDiff(m_PreCpuTime.ftUserTime);

			if (kernel + user == 0)
			{
				break;
			}

			//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
			LONGLONG cpu = (kernel + user - idle) * 100 / (kernel + user);

			bHigh = (cpu > dwPercent);

			m_PreCpuTime = cpuTime;

		} while (FALSE);
		
		return bHigh;
	}

	// hProcess进程的CPU百分比超过dwPercent，返回TRUE
	// hProcess为NULL，返回当前进程
	BOOL IsProcCpuHigh(HANDLE hProcess, DWORD dwPercent)
	{
		BOOL bHigh = FALSE;

		do
		{
			if (NULL == hProcess)
			{
				hProcess = GetCurrentProcess();
			}

			if (NULL == hProcess)
			{
				break;
			}

			CPU_TIME cpuSysTime;
			if (!GetSystemTimes(&cpuSysTime.ftIdleTime, &cpuSysTime.ftKernelTime, &cpuSysTime.ftUserTime))
			{
				break;
			}

			// 周期
			LONGLONG lastToNow = cpuSysTime.KernelTimeDiff(m_PreCpuTimeForProc.ftKernelTime) + cpuSysTime.UserTimeDiff(m_PreCpuTimeForProc.ftUserTime);

			// 进程消耗的CPU时间 = 进程消耗的内核态时间 + 进程消耗的用户态时间，即 costTime = kernelTime + UserTime
			// 进程的CPU占用率 = 进程消耗的CPU时间 / 刷新周期
			CPU_TIME cpuTime;
			GetProcessTimes(hProcess, NULL, NULL, &cpuTime.ftKernelTime, &cpuTime.ftUserTime);
			
			if (m_mapPreProcCpuTimes.find(hProcess) != m_mapPreProcCpuTimes.end())
			{
				LONGLONG kernel = cpuTime.IdleTimeDiff(m_PreCpuTime.ftKernelTime);
				LONGLONG user = cpuTime.IdleTimeDiff(m_PreCpuTime.ftUserTime);
				LONGLONG cpu = (kernel + user) * 100 / lastToNow;

				bHigh = (cpu > dwPercent);
			}

			m_mapPreProcCpuTimes.insert(std::pair<HANDLE, CPU_TIME>(hProcess, cpuTime));

		} while (FALSE);
		
		return bHigh;
	}

private:
	CPU_TIME	m_PreCpuTime;

	// 进程的
	std::map<HANDLE, CPU_TIME>	m_mapPreProcCpuTimes;
	CPU_TIME	m_PreCpuTimeForProc;
};

