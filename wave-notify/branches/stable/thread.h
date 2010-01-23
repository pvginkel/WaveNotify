/*
 * This file is part of Google Wave Notifier.
 *
 * Google Wave Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Google Wave Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INC_THREAD
#define _INC_THREAD

#pragma once

class CThread
{
private:
	HANDLE m_hThread;
	DWORD m_dwThreadId;

public:
	CThread(BOOL fSuspended = FALSE) {
		m_hThread = CreateThread(NULL, 0, CThread::ThreadProcCallback, this, fSuspended ? CREATE_SUSPENDED : 0, &m_dwThreadId);
		
		if (m_hThread == NULL)
			FAIL("Call to CreateThread failed");
	}
	virtual ~CThread() {
		Join();
		CloseHandle(m_hThread);
	}

	void Start() { Resume(); }
	void Suspend() { SuspendThread(m_hThread); }
	BOOL Resume() { return ResumeThread(m_hThread) != -1; }
	void Terminate(DWORD dwExitCode = 0) {
		TerminateThread(m_hThread, dwExitCode);
	}
	DWORD Join() {
		WaitForSingleObject(m_hThread, INFINITE);
		return GetExitCode();
	}
	DWORD GetExitCode() const {
		DWORD dwTmp;
		GetExitCodeThread(m_hThread, &dwTmp);
		return dwTmp;
	}

protected:
	virtual DWORD ThreadProc() = 0;
	void Exit(DWORD dwExitCode = 0) { ExitThread(dwExitCode); }

private:
	static DWORD WINAPI ThreadProcCallback(LPVOID lpParameter);
};

#endif // _INC_THREAD
