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

#include "stdafx.h"
#include "include.h"

HINSTANCE g_hInstance;

static INT ExecuteApplication(HINSTANCE hInstance, LPWSTR lpCmdLine);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG

	// Comparing between this MemState ignores memory allocated by
	// static member variables.

	_CrtMemState vMemState;

	_CrtMemCheckpoint(&vMemState);

#endif
	CoInitialize(NULL);

	//
	// Get the global mutex to prevent multiple instances.
	//

	HANDLE hMutex = CreateMutex(NULL, FALSE, GLOBAL_MUTEX);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return -1;
	}

	//
	// Perform automatic migration routines for version updates.
	//

	CMigration::Migrate();

	//
	// Check for a new version.
	//

	INT nResult;

	if (CVersion::NewVersionAvailable())
	{
		nResult = 0;
	}
	else
	{
		//
		// Start the application.
		//

		nResult = ExecuteApplication(hInstance, lpCmdLine);
	}

	//
	// Get the new version when one comes available.
	//

	if (CVersion::GetNewVersionAvailable())
	{
		if (!CVersion::PerformUpdate())
		{
			CloseHandle(hMutex);

			CNotifierApp::Restart();
		}
	}

	CloseHandle(hMutex);

	CoUninitialize();

#ifdef _DEBUG

	_CrtMemState vNewMemState;
	_CrtMemState vDifference;

	_CrtMemCheckpoint(&vNewMemState);

	if (_CrtMemDifference(&vDifference, &vMemState, &vNewMemState))
	{
		_CrtMemDumpAllObjectsSince(&vMemState);

		MessageBox(
			NULL,
			L"Memory leak detected",
			L"Google Wave Notifier",
			MB_OK | MB_ICONERROR);
	}

#endif // _DEBUG

	return nResult;
	
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
}

static INT ExecuteApplication(HINSTANCE hInstance, LPWSTR lpCmdLine)
{
	CNotifierApp * lpApp = new CNotifierApp(hInstance, lpCmdLine);

	if (!lpApp->Initialise())
	{
		delete lpApp;

		return -1;
	}

	int nResult = lpApp->Execute();

	delete lpApp;

	return nResult;
}
