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

#ifndef _INC_APP
#define _INC_APP

#pragma once

class CApp
{
private:
	static CApp * m_lpInstance;
	HINSTANCE m_hInstance;
	wstring m_szCmdLine;

public:
	CApp(HINSTANCE hInstance, wstring szCmdLine);
	virtual ~CApp();

	virtual BOOL Initialise();
	virtual INT Execute();

	HINSTANCE GetInstance() const { return m_hInstance; }
	wstring GetCmdLine() const { return m_szCmdLine; }

	static CApp * Instance() { return m_lpInstance; }
};

#endif // _INC_APP
