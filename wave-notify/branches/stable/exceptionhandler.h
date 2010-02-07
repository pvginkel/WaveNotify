#ifndef _INC_EXCEPTIONHANDLER
#define _INC_EXCEPTIONHANDLER

class CExceptionHandler
{
public:
	static void Register();
	static void Unregister();
	static void SetFailureReason(LPCSTR szReason, LPCSTR szLocation);

private:
	static LPSTR GetVersionInformation();
};

#endif
