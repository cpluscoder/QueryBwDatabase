#include "StdAfx.h"
#include <sstream>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>
#include <boost/locale.hpp>
#include <boost/format.hpp>
#include <tinyxml.h>
#include "Utility.h"
#include <Winver.h>

#pragma comment(lib, "Version.lib")

using namespace std;

CUtility::CUtility(void)
{
}


CUtility::~CUtility(void)
{
}

void CUtility::AddNode(TiXmlNode* pParent, const string &strKey, const string &strValue)
{
	TiXmlElement *pNode = new TiXmlElement(strKey.c_str());
	TiXmlNode *pElement = pParent->LinkEndChild(pNode);
	TiXmlText *pText = new TiXmlText(strValue.c_str());
	pElement->LinkEndChild(pText);
}


void CUtility::AddNode(TiXmlNode* pParent, const string &strComment, const string &strKey, const string &strValue)
{
	TiXmlElement *pNode = new TiXmlElement(strKey.c_str());
	TiXmlNode *pElement = pParent->LinkEndChild(pNode);
	TiXmlText *pText = new TiXmlText(strValue.c_str());
	//TiXmlComment *pComment = new TiXmlComment(strComment.c_str());
//	pElement->c(pComment);
	pElement->LinkEndChild(pText);
}

bool CUtility::GetNodeText(TiXmlNode* pNode, string &strText)
{
	strText.clear();

	if(pNode != NULL)
	{
		TiXmlElement* pElement = pNode->ToElement();
		if(pElement != NULL)
		{
			const char *pszText = pElement->GetText();
			if(pszText != NULL)
			{
				strText = pszText;
				return true;
			}
		}
	}

	return false;
}

bool CUtility::SetNodeText(TiXmlNode* pNode, const string &strText)
{
	if(pNode != NULL)
	{
		TiXmlNode *pText = pNode->FirstChild();
		if(pText != NULL)
		{
			pText->SetValue(strText.c_str());
			return true;
		}
	}

	return false;
}

bool CUtility::GetAttributeText(TiXmlNode* pNode, const string &strAttributeName, string &strText)
{
	strText.clear();

	if(pNode != NULL)
	{
		TiXmlElement* pElement = pNode->ToElement();
		if(pElement != NULL)
		{
			const char *pszText = pElement->Attribute(strAttributeName.c_str());
			if(pszText != NULL)
			{
				strText = pszText;
				return true;
			}
		}
	}

	return false;
}

bool CUtility::SetAttributeText(TiXmlNode* pNode, const string &strAttributeName, const string &strText)
{
	if(pNode != NULL)
	{
		TiXmlElement* pElement = pNode->ToElement();
		if(pElement != NULL)
		{
			pElement->SetAttribute(strAttributeName.c_str(), strText.c_str());
		}
	}

	return false;
}

int CUtility::StringToInt(const string &str)
{
	return _ttoi(str.c_str());
}

string CUtility::IntToString(const int nValue)
{
	stringstream  ss;
	ss<<nValue;
	return ss.str();
}

void CUtility::Utf8ToGbk(string &strUtf8ToGbk)
{
	string const &to_encoding("GBK");
	string const &from_encoding("UTF-8");
	strUtf8ToGbk = boost::locale::conv::between(strUtf8ToGbk.c_str(), 
		strUtf8ToGbk.c_str() + strUtf8ToGbk.size(), 
		to_encoding, 
		from_encoding);
}

void CUtility::GbkToUtf8(string &GbkToUtf8)
{
	string const &to_encoding("UTF-8");
	string const &from_encoding("GBK");
	GbkToUtf8 = boost::locale::conv::between(GbkToUtf8.c_str(), 
		GbkToUtf8.c_str() + GbkToUtf8.size(), 
		to_encoding, 
		from_encoding);
}

CString CUtility::StringToCString(const string &str)
{
	return CString(str.c_str());
}

string CUtility::CStringToString(const CString &str)
{
	string strTemp = str;
	return strTemp;
}

