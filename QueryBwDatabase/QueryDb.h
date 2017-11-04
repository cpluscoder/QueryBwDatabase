#pragma once

#include <locklist.h>
#include "CommonDef.h"

class TiXmlNode;

class CInvoiceItem
{
	DEFINE_BOOST_SHARED_PTR(CInvoiceItem)

public:

	virtual ~CInvoiceItem(void) { Clear(); }

	void Clear(void)
	{
		m_strOutCompanyName.clear();	///	销方名称
		m_strInvoiceType.clear();		/// 发票类型
		m_strRealTypeData.clear();		/// 发票类型(数据库实际数据)
		m_strInvoiceCode.clear();		/// 发票代码
		m_strInvoiceNum.clear();		/// 发票号码
		m_strUserName.clear();			/// 客户名称
		m_strUserTaxCode.clear();		/// 客户识别号
		m_strUserAddrTel.clear();		///	客户地址电话
		m_strUserBankAccount.clear();	///	客户银行账号
		m_strProductName.clear();		/// 商品名称
		m_strProductCnt.clear();		/// 商品数量
		m_strProductUnit.clear();		/// 商品单位
		m_strTypeSpecification.clear();	///	规格型号
		m_strCost.clear();				///	单价
		m_strTaxNum.clear();			/// 税额
		m_strTaxSum.clear();			/// 合计金额
		m_strPriceSum.clear();			/// 价税合计
		m_strMakePerson.clear();		/// 开票人
		m_strMakeDate.clear();			/// 开票日期
		m_strCancelStatus.clear();		///	作废状态
		m_strCancelPerson.clear();		/// 作废人
		m_strCancelDate.clear();		/// 作废日期
		m_strYFPDM.clear();				/// 原发票代码
		m_strYFPHM.clear();				/// 原发票号码
		m_strTZDBH.clear();				/// 通知单编号
		m_strTaxRate.clear();			/// 税率
	}

protected:
	explicit CInvoiceItem(void) { Clear(); }

public:
	std::string m_strOutCompanyName;	///	销方名称
	std::string m_strInvoiceType;		/// 发票类型
	std::string m_strRealTypeData;		/// 发票类型(数据库实际数据)
	std::string m_strInvoiceCode;		/// 发票代码
	std::string m_strInvoiceNum;		/// 发票号码
	std::string m_strUserName;			/// 客户名称
	std::string m_strUserTaxCode;		/// 客户识别号
	std::string m_strUserAddrTel;		///	客户地址电话
	std::string m_strUserBankAccount;	///	客户银行账号
	std::string m_strProductName;		/// 商品名称
	std::string m_strProductCnt;		/// 商品数量
	std::string m_strProductUnit;		/// 商品单位
	std::string m_strTypeSpecification;	///	规格型号
	std::string m_strCost;				///	单价
	std::string m_strTaxNum;			/// 税额
	std::string m_strTaxSum;			/// 合计金额
	std::string m_strPriceSum;			/// 价税合计
	std::string m_strMakePerson;		/// 开票人
	std::string m_strMakeDate;			/// 开票日期
	std::string m_strCancelStatus;		///	作废状态
	std::string m_strCancelPerson;		/// 作废人
	std::string m_strCancelDate;		/// 作废日期
	std::string m_strYFPDM;				/// 原发票代码
	std::string m_strYFPHM;				/// 原发票号码
	std::string m_strTZDBH;				/// 通知单编号
	std::string m_strTaxRate;			/// 税率
};

class CQueryDb
{
	DECLARE_BOOST_SHARED_PTR(CQueryDb)

public:

	virtual bool Open(const std::string &strDbFile, const std::string &strTaxCode, const std::string &strDllPath) = 0;

	virtual bool Query(bool &bFoundNewData) = 0;

	virtual bool Query(const std::string &strSql, std::map<int, std::string> &mapColIndex2ColName, std::vector<std::map<int, std::string>> &vctQueryResult) = 0;

	virtual void Close(void) = 0;

	virtual bool GenerateInvoiceData(const std::string &strChangeTaxCode, const std::string &strAppType, TiXmlNode* pNodeRoot) const;

	virtual void GetListShowData(CListShowData::Pointer &pListShowData) const;

	virtual bool GetRange(std::string &strStart, std::string &strEnd) const;

	virtual ~CQueryDb(void);

protected:
	explicit CQueryDb(void);

protected:
	CLockList<CInvoiceItem::Pointer> m_lstInvoiceData;
	HMODULE m_hModuleSqlite;/// sqlite3.dll
	int *m_pSqliteDb;/// SQLite db handle
};
