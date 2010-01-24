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

#ifndef _INC_ABOUTDIALOG
#define _INC_ABOUTDIALOG

#pragma once

#include "theming.h"

class CAboutDialog : public CThemedDialog
{
public:
	CAboutDialog() : CThemedDialog(IDD_ABOUT) { }

protected:
	INT_PTR DialogProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	INT_PTR OnInitDialog();
	INT_PTR OnCommand(WORD wCommand, WORD wNotifyMessage);
	void ExplodeVersions(TStringStringMap & vVersions, wstring szVersion);

};

#endif // _INC_ABOUTDIALOG