CString CUtility::GetFileNameFromPath(const CString &str)
{
	CString strTmp("");
	int nLow = str.ReverseFind('\\');
	int nHigh = str.GetLength();
	strTmp = str.Right(nHigh-nLow-1);
	return strTmp;
}

void CUtility::GetAppFullPath(string &strFullPath)
{
	HINSTANCE hInstance = NULL;
	char szModulePath[MAX_PATH];
	::memset(&szModulePath, 0, MAX_PATH);
	::GetModuleFileName(NULL, szModulePath, MAX_PATH);
	char* pszTemp = (char *)0;
	pszTemp = ::_tcsrchr(szModulePath, (unsigned int)'\\');
	szModulePath[pszTemp - szModulePath + 1] = '\0';
	strFullPath = szModulePath;
}


bool CUtility::ReadFile(const string &strFileName, string &strReadData)
{
	FILE *pFile = NULL;
	::fopen_s(&pFile, strFileName.c_str(), "r");
	if(pFile == NULL)
	{
		return false;
	}

	string strUTF8;
	strUTF8.clear();
	char szBuf[1024];
	while(!feof(pFile))
	{
		::memset(szBuf, '\0', sizeof(szBuf));
		::fread(szBuf, sizeof(char), sizeof(szBuf) - 2, pFile);
		strUTF8 += szBuf;
	}
	::fclose(pFile);

	/// UTF-8 ----> ANSI
	Utf8ToGbk(strUTF8);
	strReadData = strUTF8;

	return true;
}

bool CUtility::WriteFile(const string &strFileName, const string &strWriteData, const char *szMode)
{
	FILE *pFile = NULL;
	::fopen_s(&pFile, strFileName.c_str(), szMode);
	if(pFile == NULL)
	{
		return false;
	}

	/// ANSI ----> UTF-8
	string strData = strWriteData;
	GbkToUtf8(strData);

	::fwrite(strData.c_str(), sizeof(char), strData.size(), pFile);

	::fclose(pFile);

	return true;
}

void CUtility::GetFileTime(const string &strDateTime, COleDateTime &dt)
{
	COleDateTime dateTime;
	if(dateTime.ParseDateTime(strDateTime.c_str()))
	{
		dt = dateTime;
		TRACE("%04d-%02d-%02d %02d:%02d:%02d\n", dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	}
	else
	{
		dt = COleDateTime::GetCurrentTime();
	}
}

bool CUtility::GetPathFromFileName(const string &strFileName, string &strPath)
{
	strPath.clear();
	size_t st = strFileName.rfind("\\");
	if(st == string::npos)
	{
		return false;
	}

	strPath = strFileName.substr(0, st);
	strPath.append("\\");

	return true;
}

CString CUtility::GetFileSuffix(const CString &strFileName)
{
	CString strSuffix;
	int nPos = strFileName.ReverseFind('.');

	if(nPos < 0)
	{
		return "";
	}

	strSuffix = strFileName.Right(strFileName.GetLength() - nPos);
	return strSuffix;
}

bool CUtility::FileTime2String(const FILETIME &tmFile, string &strDateTime)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	ptime pt = from_ftime<ptime>(tmFile);

	strDateTime.clear();

	//strDateTime = to_iso_extended_string(pt);
	strDateTime = to_iso_string(pt);

	return !strDateTime.empty();
}

bool CUtility::String2PosixTime(const string &strDateTime, boost::posix_time::ptime &dtDateTime)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	dtDateTime = from_iso_string(strDateTime);

	return true;
}

bool CUtility::PosixTime2String(const boost::posix_time::ptime &dtDateTime, string &strDateTime)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	strDateTime.clear();

	strDateTime = to_iso_string(dtDateTime);

	return !strDateTime.empty();
}

bool CUtility::StdTime2String(const std::time_t &tmSrc, string &strDateTime)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	tm dtTemp;
	if(0 == ::localtime_s(&dtTemp, &tmSrc))
	{
		//ptime dt = ptime_from_tm(dtTemp);

		//strDateTime = to_iso_string(dt);

		string strFormat = "%04d-%02d-%02d %02d:%02d:%02d";
		boost::format formatStr = boost::format(strFormat.c_str())
			% (dtTemp.tm_year + 1900) % (dtTemp.tm_mon + 1) % dtTemp.tm_mday
			% dtTemp.tm_hour % dtTemp.tm_min % dtTemp.tm_sec;

		strDateTime = formatStr.str();

		return !strDateTime.empty();
	}

	return false;
}

