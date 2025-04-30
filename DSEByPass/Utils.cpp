#include "pch.h"
#include "Utils.h"
#include"framework.h"
#include"dia2.h"

BOOL Utils::EnableDebugPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;

	// 获取当前进程令牌
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return FALSE;
	}

	// 查找调试权限的LUID
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid)) {
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// 调整权限
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}

BOOL Is32BitProcess()
{
#if defined(_WIN64)
	return FALSE;  // 在64位编译环境下肯定是64位程序
#else
	// 检查是否运行在WOW64下（32位程序运行在64位系统）
	BOOL bIsWow64 = FALSE;
	IsWow64Process(GetCurrentProcess(), &bIsWow64);
	return bIsWow64 ? TRUE : FALSE;  // 如果在WOW64下运行则是32位程序
#endif
}


typedef HRESULT(__stdcall* DllGetClassObject_t)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
CString Utils::GetSymbolUrlString()
{
	PVOID OldValue = NULL;
	bool is32 = Is32BitProcess();

	if (is32)
	{
		
		Wow64DisableWow64FsRedirection(&OldValue);
	}

	//获取system的路径
	CString strPath;
	wchar_t Temp[MAX_PATH];
	GetSystemDirectory(Temp, MAX_PATH);
	strPath = Temp; 
	strPath.Append(L"\\ci.dll");


	
	HMODULE hDiaDll = LoadLibrary(L"msdia140.dll"); // 根据你的DIA DLL版本调整
	if (!hDiaDll) return L"";

	DllGetClassObject_t pDllGetClassObject = (DllGetClassObject_t)GetProcAddress(hDiaDll, "DllGetClassObject");
	if (!pDllGetClassObject)
	{
		FreeLibrary(hDiaDll);
		return L"";
	}

	CComPtr<IClassFactory> pClassFactory;
	if (FAILED(pDllGetClassObject(CLSID_DiaSource, IID_IClassFactory, (void**)&pClassFactory)))
	{
		FreeLibrary(hDiaDll);
		return L"";
	}

	CComPtr<IDiaDataSource> pSource;
	if (FAILED(pClassFactory->CreateInstance(NULL, IID_IDiaDataSource, (void**)&pSource)))
	{
		FreeLibrary(hDiaDll);
		return L"";
	}

	// 加载PE文件
	if (FAILED(pSource->loadDataForExe(CComBSTR(strPath.GetBuffer()), NULL, NULL)))
	{
		FreeLibrary(hDiaDll);
		return L"";
	}

	CComPtr<IDiaSession> pSession;
	if (FAILED(pSource->openSession(&pSession)))
	{
		FreeLibrary(hDiaDll);
		return L"";
	}

	CComPtr<IDiaSymbol> pGlobal;
	if (FAILED(pSession->get_globalScope(&pGlobal)))
	{
		FreeLibrary(hDiaDll);
		return L"";
	}

	// 获取PDB信息
	DWORD signature, age;
	BSTR pdbPath;
	if (SUCCEEDED(pGlobal->get_signature(&signature)) &&
		SUCCEEDED(pGlobal->get_age(&age)) &&
		SUCCEEDED(pGlobal->get_symbolsFileName(&pdbPath)))
	{
		const wchar_t* pdbName = wcsrchr(pdbPath, L'\\') + 1;
		wchar_t url[1024];
		swprintf_s(url, L"https://msdl.microsoft.com/download/symbols/%s/%08X%x/%s",
			pdbName, signature, age, pdbName);
		strPath = url;
		SysFreeString(pdbPath);
		FreeLibrary(hDiaDll);
		return strPath;
	}

	FreeLibrary(hDiaDll);


	if (is32)
	{

		Wow64RevertWow64FsRedirection(OldValue);
	}

	
	return strPath;
}

