#include "include.h"

#define PROCESS_WAIT_TIMEOUT	5000
#define DELETE_WAIT		1000
#define DELETE_RETRY		10

LPWSTR g_szBasePath = NULL;
DWORD g_dwProcessId = 0;
LPWSTR g_szExecFilename = NULL;
LPWSTR g_szUpdatePath = NULL;

static BOOL ExtractParameters(LPWSTR lpCmdLine);
static LPWSTR GetNextToken(LPCWSTR lpCmdLine, LPDWORD lpOffset);
static LPWSTR GetNextStringToken(LPCWSTR lpCmdLine, LPDWORD lpOffset);
static LPWSTR GetNextNonStringToken(LPCWSTR lpCmdLine, LPDWORD lpOffset);
static BOOL WaitForProcessEnd();
static BOOL PerformCopy();
static void Restart();
static BOOL CopyRecursive(LPCWSTR szSourceSubPath, LPCWSTR szDestSubPath);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	BOOL fResult;

	if (!ExtractParameters(lpCmdLine))
	{
		return -1;
	}

	fResult = WaitForProcessEnd();

	if (fResult)
	{
		fResult = PerformCopy();
	}

	Restart();

	return fResult ? 0 : -2;
}

static BOOL ExtractParameters(LPWSTR lpCmdLine)
{
	DWORD dwSize;
	LPWSTR szToken;
	DWORD dwOffset;
	LPWSTR szCurrentToken;

	// Get the current working directory, the base path of the installation.

	dwSize = GetCurrentDirectory(0, NULL);

	g_szBasePath = (LPWSTR)malloc(sizeof(WCHAR) * dwSize + 2);

	GetCurrentDirectory(dwSize, g_szBasePath);

	if (*(g_szBasePath + wcslen(g_szBasePath) - 1) != L'\\')
	{
		wcscat(g_szBasePath, L"\\");
	}

	// Parse the commandline.

	dwOffset = 0;

	szCurrentToken = NULL;

	while (( szToken = GetNextToken(lpCmdLine, &dwOffset) ))
	{
		if (szCurrentToken == NULL)
		{
			szCurrentToken = szToken;
		}
		else
		{
			if (_wcsicmp(szCurrentToken, L"/pid") == 0)
			{
				g_dwProcessId = _wtol(szToken);

				free(szToken);
			}
			else if (_wcsicmp(szCurrentToken, L"/path") == 0)
			{
				g_szUpdatePath = szToken;
			}
			else if (_wcsicmp(szCurrentToken, L"/exec") == 0)
			{
				g_szExecFilename = szToken;
			}
			else
			{
				return FALSE;
			}

			free(szCurrentToken);

			szCurrentToken = NULL;
		}
	}

	return g_dwProcessId != 0 && g_szUpdatePath != NULL && g_szExecFilename != NULL;
}

static LPWSTR GetNextToken(LPCWSTR lpCmdLine, LPDWORD lpOffset)
{
	LPCWSTR szOffset = lpCmdLine + *lpOffset;

	while (*szOffset && iswspace(*szOffset))
	{
		szOffset++;
	}

	if (!*szOffset)
	{
		return NULL;
	}

	*lpOffset = szOffset - lpCmdLine;

	if (*szOffset == L'\"')
	{
		return GetNextStringToken(lpCmdLine, lpOffset);
	}
	else
	{
		return GetNextNonStringToken(lpCmdLine, lpOffset);
	}
}

static LPWSTR GetNextStringToken(LPCWSTR lpCmdLine, LPDWORD lpOffset)
{
	LPCWSTR szOffset = lpCmdLine + *lpOffset + 1;
	LPCWSTR szEnd = wcschr(szOffset, L'\"');
	LPWSTR szResult;

	if (szEnd == NULL)
	{
		return NULL;
	}

	szResult = (LPWSTR)malloc(sizeof(WCHAR) * (szEnd - szOffset + 1));

	szResult = wcsncpy(szResult, szOffset, szEnd - szOffset);

	szResult[szEnd - szOffset] = L'\0';

	*lpOffset = (szEnd + 1) - lpCmdLine;

	return szResult;
}

