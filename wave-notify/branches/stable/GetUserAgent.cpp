#include "stdafx.h"
#include "include.h"

#include "support.h"
#include "version.h"

LPCWSTR GetUserAgent()
{
	static WCHAR szUserAgent[256] = L"";
	static BOOL fInitialised = FALSE;

	if (!fInitialised)
	{
		wsprintf(szUserAgent, L"net.sf.wave-notify/%s", CVersion::GetAppVersion().c_str());

		fInitialised = TRUE;
	}

	return szUserAgent;
}
