#pragma once

#include <string>
#include <boost/date_time.hpp>

class TiXmlNode;

/// 全局方法
class CUtility
{
	CUtility(const CUtility &);
	CUtility &operator= (const CUtility &);
public:
	explicit CUtility(void);
	virtual ~CUtility(void);

	static void GetAppFullPath(std::string &strFullPath);

	static void GetFileTime(const std::string &strDateTime, COleDateTime &dt);

	//static void GetFileTime(const std::string &strDateTime, FILETIME &fileTime);

//////////////////////////////////////////////////////////////////////////
/// boost date time
	/// @param[out]		strDateTime		Format:YYYY-MM-DDTHH:MI:SS
	static bool FileTime2String(const FILETIME &tmFile, std::string &strDateTime);

	/// @param[in]		strDateTime		Format:YYYY-MM-DDTHH:MI:SS
	static bool String2PosixTime(const std::string &strDateTime, boost::posix_time::ptime &dtDateTime);

	/// @param[out]		strDateTime		Format:YYYY-MM-DDTHH:MI:SS
	static bool PosixTime2String(const boost::posix_time::ptime &dtDateTime, std::string &strDateTime);

	/// @param[out]		strDateTime		Format:YYYY-MM-DD HH:MI:SS
	static bool StdTime2String(const std::time_t &tmSrc, std::string &strDateTime);
///
//////////////////////////////////////////////////////////////////////////

	static bool ReadFile(const std::string &strFileName, std::string &strReadData);
	static bool WriteFile(const std::string &strFileName, const std::string &strWriteData, const char *szMode = "a+");

	static void AddNode(TiXmlNode* pParent, const std::string &strKey, const std::string &strValue);
	static void AddNode(TiXmlNode* pParent, const std::string &strComment, const std::string &strKey, const std::string &strValue);


	static bool GetNodeText(TiXmlNode* pNode, std::string &strText);

	static bool SetNodeText(TiXmlNode* pNode, const std::string &strText);

	static bool GetAttributeText(TiXmlNode* pNode, const std::string &strAttributeName, std::string &strText);

	static bool SetAttributeText(TiXmlNode* pNode, const std::string &strAttributeName, const std::string &strText);

	static std::string IntToString(const int nValue);

	static int StringToInt(const std::string &str);

	static CString StringToCString(const std::string &str);

	static std::string CStringToString(const CString &str);

	static void Utf8ToGbk(std::string &strUtf8ToGbk);

	static void GbkToUtf8(std::string &strGbkToUtf8);

	static CString GetFileNameFromPath(const CString &str);

	/// 从文件名（全路径）获取文件路径
	static bool GetPathFromFileName(const std::string &strFileName, std::string &strPath);

	/// 从文件名获取文件后缀
	static CString GetFileSuffix(const CString &strFileName);

	static bool GetOsVersion(std::string &strOsVersion);

	static bool GetSysInfo(std::string &strComputerName, std::string &strOsLoginUser);

	static bool GetFileVersion(std::string &strAppVersion);

	static bool GetIpAddress(const std::string &strHostName, std::string &strIpAddress);
};