static LPWSTR GetNextNonStringToken(LPCWSTR lpCmdLine, LPDWORD lpOffset)
{
	LPCWSTR szOffset = lpCmdLine + *lpOffset;
	LPCWSTR szEnd = szOffset;
	LPWSTR szResult;

	while (*szEnd && !iswspace(*szEnd))
	{
		szEnd++;
	}

	szResult = (LPWSTR)malloc(sizeof(WCHAR) * (szEnd - szOffset + 1));

	szResult = wcsncpy(szResult, szOffset, szEnd - szOffset);

	szResult[szEnd - szOffset] = L'\0';

	*lpOffset = szEnd - lpCmdLine;

	return szResult;
}

static BOOL WaitForProcessEnd()
{
	HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, g_dwProcessId);

	if (hProcess == NULL)
	{
		hProcess = OpenProcess(SYNCHRONIZE, FALSE, g_dwProcessId);
	}

	if (hProcess != NULL)
	{
		if (WaitForSingleObject(hProcess, PROCESS_WAIT_TIMEOUT) != WAIT_OBJECT_0)
		{
			TerminateProcess(hProcess, 0xFFFFFFFF);
		}

		CloseHandle(hProcess);
	}

	return TRUE;
}

static BOOL PerformCopy()
{
	return CopyRecursive(g_szUpdatePath, L".");
}

static BOOL CopyRecursive(LPCWSTR szSourceSubPath, LPCWSTR szDestSubPath)
{
	WCHAR szSourcePath[MAX_PATH];
	WCHAR szDestPath[MAX_PATH];
	WCHAR szSearchPath[MAX_PATH];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	BOOL fSuccess;

	wcscpy(szSourcePath, g_szBasePath);
	wcscat(szSourcePath, szSourceSubPath);
	wcscat(szSourcePath, L"\\");

	wcscpy(szDestPath, g_szBasePath);
	wcscat(szDestPath, szDestSubPath);
	wcscat(szDestPath, L"\\");

	wcscpy(szSearchPath, szSourcePath);
	wcscat(szSearchPath, L"*");

	if (!(GetFileAttributes(szDestPath) & FILE_ATTRIBUTE_DIRECTORY))
	{
		CreateDirectory(szDestPath, NULL);
	}

	hFind = FindFirstFile(szSearchPath, &wfd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	fSuccess = FALSE;
	
	do
	{
		if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0)
		{
			continue;
		}

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			WCHAR szNewSourceSubPath[MAX_PATH];
			WCHAR szNewDestSubPath[MAX_PATH];

			wcscpy(szNewSourceSubPath, szSourceSubPath);
			wcscat(szNewSourceSubPath, L"\\");
			wcscat(szNewSourceSubPath, wfd.cFileName);

			wcscpy(szNewDestSubPath, szDestSubPath);
			wcscat(szNewDestSubPath, L"\\");
			wcscat(szNewDestSubPath, wfd.cFileName);

			if (!CopyRecursive(szNewSourceSubPath, szNewDestSubPath))
			{
				goto __end;
			}
		}
		else
		{
			WCHAR szSourceFile[MAX_PATH];
			WCHAR szDestFile[MAX_PATH];

			wcscpy(szSourceFile, szSourcePath);
			wcscat(szSourceFile, wfd.cFileName);

			wcscpy(szDestFile, szDestPath);
			wcscat(szDestFile, wfd.cFileName);

			// First try to delete the existing file.

			if (GetFileAttributes(szDestFile) != INVALID_FILE_ATTRIBUTES)
			{
				INT i;

				for (i = 0; i < DELETE_RETRY; i++)
				{
					if (DeleteFile(szDestFile))
					{
						break;
					}

					Sleep(DELETE_WAIT);
				}
			}

			if (!MoveFileEx(szSourceFile, szDestFile, MOVEFILE_COPY_ALLOWED))
			{
				goto __end;
			}
		}
	}
	while (FindNextFile(hFind, &wfd) != 0);

	fSuccess = GetLastError() == ERROR_NO_MORE_FILES;

__end:
	FindClose(hFind);

	if (!RemoveDirectory(szSourcePath))
	{
		fSuccess = FALSE;
	}

	return fSuccess;
}

static void Restart()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	WCHAR szExecPath[MAX_PATH];

	wcscpy(szExecPath, g_szBasePath);
	wcscat(szExecPath, g_szExecFilename);

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(PROCESS_INFORMATION);

	CreateProcess(
		szExecPath,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		NULL,
		&si,
		&pi);
}
