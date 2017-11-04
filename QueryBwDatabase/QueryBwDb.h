#pragma once

#include "QueryDb.h"


//typedef HMODULE (__stdcall * func_initSqlite)(char * pszCompanyCode);/// 纳税人识别号
//typedef int (__stdcall * func_uninitSqlite)(HMODULE hModule);

extern "C" {

	typedef HMODULE (__stdcall * func_initSqlite)(char * pszCompanyCode);/// 纳税人识别号
	typedef int (__stdcall * func_uninitSqlite)(HMODULE hModule);

	typedef int (* func_sqlite3_open)(const char *pszFileName, int **ppDb);
	typedef int (* func_sqlite3_close)(int *pDb);
	typedef int (* func_bw_sqlite3_key)(int *pDb, const char *pszPwd, int nPwdLength);

	typedef int (* func_sqlite3_prepare_v2)(
		int *pDb,			/* Database handle */
		const char *zSql,	/* SQL statement, UTF-8 encoded */
		int nByte,			/* Maximum length of zSql in bytes. */
		int **ppStmt,		/* OUT: Statement handle */
		const char **pzTail	/* OUT: Pointer to unused portion of zSql */
		);

	typedef const char * (* func_sqlite3_errmsg)(int *pDb);

	typedef int (* func_sqlite3_get_table)(
		int *pDb,			  /* An open database */
		const char *zSql,     /* SQL to be evaluated */
		char ***pazResult,    /* Results of the query */
		int *pnRow,           /* Number of result rows written here */
		int *pnColumn,        /* Number of result columns written here */
		char **pzErrmsg       /* Error msg written here */
		);

	typedef int (* func_sqlite3_step)(int *pStmt);
	typedef int (* func_sqlite3_finalize)(int *pStmt);
	typedef int (* func_sqlite3_column_count)(int *pStmt);
	typedef const char * (* func_sqlite3_column_name)(int *pStmt, int nColumn);
	typedef int (* func_sqlite3_column_type)(int *pStmt, int nColumn);
	typedef int (* func_sqlite3_column_int)(int *pStmt, int nColumn);
	typedef const unsigned char * (* func_sqlite3_column_text)(int *pStmt, int nColumn);
	typedef double (* func_sqlite3_column_double)(int *pStmt, int nColumn);

	//typedef int (__stdcall * func_DBDecrypt)(std::string strFileName);
}

class CTableRecord
{
	DEFINE_BOOST_SHARED_PTR(CTableRecord)

public:
	virtual ~CTableRecord(void) { Clear(); }

	virtual void Clear(void) { m_mapFieldName2Value.clear(); }

	virtual int Size(void) const { return m_mapFieldName2Value.size(); }

	virtual bool Add(const std::string &strFieldName, const std::string &strValue, bool bCoverValue = false)
	{
		auto iter = m_mapFieldName2Value.find(strFieldName);
		if(iter != m_mapFieldName2Value.end())
		{
			if(bCoverValue)
			{
				iter->second = strValue;
				return true;
			}
			else
			{
				return false;
			}
		}

		m_mapFieldName2Value.insert(std::make_pair(strFieldName, strValue));
		return true;
	}

	virtual bool GetValue(const std::string &strFieldName, std::string &strValue) const
	{
		strValue.clear();
		if(m_mapFieldName2Value.empty())
		{
			return false;
		}
		auto iter = m_mapFieldName2Value.find(strFieldName);
		if(iter == m_mapFieldName2Value.end())
		{
			return false;
		}

		strValue = iter->second;
		return true;
	}

protected:
	explicit CTableRecord(void) { Clear(); }

protected:
	std::map<std::string, std::string> m_mapFieldName2Value;
};


class CQueryBwDb : public CQueryDb
{
	DEFINE_BOOST_SHARED_PTR(CQueryBwDb)

public:

	virtual bool Open(const std::string &strDbFile, const std::string &strTaxCode, const std::string &strDllPath);

	virtual bool Query(void);

	virtual bool Query(bool &bFoundNewData);

	virtual bool Query(const std::string &strSql, std::map<int, std::string> &mapColIndex2ColName, std::vector<std::map<int, std::string>> &vctQueryResult);

	virtual void Close(void);

	virtual ~CQueryBwDb(void);

protected:
	explicit CQueryBwDb(void);

	bool LoadDll(const std::string &strDllFile);

	bool InitDll(const std::string &strDllPath, const std::string &strTaxCode);

	bool ExecuteQuery(const std::string &strSql);

	int * Prepare(const std::string &strSql);

	bool QueryPrimaryTable(void);

	bool QueryMxTable(void);

	bool QueryQdTable(void);

	bool CalcInvoiceData(void);

	///	@brief		根据发票主表记录关联查询发票明细表,并返回查询到的数据
	///	@param[in]	pPrimaryTableRecord				发票主表记录
	///	@retval		主表记录成功匹配发票明细表
	bool SearchMxTable(const CTableRecord::Pointer pPrimaryTableRecord);

	///	@brief		根据发票主表记录关联查询发票清单表,并返回查询到的数据
	///	@param[in]	pPrimaryTableRecord				发票主表记录
	///	@retval		主表记录成功匹配发票清单表
	bool SearchQdTable(const CTableRecord::Pointer pPrimaryTableRecord);

	///	@brief		根据发票主表记录生成发票数据
	bool PrimaryTableRecordToInvoiceItem(const CTableRecord::Pointer pPrimaryTableRecord, CInvoiceItem::Pointer pInvoiceItem);

	bool GenerateXml(std::string &strXml);

protected:
	func_initSqlite initSqlite;
	func_uninitSqlite uninitSqlite;

	func_sqlite3_open sqlite3_open;
	func_sqlite3_close sqlite3_close;
	func_bw_sqlite3_key sqlite3_key;
	func_sqlite3_prepare_v2 sqlite3_prepare_v2;
	func_sqlite3_errmsg sqlite3_errmsg;

	func_sqlite3_get_table sqlite3_get_table;

	func_sqlite3_step sqlite3_step;
	func_sqlite3_finalize sqlite3_finalize;
	func_sqlite3_column_count sqlite3_column_count;
	func_sqlite3_column_name sqlite3_column_name;
	func_sqlite3_column_type sqlite3_column_type;
	func_sqlite3_column_int sqlite3_column_int;
	func_sqlite3_column_text sqlite3_column_text;
	func_sqlite3_column_double sqlite3_column_double;

	//func_DBDecrypt DBDecrypt;

	HMODULE m_hModuleSkpc;	/// skpc.dll

	std::string m_strOldCurrentDirectory;

	std::string m_strTaxCode;

	std::vector<CTableRecord::Pointer> m_vctPrimaryTable;	/// 百旺发票主表数据
	std::vector<CTableRecord::Pointer> m_vctMxTable;		/// 百旺发票明细表数据
	std::vector<CTableRecord::Pointer> m_vctQdTable;		///	百旺发票清单表数据
};

