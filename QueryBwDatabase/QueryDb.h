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
		m_strOutCompanyName.clear();	///	��������
		m_strInvoiceType.clear();		/// ��Ʊ����
		m_strRealTypeData.clear();		/// ��Ʊ����(���ݿ�ʵ������)
		m_strInvoiceCode.clear();		/// ��Ʊ����
		m_strInvoiceNum.clear();		/// ��Ʊ����
		m_strUserName.clear();			/// �ͻ�����
		m_strUserTaxCode.clear();		/// �ͻ�ʶ���
		m_strUserAddrTel.clear();		///	�ͻ���ַ�绰
		m_strUserBankAccount.clear();	///	�ͻ������˺�
		m_strProductName.clear();		/// ��Ʒ����
		m_strProductCnt.clear();		/// ��Ʒ����
		m_strProductUnit.clear();		/// ��Ʒ��λ
		m_strTypeSpecification.clear();	///	����ͺ�
		m_strCost.clear();				///	����
		m_strTaxNum.clear();			/// ˰��
		m_strTaxSum.clear();			/// �ϼƽ��
		m_strPriceSum.clear();			/// ��˰�ϼ�
		m_strMakePerson.clear();		/// ��Ʊ��
		m_strMakeDate.clear();			/// ��Ʊ����
		m_strCancelStatus.clear();		///	����״̬
		m_strCancelPerson.clear();		/// ������
		m_strCancelDate.clear();		/// ��������
		m_strYFPDM.clear();				/// ԭ��Ʊ����
		m_strYFPHM.clear();				/// ԭ��Ʊ����
		m_strTZDBH.clear();				/// ֪ͨ�����
		m_strTaxRate.clear();			/// ˰��
	}

protected:
	explicit CInvoiceItem(void) { Clear(); }

public:
	std::string m_strOutCompanyName;	///	��������
	std::string m_strInvoiceType;		/// ��Ʊ����
	std::string m_strRealTypeData;		/// ��Ʊ����(���ݿ�ʵ������)
	std::string m_strInvoiceCode;		/// ��Ʊ����
	std::string m_strInvoiceNum;		/// ��Ʊ����
	std::string m_strUserName;			/// �ͻ�����
	std::string m_strUserTaxCode;		/// �ͻ�ʶ���
	std::string m_strUserAddrTel;		///	�ͻ���ַ�绰
	std::string m_strUserBankAccount;	///	�ͻ������˺�
	std::string m_strProductName;		/// ��Ʒ����
	std::string m_strProductCnt;		/// ��Ʒ����
	std::string m_strProductUnit;		/// ��Ʒ��λ
	std::string m_strTypeSpecification;	///	����ͺ�
	std::string m_strCost;				///	����
	std::string m_strTaxNum;			/// ˰��
	std::string m_strTaxSum;			/// �ϼƽ��
	std::string m_strPriceSum;			/// ��˰�ϼ�
	std::string m_strMakePerson;		/// ��Ʊ��
	std::string m_strMakeDate;			/// ��Ʊ����
	std::string m_strCancelStatus;		///	����״̬
	std::string m_strCancelPerson;		/// ������
	std::string m_strCancelDate;		/// ��������
	std::string m_strYFPDM;				/// ԭ��Ʊ����
	std::string m_strYFPHM;				/// ԭ��Ʊ����
	std::string m_strTZDBH;				/// ֪ͨ�����
	std::string m_strTaxRate;			/// ˰��
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
