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

#define REG_PASSWORD_OLD	L"Password"

void CMigration::Migrate()
{
	EncryptPassword();
	StoreInstalledVersion();
	InitialiseNewSettings();
}

void CMigration::EncryptPassword()
{
	CSettings vSettings(TRUE);

	wstring szPassword;

	if (vSettings.GetValue(REG_PASSWORD_OLD, szPassword))
	{
		vSettings.SetGooglePassword(szPassword);

		vSettings.DeleteValue(REG_PASSWORD_OLD);
	}
}

void CMigration::StoreInstalledVersion()
{
	CSettings vSettings(TRUE);

	wstring szVersion;

	if (!vSettings.GetInstalledVersion(szVersion))
	{
		vSettings.SetInstalledVersion(CVersion::GetAppVersion());
	}
}

void CMigration::InitialiseNewSettings()
{
	CSettings vSettings(TRUE);

	BOOL fValue;

	if (!vSettings.GetCollectStatistics(fValue))
	{
		vSettings.SetCollectStatistics(TRUE);
	}

	if (!vSettings.GetUseGenericUnicodeFont(fValue))
	{
		vSettings.SetUseGenericUnicodeFont(TRUE);
	}
}
