#include "StdAfx.h"
#include "QueryBwDb.h"
#include "Utility.h"
#include <boost/algorithm/string.hpp>
#include <tinyxml.h>

using namespace std;


#ifndef SQLITE_OK
#define SQLITE_OK		0   /* Successful result */
#endif

#ifndef SQLITE_ROW
#define SQLITE_ROW		100
#endif

#ifndef SQLITE_DONE
#define SQLITE_DONE		101
#endif

#ifndef SQLITE_INTEGER
#define SQLITE_INTEGER	1
#endif

#ifndef SQLITE_FLOAT
#define SQLITE_FLOAT	2
#endif

#ifndef SQLITE_TEXT
#define SQLITE_TEXT		3
#endif

#ifndef SQLITE_BLOB
#define SQLITE_BLOB		4
#endif

#ifndef SQLITE_NULL
#define SQLITE_NULL		5
#endif


CQueryBwDb::CQueryBwDb(void)
{
	m_hModuleSqlite = NULL;
	m_hModuleSkpc = NULL;

	initSqlite = NULL;
	uninitSqlite = NULL;

	sqlite3_open = NULL;
	sqlite3_close = NULL;
	sqlite3_key = NULL;
	//DBDecrypt = NULL;

	sqlite3_prepare_v2 = NULL;
	sqlite3_errmsg = NULL;

	sqlite3_get_table = NULL;

	sqlite3_step = NULL;
	sqlite3_finalize = NULL;
	sqlite3_column_count = NULL;
	sqlite3_column_name = NULL;
	sqlite3_column_type = NULL;
	sqlite3_column_int = NULL;
	sqlite3_column_text = NULL;
	sqlite3_column_double = NULL;

	m_strOldCurrentDirectory.clear();

	m_strTaxCode.clear();
}

CQueryBwDb::~CQueryBwDb(void)
{
}

bool CQueryBwDb::LoadDll(const string &strDllFile)
{
	//string strDllPath = "C:\\Program Files (x86)\\税控发票开票软件(税控盘版)\\sqlite3.dll";
	m_hModuleSqlite = LoadLibrary(strDllFile.c_str());
	if(m_hModuleSqlite == NULL)
	{
		return false;
	}

	initSqlite = (func_initSqlite)::GetProcAddress(m_hModuleSqlite, "initSqlite");
	if(initSqlite == NULL)
	{
		return false;
	}
	uninitSqlite = (func_uninitSqlite)::GetProcAddress(m_hModuleSqlite, "uninitSqlite");
	if(uninitSqlite == NULL)
	{
		return false;
	}

	sqlite3_open = (func_sqlite3_open)::GetProcAddress(m_hModuleSqlite, "sqlite3_open");
	if(sqlite3_open == NULL)
	{
		return false;
	}
	sqlite3_close = (func_sqlite3_close)::GetProcAddress(m_hModuleSqlite, "sqlite3_close");
	if(sqlite3_close == NULL)
	{
		return false;
	}
	sqlite3_key = (func_bw_sqlite3_key)::GetProcAddress(m_hModuleSqlite, "sqlite3_key");
	if(sqlite3_key == NULL)
	{
		return false;
	}
	sqlite3_prepare_v2 = (func_sqlite3_prepare_v2)::GetProcAddress(m_hModuleSqlite, "sqlite3_prepare_v2");
	if(sqlite3_prepare_v2 == NULL)
	{
		return false;
	}
	sqlite3_errmsg = (func_sqlite3_errmsg)::GetProcAddress(m_hModuleSqlite, "sqlite3_errmsg");
	if(sqlite3_errmsg == NULL)
	{
		return false;
	}

	sqlite3_get_table = (func_sqlite3_get_table)::GetProcAddress(m_hModuleSqlite, "sqlite3_get_table");
	if(sqlite3_get_table == NULL)
	{
		return false;
	}

	sqlite3_step = (func_sqlite3_step)::GetProcAddress(m_hModuleSqlite, "sqlite3_step");
	if(sqlite3_step == NULL)
	{
		return false;
	}
	sqlite3_finalize = (func_sqlite3_finalize)::GetProcAddress(m_hModuleSqlite, "sqlite3_finalize");
	if(sqlite3_finalize == NULL)
	{
		return false;
	}
	sqlite3_column_count = (func_sqlite3_column_count)::GetProcAddress(m_hModuleSqlite, "sqlite3_column_count");
	if(sqlite3_column_count == NULL)
	{
		return false;
	}
	sqlite3_column_name = (func_sqlite3_column_name)::GetProcAddress(m_hModuleSqlite, "sqlite3_column_name");
	if(sqlite3_column_name == NULL)
	{
		return false;
	}
	sqlite3_column_type = (func_sqlite3_column_type)::GetProcAddress(m_hModuleSqlite, "sqlite3_column_type");
	if(sqlite3_column_type == NULL)
	{
		return false;
	}
	sqlite3_column_int = (func_sqlite3_column_int)::GetProcAddress(m_hModuleSqlite, "sqlite3_column_int");
	if(sqlite3_column_int == NULL)
	{
		return false;
	}
	sqlite3_column_text = (func_sqlite3_column_text)::GetProcAddress(m_hModuleSqlite, "sqlite3_column_text");
	if(sqlite3_column_text == NULL)
	{
		return false;
	}
	sqlite3_column_double = (func_sqlite3_column_double)::GetProcAddress(m_hModuleSqlite, "sqlite3_column_double");
	if(sqlite3_column_double == NULL)
	{
		return false;
	}

	return true;
}

