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

#ifndef _INC_BOOTSTRAP
#define _INC_BOOTSTRAP

#pragma once

class CBootstrap
{
private:
	TStringStringMap m_vModules;
	wstring m_szPath;

public:
	CBootstrap() { }
	virtual ~CBootstrap() { }

	INT Execute(INT nCmdShow);

private:
	BOOL SwitchToRunning();
	BOOL InitializePath();
	BOOL DetectModules();
	void UpdateEnvironment();
	BOOL BootApplication(wstring szExecutable, INT nCmdShow);
	void AddModulePath(wstring szFilename);
	BOOL IsVersion(wstring szVersion);
	INT CompareVersion(wstring szLeft, wstring szRight);
	void UpdatePath();
	void RegisterModules();
};

#endif // _INC_BOOTSTRAP