bool CUtility::GetOsVersion(string &strOsVersion)
{
	strOsVersion.clear();

	SYSTEM_INFO systemInfo;
	ZeroMemory(&systemInfo, sizeof(SYSTEM_INFO));
	typedef void (WINAPI *FN_GetNativeSystemInfo)(LPSYSTEM_INFO);
	FN_GetNativeSystemInfo pfnGetNativeSystemInfo = NULL;
	pfnGetNativeSystemInfo = (FN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if(NULL != pfnGetNativeSystemInfo)
	{
		pfnGetNativeSystemInfo(&systemInfo);
	}
	else
	{
		::GetSystemInfo(&systemInfo);
	}

	OSVERSIONINFOEX osVerInfo;
	ZeroMemory(&osVerInfo, sizeof(OSVERSIONINFOEX));
	osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!::GetVersionEx((OSVERSIONINFO *)&osVerInfo))
	{
		return false;
	}
	if(VER_PLATFORM_WIN32_NT != osVerInfo.dwPlatformId || osVerInfo.dwMajorVersion <= 4)
	{
		return false;
	}
	typedef BOOL (WINAPI *FN_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
	FN_GetProductInfo pfnGetProductInfo = NULL;
	pfnGetProductInfo = (FN_GetProductInfo)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
	if(pfnGetProductInfo == NULL)
	{
		return false;
	}

	DWORD dwType = 0;
	CString strTemp = "Microsoft ";
	if(osVerInfo.dwMajorVersion == 6)
	{
		if(osVerInfo.dwMinorVersion == 0)
		{
			if(osVerInfo.wProductType == VER_NT_WORKSTATION)
			{
				strTemp += "Windows Vista ";
			}
			else
			{
				strTemp += "Windows Server 2008 ";
			}
		}
		else if(osVerInfo.dwMinorVersion == 1)
		{
			if(osVerInfo.wProductType == VER_NT_WORKSTATION)
			{
				strTemp += "Windows 7 ";
			}
			else
			{
				strTemp += "Windows Server 2008 R2 ";
			}
		}

		pfnGetProductInfo(osVerInfo.dwMajorVersion, osVerInfo.dwMinorVersion, 0, 0, &dwType);
		switch(dwType)
		{
		case PRODUCT_ULTIMATE:
			{
				strTemp += "Ultimate Edition";
			}
			break;
		case PRODUCT_PROFESSIONAL:
			{
				strTemp += "Professional";
			}
			break;
		case PRODUCT_HOME_PREMIUM:
			{
				strTemp += "Home Premium Edition";
			}
			break;
		case PRODUCT_HOME_BASIC:
			{
				strTemp += "Home Basic Edition";
			}
			break;
		case PRODUCT_ENTERPRISE:
			{
				strTemp += "Enterprise Edition";
			}
			break;
		case PRODUCT_BUSINESS:
			{
				strTemp += "Business Edition";
			}
			break;
		case PRODUCT_STARTER:
			{
				strTemp += "Starter Edition";
			}
			break;
		case PRODUCT_CLUSTER_SERVER:
			{
				strTemp += "Cluster Server Edition";
			}
			break;
		case PRODUCT_DATACENTER_SERVER:
			{
				strTemp += "Datacenter Edition";
			}
			break;
		case PRODUCT_DATACENTER_SERVER_CORE:
			{
				strTemp += "core installation";
			}
			break;
		case PRODUCT_ENTERPRISE_SERVER:
			{
				strTemp += "Enterprise Edition";
			}
			break;
		case PRODUCT_ENTERPRISE_SERVER_CORE:
			{
				strTemp += "Enterprise Edition (core installation)";
			}
			break;
		case PRODUCT_ENTERPRISE_SERVER_IA64:
			{
				strTemp += "Enterprise Edition for Itanium-based Systems";
			}
			break;
		case PRODUCT_SMALLBUSINESS_SERVER:
			{
				strTemp += "Small Business Server";
			}
			break;
		case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
			{
				strTemp += "Small Business Server Premium Edition";
			}
			break;
		case PRODUCT_STANDARD_SERVER:
			{
				strTemp += "Standard Edition";
			}
			break;
		case PRODUCT_STANDARD_SERVER_CORE:
			{
				strTemp += "Standard Edition (core installation)";
			}
			break;
		case PRODUCT_WEB_SERVER:
			{
				strTemp += "Web Server Edition";
			}
			break;
		default:
			break;
		}
	}
	else if(osVerInfo.dwMajorVersion == 5 && osVerInfo.dwMinorVersion == 2)
	{
		if(::GetSystemMetrics(SM_SERVERR2))
		{
			strTemp += "Windows Server 2003 R2, ";
		}
		else if(osVerInfo.wSuiteMask & VER_SUITE_STORAGE_SERVER)
		{
			strTemp += "Windows Storage Server 2003";
		}
		else if(osVerInfo.wSuiteMask & VER_SUITE_WH_SERVER)
		{
			strTemp += "Windows Home Server";
		}
		else if(osVerInfo.wProductType == VER_NT_WORKSTATION && systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		{
			strTemp += "Windows XP Professional x64 Edition";
		}
		else
		{
			strTemp += "Windows Server 2003, ";
		}

		if(osVerInfo.wProductType != VER_NT_WORKSTATION)
		{
			if(systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
			{
				if(osVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
				{
					strTemp += "Datacenter Edition for Itanium-based Systems";
				}
				else if(osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					strTemp += "Enterprise Edition for Itanium-based Systems";
				}
			}
			else if(systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			{
				if(osVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
				{
					strTemp += "Datacenter x64 Edition";
				}
				else if(osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					strTemp += "Datacenter x64 Edition";
				}
				else
				{
					strTemp += "Standard x64 Edition";
				}
			}
			else
			{
				if(osVerInfo.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
				{
					strTemp += "Compute Cluster Edition";
				}
				else if(osVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
				{
					strTemp += "Datacenter Edition";
				}
				else if(osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					strTemp += "Enterprise Edition";
				}
				else if(osVerInfo.wSuiteMask & VER_SUITE_BLADE)
				{
					strTemp += "Web Edition";
				}
				else
				{
					strTemp += "Standard Edition";
				}
			}
		}
	}
	else if(osVerInfo.dwMajorVersion == 5 && osVerInfo.dwMinorVersion == 1)
	{
		strTemp += "Windows XP ";
		if(osVerInfo.wSuiteMask & VER_SUITE_PERSONAL)
		{
			strTemp += "Home Edition";
		}
		else
		{
			strTemp += "Professional";
		}
	}
	else if(osVerInfo.dwMajorVersion == 5 && osVerInfo.dwMinorVersion == 0)
	{
		strTemp += "Windows 2000 ";
		if(osVerInfo.wProductType == VER_NT_WORKSTATION)
		{
			strTemp += "Professional";
		}
		else
		{
			if(osVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
			{
				strTemp += "Datacenter Server";
			}
			else if(osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
			{
				strTemp += "Advanced Server";
			}
			else
			{
				strTemp += "Server";
			}
		}
	}

	if(_tcslen(osVerInfo.szCSDVersion) > 0)
	{
		strTemp += " ";
		strTemp += osVerInfo.szCSDVersion;
	}

	CString strNum;
	strNum.Format(" (build %d)", osVerInfo.dwBuildNumber);
	strTemp += strNum;
	if(osVerInfo.dwMajorVersion >= 6)
	{
		if(systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		{
			strTemp += ", 64-bit";
		}
		else if(systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		{
			strTemp += ", 32-bit";
		}
	}

	strOsVersion = strTemp;
	TRACE("OsVersion:%s\n", strTemp);

	return !strOsVersion.empty();
}

bool CUtility::GetSysInfo(string &strComputerName, string &strOsLoginUser)
{
	DWORD dwBufCharCount = 0;
	const int INFO_BUFFER_SIZE = 32767;
	char szInfoBuf[INFO_BUFFER_SIZE];

	strComputerName.clear();
	ZeroMemory(szInfoBuf, INFO_BUFFER_SIZE);
	dwBufCharCount = INFO_BUFFER_SIZE;
	if(!::GetComputerName(szInfoBuf, &dwBufCharCount))
	{
		return false;
	}
	strComputerName = szInfoBuf;
	TRACE("\nComputer name:%s", strComputerName.c_str());

	// Get the user name
	strOsLoginUser.clear();
	ZeroMemory(szInfoBuf, INFO_BUFFER_SIZE);
	dwBufCharCount = INFO_BUFFER_SIZE;
	if(!::GetUserName(szInfoBuf, &dwBufCharCount))
	{
		return false;
	}
	strOsLoginUser = szInfoBuf;
	TRACE("\nUser name:%s", strOsLoginUser.c_str());

	return true;
}

bool CUtility::GetFileVersion(string &strAppVersion)
{
	strAppVersion.clear();

	char szPath[MAX_PATH];
	::GetModuleFileName(NULL, szPath, sizeof(szPath));

	/// 获取文件版本信息大小
	DWORD dwDummy = 0;
	DWORD dwVerInfoSize = ::GetFileVersionInfoSize(szPath, &dwDummy);
	if(dwVerInfoSize <= 0)
	{
		return false;
	}

	/// 获取文件版本信息.
	LPBYTE lpVersionInfo = new BYTE[dwVerInfoSize];
	if(lpVersionInfo == NULL)
	{
		return false;
	}
	if(::GetFileVersionInfo(szPath, 0, dwVerInfoSize, lpVersionInfo))
	{
		UINT uLen = 0;
		VS_FIXEDFILEINFO *lpFileInfo = NULL;
		/// 查询文件版本信息
		if(::VerQueryValue(lpVersionInfo, _T("\\"), (LPVOID *)&lpFileInfo, &uLen))
		{
			if(lpFileInfo != NULL)
			{
				DWORD dwFileVersionMS = 0;
				DWORD dwFileVersionLS = 0;

				dwFileVersionMS = lpFileInfo->dwFileVersionMS;
				dwFileVersionLS = lpFileInfo->dwFileVersionLS;

				//LONGLONG lVer = 0;
				//lVer = dwFileVersionMS;
				//lVer <<= 32;
				//lVer |= dwFileVersionLS;

				//DWORD dwFileVersionMS = lVer >> 32;
				//DWORD dwFileVersionLS = lVer & 0x00000000FFFFFFFF;

				DWORD dwVer1 = HIWORD(dwFileVersionMS);
				DWORD dwVer2 = LOWORD(dwFileVersionMS);
				DWORD dwVer3 = HIWORD(dwFileVersionLS);
				DWORD dwVer4 = LOWORD(dwFileVersionLS);

				CString strTemp;
				strTemp.Format(_T("%d.%d.%d.%d"), dwVer1, dwVer2, dwVer3, dwVer4);
				strAppVersion = strTemp;
			}
		}
	}

	delete [] lpVersionInfo;

	return !strAppVersion.empty();
}

bool CUtility::GetIpAddress(const string &strHostName, string &strIpAddress)
{
	strIpAddress.clear();
	if(strHostName.empty())
	{
		return false;
	}

	WSADATA wsaData;
	if(0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return false;
	}

	struct hostent *remoteHost = ::gethostbyname(strHostName.c_str());
	if(NULL != remoteHost)
	{
		if(AF_INET == remoteHost->h_addrtype)
		{
			if(NULL != remoteHost->h_addr_list[0])
			{
				struct in_addr inAddr;
				inAddr.s_addr = *(u_long *)remoteHost->h_addr_list[0];
				strIpAddress = inet_ntoa(inAddr);
			}
		}
	}

	::WSACleanup();

	return !strIpAddress.empty();
}