bool CQueryBwDb::InitDll(const string &strDllPath, const string &strTaxCode)
{
	char szCurrentDir[MAX_PATH];
	if(::GetCurrentDirectory(MAX_PATH, szCurrentDir) == 0)
	{
		return false;
	}
	m_strOldCurrentDirectory = szCurrentDir;

	CString strPath = strDllPath.c_str();
	strPath = strPath.Left(strPath.ReverseFind('\\'));

	/// 设置strDllFile路径
	if(!::SetCurrentDirectory(strPath))
	{
		return false;
	}

	/// 设置纳税人识别号
	m_hModuleSkpc = initSqlite((char *)(strTaxCode.c_str()));
	if(m_hModuleSkpc == NULL)
	{
		return false;
	}

	return true;
}

bool CQueryBwDb::Open(const string &strDbFile, const string &strTaxCode, const string &strDllPath)
{
	if(!LoadDll(strDllPath))
	{
		return false;
	}

	if(!InitDll(strDllPath, strTaxCode))
	{
		return false;
	}

	m_pSqliteDb = NULL;
	string strDbPath = strDbFile;//"C:\\Program Files (x86)\\税控发票开票软件(税控盘版)\\91320104302767442H\\539905411217.dat";
	CUtility::GbkToUtf8(strDbPath);

	if(sqlite3_open(strDbPath.c_str(), &m_pSqliteDb) != SQLITE_OK)
	{
		return false;
	}

	if(sqlite3_key(m_pSqliteDb, "1234567890123456", 16) != SQLITE_OK)
	{
		return false;
	}

	m_strTaxCode = strTaxCode;

	return true;
}

void CQueryBwDb::Close(void)
{
	m_strTaxCode.clear();

	if(uninitSqlite != NULL && m_hModuleSkpc != NULL)
	{
		uninitSqlite(m_hModuleSkpc);
	}

	if(m_pSqliteDb != NULL)
	{
		sqlite3_close(m_pSqliteDb);
		m_pSqliteDb = NULL;
	}

	if(m_hModuleSqlite != NULL)
	{
		::FreeLibrary(m_hModuleSqlite);
		m_hModuleSqlite = NULL;
	}

	if(!m_strOldCurrentDirectory.empty())
	{
		::SetCurrentDirectory(m_strOldCurrentDirectory.c_str());
		m_strOldCurrentDirectory.clear();
	}
}

bool CQueryBwDb::Query(bool &bFoundNewData)
{
	string strSql = "select fpzl_dm, fpzt, fpdm, fphm, gf_nsrmc, zyspmc, se, hjje, jshj, yfpdm, ";
	strSql.append("yfphm, tzdbh, kpr, kprq, zfr, zfrq, gf_nsrsbh, yfpdm, yfphm, tzdbh, sl, xf_nsrmc from zzs_fpkj order by kprq");
	if(!ExecuteQuery(strSql))
	{
		return false;
	}

	bFoundNewData = !(m_lstInvoiceData.empty());
	return true;
}

bool CQueryBwDb::Query(const string &strSql, map<int, string> &mapColIndex2ColName, vector<map<int, string>> &vctQueryResult)
{
	int *pStmt = Prepare(strSql);
	if(pStmt == NULL)
	{
		return false;
	}

	mapColIndex2ColName.clear();
	vctQueryResult.clear();
	//string strRowData;
	int i = 0;
	string strValue;
	///	get the number of returned columns
	int nColumnCount = sqlite3_column_count(pStmt);
	for(i = 0; i < nColumnCount; i++)
	{
		const char *pszName = sqlite3_column_name(pStmt, i);
		strValue = pszName == NULL ? "" : pszName;
		//TRACE("ColIndex:%d,\tColumnName:%s,\tColType:%d\n", i, strValue.c_str(), sqlite3_column_type(pStmt, i));
		auto iter = mapColIndex2ColName.find(i);
		if(iter == mapColIndex2ColName.end())
		{
			mapColIndex2ColName.insert(std::make_pair(i, strValue));
		}
		//strRowData.append(strValue);
		//strRowData.append(";");
	}
	if(mapColIndex2ColName.empty())
	{
		return false;
	}
	//CUtility::WriteFile("D:\\Database.sql", strRowData);

	char *szErrMsg = NULL;
	char **pszDbResult = NULL;
	int nResult = 0, j = 0, nRow = 0, nColumn = 0, nIndex = 0;
	nResult = sqlite3_get_table(m_pSqliteDb, strSql.c_str(), &pszDbResult, &nRow, &nColumn, &szErrMsg);
	if(nResult != SQLITE_OK)
	{
		if(szErrMsg != NULL)
		{
			TRACE("%s\n", szErrMsg);
			AfxMessageBox(szErrMsg);
		}
		return false;
	}
	nIndex = nColumn;
	string strField;
	for(i = 0; i < nRow; i++)
	{
		//strRowData.clear();
		//TRACE("第%d条记录\n", i + 1);
		map<int, string> mapColIndex2Value;
		for(j = 0; j < nColumn; j++)
		{
			//pszDbResult字段值是连续的,0->(nColumn - 1)是字段名称,nColumn->结束是字段值
			strField = pszDbResult[j] == NULL ? string("") : pszDbResult[j];
			CUtility::Utf8ToGbk(strField);
			auto iter = mapColIndex2ColName.find(j);
			if(iter == mapColIndex2ColName.end())
			{
				AfxMessageBox("查询到不存在的列");
				break;
			}
			strValue = pszDbResult[nIndex] == NULL ? string("") : pszDbResult[nIndex];
			CUtility::Utf8ToGbk(strValue);
			TRACE("字段名:%s<->字段值:%s\n", strField.c_str(), strValue.c_str());
			//CUtility::WriteFile("D:\\Database.sql", strValue);
			//CUtility::WriteFile("D:\\Database.sql", ";");
			//strRowData.append(strValue);
			//strRowData.append(";");
/*
			if(boost::algorithm::iequals(strField, "kprq")
				|| boost::algorithm::iequals(strField, "zfrq"))
			{
				CString strDate;
				if(strValue.empty())
				{
					strDate = "";
				}
				else
				{
					COleDateTime dt(atof(strValue.c_str()));
					strDate.Format("%04d-%02d-%02d %02d:%02d:%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay(),
						dt.GetHour(), dt.GetMinute(), dt.GetSecond());
				}

				//bInRange = pInvoiceItem->m_strMakeDate >= strStart && pInvoiceItem->m_strMakeDate < strEnd;

				mapColIndex2Value[j] = strDate;
			}
			else
*/
			{
				mapColIndex2Value[j] = strValue;
			}
			nIndex++;
		}
		//strRowData.append("\n");
		//CUtility::WriteFile("D:\\Database.sql", strRowData);
		if(!mapColIndex2Value.empty())
		{
			vctQueryResult.push_back(mapColIndex2Value);
		}
	}
	
	return true;
}

