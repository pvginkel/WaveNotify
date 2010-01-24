#include "stdafx.h"
#include "include.h"

#define HASH_POOL			L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"

INT Rand(INT nMin, INT nMax)
{
	ASSERT(nMin <= nMax);

	static BOOL fInitialised = FALSE;

	if (!fInitialised)
	{
		fInitialised = TRUE;

		srand(GetTickCount());
	}

	INT nRand = 0;
	INT nRandMax = 0;
	INT nCount = INT(ceil(DOUBLE(nMax - nMin) / DOUBLE(RAND_MAX)));

	if (nCount < 1)
	{
		nCount = 1;
	}

	for (INT i = 0; i < nCount; i++)
	{
		nRand += rand();
		nRandMax += RAND_MAX;
	}

	INT nResult = nMin + INT((DOUBLE(nRand) / DOUBLE(nRandMax)) * DOUBLE(nMax - nMin + 1));

	// Force result inside nMin/nMax because of DOUBLE rounding errors

	if (nResult < nMin)
	{
		return nMin;
	}
	else if (nResult > nMax)
	{
		return nMax;
	}
	else
	{
		return nResult;
	}
}

wstring CreateHash(DWORD dwLength, LPCWSTR szPool)
{
	ASSERT(dwLength > 0);

	if (szPool == NULL)
	{
		szPool = HASH_POOL;
	}

	DWORD cbPool = wcslen(szPool);

	wstring szResult;

	szResult.reserve(dwLength);

	for (DWORD i = 0; i < dwLength; i++)
	{
		szResult += *(szPool + Rand(0, cbPool - 1));
	}

	return szResult;
}
