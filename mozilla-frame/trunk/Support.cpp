#include "stdafx.h"
#include "include.h"

typedef HRESULT (__stdcall *pDllGetClassObject)(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);

HRESULT WINAPI CoCreateInstanceFromFile(LPCWSTR szFilename, REFCLSID rclsid, LPUNKNOWN pUnkOuter, REFIID riid, LPVOID FAR* ppv)
{
	HRESULT hr = REGDB_E_KEYMISSING;

	// We purposefully do not unload the library. It will be required for the
	// full duration of the application anyway. And we have no trigger on when
	// to actually do unload the module.

	HMODULE hModule = ::LoadLibrary(szFilename);

	if (hModule == NULL)
	{
		return hr;
	}

	pDllGetClassObject lpGetClassObject = (pDllGetClassObject)GetProcAddress(hModule, "DllGetClassObject");

	if (lpGetClassObject == NULL)
	{
		FreeLibrary(hModule);

		return hr;
	}

	CComPtr<IClassFactory> lpClassFactory;

	hr = lpGetClassObject(rclsid, IID_IClassFactory, (void **)&lpClassFactory);

	if (!SUCCEEDED(hr))
	{
		return hr;
	}

	return lpClassFactory->CreateInstance(pUnkOuter, riid, ppv);
}
