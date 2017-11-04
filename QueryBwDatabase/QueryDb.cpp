#include "StdAfx.h"
#include "QueryDb.h"
#include <tinyxml.h>
#include "Utility.h"

using namespace std;


CQueryDb::CQueryDb(void)
{
	m_hModuleSqlite = NULL;
	m_pSqliteDb = NULL;

	m_lstInvoiceData.clear();
}

CQueryDb::~CQueryDb(void)
{
	m_lstInvoiceData.clear();
}

bool CQueryDb::GenerateInvoiceData(const string &strChangeTaxCode, const string &strAppType, TiXmlNode* pNodeRoot) const
{
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
	CUtility::AddNode(pElementInvoiceCompany, "TaxCode", strChangeTaxCode);
	CUtility::AddNode(pElementInvoiceCompany, "AppType", strAppType);
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

		pElementRows->LinkEndChild(pElementRow);
	}

	pNodeYKFP->LinkEndChild(pElementRows);

	return true;
}

void CQueryDb::GetListShowData(CListShowData::Pointer &pListShowData) const
{
	CLockList<CInvoiceItem::Pointer>::const_iterator iter;
	BoostReadLock rdLock(const_cast<boost::shared_mutex&>(m_lstInvoiceData.mutex()));
	for(iter = m_lstInvoiceData.begin(); iter != m_lstInvoiceData.end(); iter++)
	{
		const CInvoiceItem::Pointer &pInvoiceItem = *iter;
		map<int, string> mapCol2Text;
		mapCol2Text.insert(std::make_pair(ColCompany, pInvoiceItem->m_strOutCompanyName));
		mapCol2Text.insert(std::make_pair(ColInvoiceType, pInvoiceItem->m_strRealTypeData));
		mapCol2Text.insert(std::make_pair(ColInvoiceStatus, pInvoiceItem->m_strCancelStatus));
		mapCol2Text.insert(std::make_pair(ColInvoiceCode, pInvoiceItem->m_strInvoiceCode));
		mapCol2Text.insert(std::make_pair(ColInvoiceNum, pInvoiceItem->m_strInvoiceNum));
		mapCol2Text.insert(std::make_pair(ColUserName, pInvoiceItem->m_strUserName));
		//mapCol2Text.insert(std::make_pair(ColProductName, pInvoiceItem->m_strProductName));
		mapCol2Text.insert(std::make_pair(ColTaxNum, pInvoiceItem->m_strTaxNum));
		mapCol2Text.insert(std::make_pair(ColTaxSum, pInvoiceItem->m_strTaxSum));
		mapCol2Text.insert(std::make_pair(ColPriceSum, pInvoiceItem->m_strPriceSum));
		mapCol2Text.insert(std::make_pair(ColMakePerson, pInvoiceItem->m_strMakePerson));
		mapCol2Text.insert(std::make_pair(ColMakeDate, pInvoiceItem->m_strMakeDate));
		
		pListShowData->m_vctListData.push_back(mapCol2Text);
	}
}

bool CQueryDb::GetRange(string &strStart, string &strEnd) const
{
	COleDateTime dtCurrent = COleDateTime::GetCurrentTime();

	int nYear = dtCurrent.GetYear();
	int nMonth = dtCurrent.GetMonth();
	int nDay = dtCurrent.GetDay();

	COleDateTime dtStart, dtEnd;

	switch(nMonth)
	{
	case 1:
		{
			/// 去年10月、去年11月、去年12月，本年1月
			dtStart.SetDateTime(nYear - 1, 10, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 2, 1, 0, 0, 0);
		}
		break;
	case 2:
		{
			/// 1月、2月
			dtStart.SetDateTime(nYear, 1, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 3, 1, 0, 0, 0);
		}
		break;
	case 3:
		{
			/// 1月、2月、3月
			dtStart.SetDateTime(nYear, 1, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 4, 1, 0, 0, 0);
		}
		break;
	case 4:
		{
			/// 1月、2月、3月、4月
			dtStart.SetDateTime(nYear, 1, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 5, 1, 0, 0, 0);
		}
		break;
	case 5:
		{
			/// 4月、5月
			dtStart.SetDateTime(nYear, 4, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 6, 1, 0, 0, 0);
		}
		break;
	case 6:
		{
			/// 4月、5月、6月
			dtStart.SetDateTime(nYear, 4, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 7, 1, 0, 0, 0);
		}
		break;
	case 7:
		{
			/// 4月、5月、6月、7月
			dtStart.SetDateTime(nYear, 4, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 8, 1, 0, 0, 0);
		}
		break;
	case 8:
		{
			/// 7月、8月
			dtStart.SetDateTime(nYear, 7, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 9, 1, 0, 0, 0);
		}
		break;
	case 9:
		{
			/// 7月、8月、9月
			dtStart.SetDateTime(nYear, 7, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 10, 1, 0, 0, 0);
		}
		break;
	case 10:
		{
			/// 7月、8月、9月、10月
			dtStart.SetDateTime(nYear, 7, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 11, 1, 0, 0, 0);
		}
		break;
	case 11:
		{
			/// 10月、11月
			dtStart.SetDateTime(nYear, 10, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear, 12, 1, 0, 0, 0);
		}
		break;
	case 12:
		{
			/// 10月、11月、12月
			dtStart.SetDateTime(nYear, 10, 1, 0, 0, 0);
			dtEnd.SetDateTime(nYear + 1, 1, 1, 0, 0, 0);
		}
		break;
	default:
		{
			return false;
		}
		break;
	}

	CString strDateTime;
	//strDateTime.Format("%04d-%02d-%02d %02d:%02d:%02d",
	//	dtStart.GetYear(), dtStart.GetMonth(), dtStart.GetDay(),
	//	dtStart.GetHour(), dtStart.GetMinute(), dtStart.GetSecond());
	strDateTime.Format("%04d-%02d-%02d",
		dtStart.GetYear(), dtStart.GetMonth(), dtStart.GetDay());
	strStart = strDateTime;

	strDateTime.Format("%04d-%02d-%02d",
		dtEnd.GetYear(), dtEnd.GetMonth(), dtEnd.GetDay());
	strEnd = strDateTime;

#ifdef _DEBUG
	strStart = "2010-01-01";
	strEnd = "2018-01-01";
#endif // _DEBUG

	return true;
}
