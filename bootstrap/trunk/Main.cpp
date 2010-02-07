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
#include "bootstrap.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG

	// Comparing between this MemState ignores memory allocated by
	// static member variables.

	_CrtMemState vMemState;

	_CrtMemCheckpoint(&vMemState);

#endif

	int nResult;

// (( scope ))
{
	CBootstrap vBootstrap;

	nResult = vBootstrap.Execute(nCmdShow);
}

#ifdef _DEBUG

	_CrtMemState vNewMemState;
	_CrtMemState vDifference;

	_CrtMemCheckpoint(&vNewMemState);

	if (_CrtMemDifference(&vDifference, &vMemState, &vNewMemState))
	{
		_CrtMemDumpAllObjectsSince(&vMemState);

		MessageBox(
			NULL,
			L"Memory leaks detected",
			L"Google Wave Notifier",
			MB_OK | MB_ICONERROR);
	}

#endif // _DEBUG

	return nResult;
	
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
}