bool CQueryBwDb::ExecuteQuery(const string &strSql)
{
	int *pStmt = Prepare(strSql);
	if(pStmt == NULL)
	{
		return false;
	}

	string strStart, strEnd;
	if(!GetRange(strStart, strEnd))
	{
		return false;
	}

	string strValue;
	int i = 0, nValue = 0;

	//get the number of returned columns
	int nColumnCount = sqlite3_column_count(pStmt);
	//for(i = 0; i < nColumnCount; i++)
	//{
	//	const char *pszName = sqlite3_column_name(pStmt, i);
	//	strValue = pszName == NULL ? "" : pszName;
	//	TRACE("ColIndex:%d,\tColumnName:%s,\tColType:%d\n", i, strValue.c_str(), sqlite3_column_type(pStmt, i));
	//}

	m_lstInvoiceData.clear();
	while(sqlite3_step(pStmt) == SQLITE_ROW)
	{
		bool bInRange = false;
		CInvoiceItem::Pointer pInvoiceItem = CInvoiceItem::Create();
		for(i = 0; i < nColumnCount; i++)
		{
			double dbValue;
			string strGbk;
			if(i == 13 || i == 15)
			{
				dbValue = sqlite3_column_double(pStmt, i);
			}
			else
			{
				char *pszValue = (char *)(sqlite3_column_text(pStmt, i));
				strGbk = pszValue == NULL ? "" : pszValue;
				CUtility::Utf8ToGbk(strGbk);
			}
			
			switch(i)
			{
			case 0:
				{/// fpzl_dm(发票种类_代码)
					if(strGbk == "007")
					{
						pInvoiceItem->m_strInvoiceType = "增值税普票";
					}
					else if(strGbk == "004")
					{
						pInvoiceItem->m_strInvoiceType = "增值税专票";
					}
					else
					{
						pInvoiceItem->m_strInvoiceType = "未知发票类型";
					}

					pInvoiceItem->m_strRealTypeData = strGbk;
					
					TRACE("fpzl_dm=%s,", strGbk.c_str());
				}
				break;
			case 1:
				{/// fpzt(发票状态)(作废状态)
					pInvoiceItem->m_strCancelStatus = strGbk;
					
					TRACE("fpzt=%s,", strGbk.c_str());
				}
				break;
			case 2:
				{/// fpdm
					pInvoiceItem->m_strInvoiceCode = strGbk;/// 发票代码

					TRACE("fpdm=%s,", strGbk.c_str());
				}
				break;
			case 3:
				{/// fphm
					pInvoiceItem->m_strInvoiceNum = strGbk;	/// 发票号码

					TRACE("fphm=%s,", strGbk.c_str());
				}
				break;
			case 4:
				{/// gf_nsrmc
					pInvoiceItem->m_strUserName = strGbk;/// 客户名称
					TRACE("gf_nsrmc=%s,", strGbk.c_str());
				}
				break;
			case 5:
				{/// zyspmc
					//pInvoiceItem->m_strProductName = strGbk;/// 主要商品名称

					TRACE("zyspmc=%s,", strGbk.c_str());
				}
				break;
			case 6:
				{/// se
					pInvoiceItem->m_strTaxNum = strGbk;/// 税额

					TRACE("se=%s,", strGbk.c_str());
				}
				break;
			case 7:
				{/// hjje
					pInvoiceItem->m_strTaxSum = strGbk;/// 合计金额

					TRACE("hjje=%s,", strGbk.c_str());
				}
				break;
			case 8:
				{/// jshj
					pInvoiceItem->m_strPriceSum = strGbk;/// 价税合计

					TRACE("jshj=%s,", strGbk.c_str());
				}
				break;
			case 9:
				{/// yfpdm
					TRACE("yfpdm=%s,", strGbk.c_str());
				}
				break;
			case 10:
				{/// yfphm
					TRACE("yfphm=%s,", strGbk.c_str());
				}
				break;
			case 11:
				{/// tzdbh
					TRACE("tzdbh=%s,", strGbk.c_str());
				}
				break;
			case 12:
				{/// kpr
					pInvoiceItem->m_strMakePerson = strGbk;/// 开票人

					TRACE("kpr=%s,", strGbk.c_str());
				}
				break;
			case 13:
				{/// kprq

					pInvoiceItem->m_strMakeDate = strGbk;/// 开票人

/*
					CString strDate;
					if(dbValue < 0.01)
					{
						strDate = "";
					}
					else
					{
						COleDateTime dt(dbValue);
						strDate.Format("%04d-%02d-%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
					}

					pInvoiceItem->m_strMakeDate = strDate;/// 开票日期

					bInRange = pInvoiceItem->m_strMakeDate >= strStart && pInvoiceItem->m_strMakeDate < strEnd;

					TRACE("kprq=%s,", strDate);
*/
				}
				break;
			case 14:
				{/// zfr
					pInvoiceItem->m_strCancelPerson = strGbk;/// 作废人
					TRACE("zfr=%s,", strGbk.c_str());
				}
				break;
			case 15:
				{/// zfrq
					CString strDate;
					if(dbValue < 0.01)
					{
						strDate = "";
					}
					else
					{
						COleDateTime dt(dbValue);
						strDate.Format("%04d-%02d-%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
					}
					pInvoiceItem->m_strCancelDate = strGbk;/// 作废日期

					TRACE("zfrq=%s,", strDate);
				}
				break;
			case 16:
				{/// gf_nsrsbh
					pInvoiceItem->m_strUserTaxCode = strGbk;/// 客户识别号
					TRACE("gf_nsrsbh=%s,", strGbk.c_str());
				}
				break;
			case 17:
				{/// yfpdm
					pInvoiceItem->m_strYFPDM = strGbk;/// 原发票代码
					TRACE("yfpdm=%s,", strGbk.c_str());
				}
				break;
			case 18:
				{/// yfphm
					pInvoiceItem->m_strYFPHM = strGbk;/// 原发票号码
					TRACE("yfphm=%s,", strGbk.c_str());
				}
				break;
			case 19:
				{/// tzdbh
					pInvoiceItem->m_strTZDBH = strGbk;/// 通知单编号
					TRACE("tzdbh=%s,", strGbk.c_str());
				}
				break;
			case 20:
				{/// sl
					pInvoiceItem->m_strTaxRate = strGbk;/// 税率
					TRACE("sl=%s", strGbk.c_str());
				}
				break;
			case 21:
				{/// xf_nsrmc
					pInvoiceItem->m_strOutCompanyName = strGbk;/// 销方企业名称
				}
				break;
			default:
				break;
			}
		}

		TRACE("\n");
		if(bInRange)
		{
			m_lstInvoiceData.add(pInvoiceItem);	
		}
	}

	if(sqlite3_finalize(pStmt) != SQLITE_OK)
	{
		return false;
	}
	
	return true;
}

int * CQueryBwDb::Prepare(const string &strSql)
{
	string strUtf8 = strSql;
	CUtility::GbkToUtf8(strUtf8);
	int *pStmt = NULL;

	int nReturn = sqlite3_prepare_v2(m_pSqliteDb, strUtf8.c_str(), strUtf8.size(), &pStmt, NULL);
	char *pszErrorMsg = (char *)(sqlite3_errmsg(m_pSqliteDb));
	if(nReturn != SQLITE_OK)
	{
		if(pszErrorMsg != nullptr)
		{
			OutputDebugString(pszErrorMsg);
		}
		CString strMsg;
		strMsg.Format("系统读取不到指定的税控盘(%s)，无法自动提取数据，请使用“百旺手动采集”提取数据", m_strTaxCode.c_str());
		OutputDebugString(strMsg);
		return NULL;
	}

	return pStmt;
}

bool CQueryBwDb::QueryPrimaryTable(void)
{
	bool bInRange = false;
	string strSql = "SELECT fpzl_dm, fpdm, fphm, fpzt, kprq, kpr, zfr, zfrq, jshj, xf_nsrmc, gf_nsrmc, ";
	strSql.append("gf_nsrsbh, qdbz, yfpdm, yfphm, tzdbh FROM zzs_fpkj order by fpdm, fphm");
	int *pStmt = Prepare(strSql);
	if(pStmt == NULL)
	{
		return false;
	}

	char *szErrMsg = NULL;
	char **pszDbResult = NULL;
	int nResult = 0, i = 0, j = 0, nRow = 0, nColumn = 0, nIndex = 0;
	nResult = sqlite3_get_table(m_pSqliteDb, strSql.c_str(), &pszDbResult, &nRow, &nColumn, &szErrMsg);
	if(nResult != SQLITE_OK)
	{
		if(szErrMsg != NULL)
		{
			TRACE("%s\n", szErrMsg);
			AfxMessageBox(szErrMsg);
		}
		return false;
	}
	nIndex = nColumn;
	string strField, strValue;
	for(i = 0; i < nRow; i++)
	{
		bInRange = false;
		//TRACE("第%d条记录\n", i + 1);
		CTableRecord::Pointer pTableRecord = CTableRecord::Create();
		map<int, string> mapColIndex2Value;
		for(j = 0; j < nColumn; j++)
		{
			//pszDbResult字段值是连续的,0->(nColumn - 1)是字段名称,nColumn->结束是字段值
			strField = pszDbResult[j] == NULL ? string("") : pszDbResult[j];
			CUtility::Utf8ToGbk(strField);
			strValue = pszDbResult[nIndex] == NULL ? string("") : pszDbResult[nIndex];
			CUtility::Utf8ToGbk(strValue);
			TRACE("字段名:%s<->字段值:%s\n", strField.c_str(), strValue.c_str());
			mapColIndex2Value[j] = strValue;
			pTableRecord->Add(strField, strValue);

			nIndex++;
		}

		if(pTableRecord->Size() == nColumn/* && bInRange*/)
		{
			m_vctPrimaryTable.push_back(pTableRecord);
		}
	}

	return !m_vctPrimaryTable.empty();
}

bool CQueryBwDb::QueryMxTable(void)
{
	string strSql = "select fpzl_dm, fpdm, fphm, fpmxxh, spsl, je, sl, se, spmc, jldw, ggxh, spdj from zzs_fpkj_mx order by fpdm, fphm, fpmxxh";
	int *pStmt = Prepare(strSql);
	if(pStmt == NULL)
	{
		return false;
	}

	char *szErrMsg = NULL;
	char **pszDbResult = NULL;
	int nResult = 0, i = 0, j = 0, nRow = 0, nColumn = 0, nIndex = 0;
	nResult = sqlite3_get_table(m_pSqliteDb, strSql.c_str(), &pszDbResult, &nRow, &nColumn, &szErrMsg);
	if(nResult != SQLITE_OK)
	{
		if(szErrMsg != NULL)
		{
			TRACE("%s\n", szErrMsg);
			AfxMessageBox(szErrMsg);
		}
		return false;
	}
	nIndex = nColumn;
	string strField, strValue;
	for(i = 0; i < nRow; i++)
	{
		CTableRecord::Pointer pTableRecord = CTableRecord::Create();
		map<int, string> mapColIndex2Value;
		for(j = 0; j < nColumn; j++)
		{
			strField = pszDbResult[j] == NULL ? string("") : pszDbResult[j];
			CUtility::Utf8ToGbk(strField);
			strValue = pszDbResult[nIndex] == NULL ? string("") : pszDbResult[nIndex];
			CUtility::Utf8ToGbk(strValue);
			TRACE("字段名:%s<->字段值:%s\n", strField.c_str(), strValue.c_str());
			mapColIndex2Value[j] = strValue;
			pTableRecord->Add(strField, strValue);

			nIndex++;
		}

		if(pTableRecord->Size() == nColumn)
		{
			m_vctMxTable.push_back(pTableRecord);
		}
	}

	return !m_vctMxTable.empty();
}

bool CQueryBwDb::QueryQdTable(void)
{
	string strSql = "select fpzl_dm, fpdm, fphm, fpmxxh, spsl, je, sl, se, spmc, jldw, ggxh, spdj from zzs_fpkj_qd order by fpdm, fphm, fpmxxh";
	int *pStmt = Prepare(strSql);
	if(pStmt == NULL)
	{
		return false;
	}

	char *szErrMsg = NULL;
	char **pszDbResult = NULL;
	int nResult = 0, i = 0, j = 0, nRow = 0, nColumn = 0, nIndex = 0;
	nResult = sqlite3_get_table(m_pSqliteDb, strSql.c_str(), &pszDbResult, &nRow, &nColumn, &szErrMsg);
	if(nResult != SQLITE_OK)
	{
		if(szErrMsg != NULL)
		{
			TRACE("%s\n", szErrMsg);
			AfxMessageBox(szErrMsg);
		}
		return false;
	}
	nIndex = nColumn;
	string strField, strValue;
	for(i = 0; i < nRow; i++)
	{
		CTableRecord::Pointer pTableRecord = CTableRecord::Create();
		map<int, string> mapColIndex2Value;
		for(j = 0; j < nColumn; j++)
		{
			strField = pszDbResult[j] == NULL ? string("") : pszDbResult[j];
			CUtility::Utf8ToGbk(strField);
			strValue = pszDbResult[nIndex] == NULL ? string("") : pszDbResult[nIndex];
			CUtility::Utf8ToGbk(strValue);
			TRACE("字段名:%s<->字段值:%s\n", strField.c_str(), strValue.c_str());
			pTableRecord->Add(strField, strValue);

			nIndex++;
		}

		if(pTableRecord->Size() == nColumn)
		{
			m_vctQdTable.push_back(pTableRecord);
		}
	}

	return !m_vctQdTable.empty();
}

bool CQueryBwDb::Query(void)
{
	m_lstInvoiceData.clear();

	///	1 查询主表,生成数据结构
	if(!QueryPrimaryTable())
	{
		return false;
	}

	///	2 查询明细表,生成数据结构
	QueryMxTable();

	///	3 查询清单表,生成数据结构
	QueryQdTable();

	/// 4 遍历主表数据,根据特定字段值,关联查找明细表、清单表
	CalcInvoiceData();

	string strXml;
	GenerateXml(strXml);

	return !m_lstInvoiceData.empty();
}

bool CQueryBwDb::CalcInvoiceData(void)
{
	if(m_vctPrimaryTable.empty())
	{
		return false;
	}

	m_lstInvoiceData.clear();
	
	string strValue;
	vector<CTableRecord::Pointer>::const_iterator iterPrimary;
	for(iterPrimary = m_vctPrimaryTable.begin(); iterPrimary != m_vctPrimaryTable.end(); iterPrimary++)
	{
		const CTableRecord::Pointer &pPrimaryTableRecord = *iterPrimary;
		pPrimaryTableRecord->GetValue("qdbz", strValue);
		if(strValue == "0")
		{
			/// 没有清单
			SearchMxTable(pPrimaryTableRecord);
		}
		else
		{
			/// 有清单
			SearchQdTable(pPrimaryTableRecord);
		}
	}
	
	return true;
}

bool CQueryBwDb::PrimaryTableRecordToInvoiceItem(const CTableRecord::Pointer pPrimaryTableRecord, CInvoiceItem::Pointer pInvoiceItem)
{
	if(!pInvoiceItem)
	{
		return false;
	}

/*
SELECT fpzl_dm, fpdm, fphm, fpzt, kprq, kpr, zfrq, zfr, jshj, xf_nsrmc, gf_nsrmc, 
gf_nsrsbh, qdbz, yfpdm, yfphm, tzdbh FROM zzs_fpkj order by fpdm, fphm
*/
	string strValue;
	pInvoiceItem->Clear();

	if(pPrimaryTableRecord->GetValue("fpzl_dm", strValue))
	{
		pInvoiceItem->m_strRealTypeData = strValue;

		if(strValue == "007")
		{
			pInvoiceItem->m_strInvoiceType = "增值税普票";
		}
		else if(strValue == "004")
		{
			pInvoiceItem->m_strInvoiceType = "增值税专票";
		}
		else
		{
			pInvoiceItem->m_strInvoiceType = "未知发票类型";
		}
	}

	if(pPrimaryTableRecord->GetValue("fpdm", strValue))
	{
		pInvoiceItem->m_strInvoiceCode = strValue;
	}

	if(pPrimaryTableRecord->GetValue("fphm", strValue))
	{
		pInvoiceItem->m_strInvoiceNum = strValue;
	}

	if(pPrimaryTableRecord->GetValue("fpzt", strValue))
	{
		pInvoiceItem->m_strCancelStatus = strValue;
	}

	if(pPrimaryTableRecord->GetValue("kprq", strValue))
	{
		CString strDate;
		if(strValue.empty())
		{
			strDate = "";
		}
		else
		{
			COleDateTime dt(atof(strValue.c_str()));
			strDate.Format("%04d-%02d-%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
		}
		pInvoiceItem->m_strMakeDate = strDate;
	}

	if(pPrimaryTableRecord->GetValue("kpr", strValue))
	{
		pInvoiceItem->m_strMakePerson = strValue;
	}

	if(pPrimaryTableRecord->GetValue("zfrq", strValue))
	{
		CString strDate;
		if(strValue.empty())
		{
			strDate = "";
		}
		else
		{
			COleDateTime dt(atof(strValue.c_str()));
			strDate.Format("%04d-%02d-%02d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
		}
		pInvoiceItem->m_strCancelDate = strDate;
	}

	if(pPrimaryTableRecord->GetValue("zfr", strValue))
	{
		pInvoiceItem->m_strCancelPerson = strValue;
	}

	if(pPrimaryTableRecord->GetValue("jshj", strValue))
	{
		pInvoiceItem->m_strPriceSum = strValue;
	}

	if(pPrimaryTableRecord->GetValue("xf_nsrmc", strValue))
	{
		pInvoiceItem->m_strOutCompanyName = strValue;
	}

	if(pPrimaryTableRecord->GetValue("gf_nsrmc", strValue))
	{
		pInvoiceItem->m_strUserName = strValue;
	}

	if(pPrimaryTableRecord->GetValue("gf_nsrsbh", strValue))
	{
		pInvoiceItem->m_strUserTaxCode = strValue;
	}

	if(pPrimaryTableRecord->GetValue("gf_dzdh", strValue))
	{
		pInvoiceItem->m_strUserAddrTel = strValue;
	}

	if(pPrimaryTableRecord->GetValue("gf_yhzh", strValue))
	{
		pInvoiceItem->m_strUserBankAccount = strValue;
	}

	if(pPrimaryTableRecord->GetValue("yfpdm", strValue))
	{
		pInvoiceItem->m_strYFPDM = strValue;
	}

	if(pPrimaryTableRecord->GetValue("yfphm", strValue))
	{
		pInvoiceItem->m_strYFPHM = strValue;
	}

	if(pPrimaryTableRecord->GetValue("tzdbh", strValue))
	{
		pInvoiceItem->m_strTZDBH = strValue;
	}

	return true;
}

bool CQueryBwDb::SearchMxTable(const CTableRecord::Pointer pPrimaryTableRecord)
{
	if(!pPrimaryTableRecord)
	{
		return false;
	}

	string strFpdm;/// 发票代码
	if(!pPrimaryTableRecord->GetValue("fpdm", strFpdm))
	{
		return false;
	}

	string strFphm;/// 发票号码
	if(!pPrimaryTableRecord->GetValue("fphm", strFphm))
	{
		return false;
	}

	for(auto iter = m_vctMxTable.begin(); iter != m_vctMxTable.end(); iter++)
	{
		string strFpdmNew, strFphmNew;
		CTableRecord::Pointer &pMxTableRecord = *iter;
		if(!pMxTableRecord->GetValue("fpdm", strFpdmNew))
		{
			continue;
		}
		if(!pMxTableRecord->GetValue("fphm", strFphmNew))
		{
			continue;
		}

		if(strFpdm == strFpdmNew && strFphm == strFphmNew)
		{
			CInvoiceItem::Pointer pInvoiceItem = CInvoiceItem::Create();

			PrimaryTableRecordToInvoiceItem(pPrimaryTableRecord, pInvoiceItem);

			pMxTableRecord->GetValue("je", pInvoiceItem->m_strTaxSum);

			pMxTableRecord->GetValue("sl", pInvoiceItem->m_strTaxRate);

			pMxTableRecord->GetValue("se", pInvoiceItem->m_strTaxNum);

			pMxTableRecord->GetValue("spmc", pInvoiceItem->m_strProductName);

			pMxTableRecord->GetValue("spsl", pInvoiceItem->m_strProductCnt);

			pMxTableRecord->GetValue("jldw", pInvoiceItem->m_strProductUnit);

			pMxTableRecord->GetValue("ggxh", pInvoiceItem->m_strTypeSpecification);

			pMxTableRecord->GetValue("spdj", pInvoiceItem->m_strCost);

			m_lstInvoiceData.push_back(pInvoiceItem);
		}
	}

	return true;
}

bool CQueryBwDb::SearchQdTable(const CTableRecord::Pointer pPrimaryTableRecord)
{
	if(!pPrimaryTableRecord)
	{
		return false;
	}

	string strFpdm;/// 发票代码
	if(!pPrimaryTableRecord->GetValue("fpdm", strFpdm))
	{
		return false;
	}

	string strFphm;/// 发票号码
	if(!pPrimaryTableRecord->GetValue("fphm", strFphm))
	{
		return false;
	}

	for(auto iter = m_vctQdTable.begin(); iter != m_vctQdTable.end(); iter++)
	{
		string strFpdmNew, strFphmNew;
		CTableRecord::Pointer &pQdTableRecord = *iter;
		if(!pQdTableRecord->GetValue("fpdm", strFpdmNew))
		{
			continue;
		}
		if(!pQdTableRecord->GetValue("fphm", strFphmNew))
		{
			continue;
		}

		if(strFpdm == strFpdmNew && strFphm == strFphmNew)
		{
			CInvoiceItem::Pointer pInvoiceItem = CInvoiceItem::Create();

			PrimaryTableRecordToInvoiceItem(pPrimaryTableRecord, pInvoiceItem);

			pQdTableRecord->GetValue("je", pInvoiceItem->m_strTaxSum);

			pQdTableRecord->GetValue("sl", pInvoiceItem->m_strTaxRate);

			pQdTableRecord->GetValue("se", pInvoiceItem->m_strTaxNum);

			pQdTableRecord->GetValue("spmc", pInvoiceItem->m_strProductName);

			pQdTableRecord->GetValue("spsl", pInvoiceItem->m_strProductCnt);

			pQdTableRecord->GetValue("jldw", pInvoiceItem->m_strProductUnit);

			pQdTableRecord->GetValue("ggxh", pInvoiceItem->m_strTypeSpecification);

			pQdTableRecord->GetValue("spdj", pInvoiceItem->m_strCost);

			m_lstInvoiceData.push_back(pInvoiceItem);
		}
	}

	return true;
}

bool CQueryBwDb::GenerateXml(string &strXml)
{
	strXml.clear();

	TiXmlDocument xmlDoc;
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "");
	xmlDoc.LinkEndChild(pDeclaration);
	TiXmlElement *pElementRoot = new TiXmlElement("Data");
	if(pElementRoot == NULL)
	{
		return false;
	}
	CUtility::SetAttributeText(pElementRoot, "INFO", "YIKAIFAPIAO");
	TiXmlNode* pNodeRoot = xmlDoc.LinkEndChild(pElementRoot);
	if(pNodeRoot == NULL)
	{
		return false;
	}

	CUtility::AddNode(pElementRoot, "GUID", "");

	TiXmlNode *pNodeRunEnvironment = new TiXmlElement("RunEnvironment");
	if(pNodeRunEnvironment == NULL)
	{
		return false;
	}
	//CUtility::AddNode(pNodeRunEnvironment, "ComputerName", m_strComputerName);
	//CUtility::AddNode(pNodeRunEnvironment, "IpAddress", m_strIpAddress);
	//CUtility::AddNode(pNodeRunEnvironment, "OsVersion", m_strOsVersion);
	//CUtility::AddNode(pNodeRunEnvironment, "OsLoginUser", m_strOsLoginUser);
	//CUtility::AddNode(pNodeRunEnvironment, "AppVersion", m_strAppVersion);
	pNodeRoot->LinkEndChild(pNodeRunEnvironment);

	TiXmlNode *pNodeCompanyInfo = new TiXmlElement("CompanyInfo");
	if(pNodeCompanyInfo == NULL)
	{
		return false;
	}
	CUtility::AddNode(pNodeCompanyInfo, "CompanyName", "");
	CUtility::AddNode(pNodeCompanyInfo, "CompanyCode", "");
	CUtility::AddNode(pNodeCompanyInfo, "LoginName", "admin");

	pNodeRoot->LinkEndChild(pNodeCompanyInfo);

	if(m_lstInvoiceData.empty())
	{
		return false;
	}

	TiXmlElement *pElementYKFP = new TiXmlElement("YKFP");
	if(pElementYKFP == NULL)
	{
		return false;
	}
	TiXmlNode* pNodeYKFP = pNodeRoot->LinkEndChild(pElementYKFP);
	if(pNodeYKFP == NULL)
	{
		return false;
	}

	TiXmlElement *pElementInvoiceCompany = new TiXmlElement("InvoiceCompany");
	if(pElementInvoiceCompany == NULL)
	{
		return false;
	}
	CUtility::AddNode(pElementInvoiceCompany, "TaxCode", "");
	CUtility::AddNode(pElementInvoiceCompany, "AppType", "BW");
	//CUtility::AddNode(pElementInvoiceCompany, "CompanyName", "");
	pNodeYKFP->LinkEndChild(pElementInvoiceCompany);

	TiXmlElement *pElementRows = new TiXmlElement("Rows");
	if(pElementRows == NULL)
	{
		return false;
	}

	bool bSetCompanyName = false;
	CLockList<CInvoiceItem::Pointer>::const_iterator iter;
	BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_lstInvoiceData.mutex()));
	for(iter = m_lstInvoiceData.begin(); iter != m_lstInvoiceData.end(); iter++)
	{
		const CInvoiceItem::Pointer &pInvoiceItem = *iter;

		TiXmlElement *pElementRow = new TiXmlElement("Row");
		if(pElementRow == NULL)
		{
			return false;
		}
		if(!bSetCompanyName && !pInvoiceItem->m_strOutCompanyName.empty())
		{
			CUtility::AddNode(pElementInvoiceCompany, "CompanyName", pInvoiceItem->m_strOutCompanyName);
			bSetCompanyName = true;
		}

		CUtility::SetAttributeText(pElementRow, string("发票类型"), pInvoiceItem->m_strInvoiceType);
		CUtility::SetAttributeText(pElementRow, string("发票类型-原始数据"), pInvoiceItem->m_strRealTypeData);
		CUtility::SetAttributeText(pElementRow, string("发票代码"), pInvoiceItem->m_strInvoiceCode);
		CUtility::SetAttributeText(pElementRow, string("发票号码"), pInvoiceItem->m_strInvoiceNum);
		CUtility::SetAttributeText(pElementRow, string("客户名称"), pInvoiceItem->m_strUserName);
		CUtility::SetAttributeText(pElementRow, string("客户识别号"), pInvoiceItem->m_strUserTaxCode);
		//CUtility::SetAttributeText(pElementRow, string("主要商品名称"), pInvoiceItem->m_strProductName);
		CUtility::SetAttributeText(pElementRow, string("税额"), pInvoiceItem->m_strTaxNum);
		CUtility::SetAttributeText(pElementRow, string("合计金额"), pInvoiceItem->m_strTaxSum);
		CUtility::SetAttributeText(pElementRow, string("价税合计"), pInvoiceItem->m_strPriceSum);
		CUtility::SetAttributeText(pElementRow, string("原发票代码"), pInvoiceItem->m_strYFPDM);
		CUtility::SetAttributeText(pElementRow, string("原发票号码"), pInvoiceItem->m_strYFPHM);
		CUtility::SetAttributeText(pElementRow, string("通知单编号"), pInvoiceItem->m_strTZDBH);
		CUtility::SetAttributeText(pElementRow, string("开票人"), pInvoiceItem->m_strMakePerson);
		CUtility::SetAttributeText(pElementRow, string("开票日期"), pInvoiceItem->m_strMakeDate);
		CUtility::SetAttributeText(pElementRow, string("作废状态"), pInvoiceItem->m_strCancelStatus);
		CUtility::SetAttributeText(pElementRow, string("作废人"), pInvoiceItem->m_strCancelPerson);
		CUtility::SetAttributeText(pElementRow, string("作废日期"), pInvoiceItem->m_strCancelDate);
		CUtility::SetAttributeText(pElementRow, string("税率"), pInvoiceItem->m_strTaxRate);

		CUtility::SetAttributeText(pElementRow, string("购方地址电话"), pInvoiceItem->m_strUserAddrTel);
		CUtility::SetAttributeText(pElementRow, string("客户银行帐号"), pInvoiceItem->m_strUserBankAccount);
		CUtility::SetAttributeText(pElementRow, string("商品名称"), pInvoiceItem->m_strProductName);
		CUtility::SetAttributeText(pElementRow, string("商品数量"), pInvoiceItem->m_strProductCnt);
		CUtility::SetAttributeText(pElementRow, string("商品单位"), pInvoiceItem->m_strProductUnit);
		CUtility::SetAttributeText(pElementRow, string("规格型号"), pInvoiceItem->m_strTypeSpecification);
		CUtility::SetAttributeText(pElementRow, string("单价"), pInvoiceItem->m_strCost);

		pElementRows->LinkEndChild(pElementRow);
	}

	pNodeYKFP->LinkEndChild(pElementRows);
	TiXmlPrinter printer;
	xmlDoc.Accept(&printer);
	strXml = printer.CStr();

	//TRACE("\n%s", strXml.c_str());

	CUtility::WriteFile("D:\\InvoiceData.xml", strXml);

	CUtility::GbkToUtf8(strXml);

	return true;
}
