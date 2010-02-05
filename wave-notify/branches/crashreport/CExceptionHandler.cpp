#include "stdafx.h"
#include "include.h"
#include "exceptionhandler.h"
#include "settings.h"
#include "client\windows\handler\exception_handler.h"

static google_breakpad::ExceptionHandler * g_lpExceptionHandler = NULL;
static LPSTR g_szExtraInformation = NULL;

static bool MinidumpCallback(const wchar_t * szDumpPath, const wchar_t * szMinidumpID,
	void * lpContext, EXCEPTION_POINTERS * lpExceptionInfo,
	MDRawAssertionInfo * lpAssertion, bool fSucceeded);

void CExceptionHandler::Register()
{
// (( scope ))
{
	ASSERT(g_lpExceptionHandler == NULL);

	// Build our required extra information.

	g_szExtraInformation = GetVersionInformation();

	if (g_szExtraInformation == NULL)
	{
		goto __failed;
	}

	// Find the path to create the dump files in and ensure it exists.

	wstring szCrashDumpPath(GetDirname(GetModuleFileNameEx()) + L"\\" + CRASH_DUMP_PATH);

	SetEnvironmentVariable(L"MOZ_CRASHREPORTER_DATA_DIRECTORY", szCrashDumpPath.c_str());

	DWORD dwAttr = GetFileAttributes(szCrashDumpPath.c_str());

	if (dwAttr == INVALID_FILE_ATTRIBUTES)
	{
		BOOL fResult = CreateDirectory(szCrashDumpPath.c_str(), NULL);

		if (!fResult)
		{
			goto __failed;
		}
	}

	// Set the module name as the first restart parameter so the crash
	// reporter displays the restart button.

	SetEnvironmentVariable(L"MOZ_CRASHREPORTER_RESTART_ARG_0", GetModuleFileNameEx().c_str());

	// Create the exception handler.

	g_lpExceptionHandler = new google_breakpad::ExceptionHandler(
		GetDirname(GetModuleFileNameEx()).c_str(),
		NULL,
		MinidumpCallback,
		NULL,
		google_breakpad::ExceptionHandler::HANDLER_ALL
	);

	return;
}

__failed:
	if (g_szExtraInformation != NULL)
	{
		free(g_szExtraInformation);

		g_szExtraInformation = NULL;
	}
}

void CExceptionHandler::Unregister()
{
	if (g_lpExceptionHandler != NULL)
	{
		delete g_lpExceptionHandler;

		g_lpExceptionHandler = NULL;

		free(g_szExtraInformation);

		g_szExtraInformation = NULL;
	}
}

LPSTR CExceptionHandler::GetVersionInformation()
{
	LPSTR szResult = NULL;
	LPVOID lpData = NULL;

// (( scope ))
{
	stringstream szDetails;

	// Load the version information.

	wstring szFilename(GetModuleFileNameEx());

	DWORD dwHandle = 0;
	DWORD dwSize = GetFileVersionInfoSize(szFilename.c_str(), &dwHandle);

	if (dwSize == 0)
	{
		goto __end;
	}

	lpData = malloc(dwSize);

	if (!GetFileVersionInfo(szFilename.c_str(), dwHandle, dwSize, lpData))
	{
		goto __end;
	}

	// Get the product name.

	UINT uLen;
	LPCWSTR szData;

	if (!VerQueryValue(lpData, L"\\StringFileInfo\\040904b0\\ProductName", (LPVOID*)&szData, &uLen))
	{
		goto __end;
	}

	szDetails << "ProductName=" << ConvertToMultiByte(szData) << "\n";

	// Get the vendor.

	if (!VerQueryValue(lpData, L"\\StringFileInfo\\040904b0\\CompanyName", (LPVOID*)&szData, &uLen))
	{
		goto __end;
	}

	szDetails << "Vendor=" << ConvertToMultiByte(szData) << "\n";

	// Get the application version.

	VS_FIXEDFILEINFO * lpffi;

	if (!VerQueryValue(lpData, L"\\", (LPVOID *)&lpffi, &uLen))
	{
		goto __end;
	}

	ASSERT(lpffi != NULL);

	wstring szVersion(Format(
		L"%d.%d.%d.%d",
		(INT)HIWORD(lpffi->dwProductVersionMS),
		(INT)LOWORD(lpffi->dwProductVersionMS),
		(INT)HIWORD(lpffi->dwProductVersionLS),
		(INT)LOWORD(lpffi->dwProductVersionLS)));

	szDetails << "Version=" << ConvertToMultiByte(szVersion) << "\n";

	// Build the server URL.

	wstring szServerUrl(CRASH_SERVER_URL);

	wstring szCookie;

	if (CSettings(FALSE).GetStatisticsCookie(szCookie))
	{
		szServerUrl += L"?cookie=";
		szServerUrl += szCookie;
	}

	szDetails << "ServerURL=" << ConvertToMultiByte(szServerUrl) << "\n";

	szResult = _strdup(szDetails.str().c_str());
}

__end:
	if (lpData != NULL)
	{
		free(lpData);
	}

	return szResult;
}

void CExceptionHandler::SetFailureReason(LPCSTR szReason, LPCSTR szLocation)
{
	if (szReason == NULL)
	{
		return;
	}

	LPSTR szExtraInformation = GetVersionInformation();

	if (szExtraInformation == NULL)
	{
		return;
	}

	stringstream szDetails;

	szDetails << szExtraInformation;

	free(szExtraInformation);
	
	szDetails << "Message=" << szReason << "\n";

	if (szLocation != NULL)
	{
		szDetails << "Location=" << szLocation << "\n";
	}

	if (g_szExtraInformation != NULL)
	{
		free(g_szExtraInformation);
	}

	g_szExtraInformation = _strdup(szDetails.str().c_str());
}

static bool MinidumpCallback(const wchar_t * szDumpPath, const wchar_t * szMinidumpID,
	void * lpContext, EXCEPTION_POINTERS * lpExceptionInfo,
	MDRawAssertionInfo * lpAssertion, bool fSucceeded)
{
	// Resolve all necessary paths.

	WCHAR szDumpFileName[MAX_PATH];

	wcscpy(szDumpFileName, szDumpPath);
	wcscat(szDumpFileName, L"\\");
	wcscat(szDumpFileName, szMinidumpID);
	wcscat(szDumpFileName, L".dmp");

	WCHAR szCommandLine[MAX_PATH * 2 + 6];

	wcscpy(szCommandLine, L"\"");
	wcscat(szCommandLine, CRASH_REPORTER_PATH);
	wcscat(szCommandLine, L"\" \"");
	wcscat(szCommandLine, szDumpFileName);
	wcscat(szCommandLine, L"\"");

	// Write the extra data file.

	WCHAR szExtraFileName[MAX_PATH];

	wcscpy(szExtraFileName, szDumpPath);
	wcscat(szExtraFileName, L"\\");
	wcscat(szExtraFileName, szMinidumpID);
	wcscat(szExtraFileName, L".extra");

	HANDLE hFile = CreateFile(
		szExtraFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWritten;

		WriteFile(
			hFile,
			g_szExtraInformation,
			strlen(g_szExtraInformation),
			&dwWritten,
			NULL);

		CloseHandle(hFile);
	}

	// Start the crash reporter process.

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;

	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	BOOL fResult = CreateProcess(
		NULL,
		szCommandLine,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi);

	if (fResult)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	// We actually have crashed, so we can quit now.

	TerminateProcess(GetCurrentProcess(), 1);

	return fSucceeded;
}
