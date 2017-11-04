
// QueryBwDatabaseDlg.h : ͷ�ļ�
//

#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>

// CQueryBwDatabaseDlg �Ի���
class CQueryBwDatabaseDlg : public CDialogEx
{
public:
	
protected:
	void CreateList(const std::vector<std::string> &vctColName);
	void ResizeList(void);

	bool QueryDatabase(const std::string &strSql, std::map<int, std::string> &mapColIndex2ColName, std::vector<std::map<int, std::string>> &vctQueryResult);
	void SetListData(const std::vector<std::map<int, std::string>> &vctQueryResult);
	bool Collect(void);

	void EnableCtrl(bool bEnable = true);

	bool GetQueryParam(const std::string &strDbFile, std::string &strDllFile, std::string &strTaxCode);

protected:
	CListCtrl *m_pQueryRstList;
	std::string m_strDbFile;

// ����
public:
	CQueryBwDatabaseDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CQueryBwDatabaseDlg(void);

// �Ի�������
	enum { IDD = IDD_QUERYBWDATABASE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnQuery();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnCollect();
	afx_msg void OnBnClickedBtnSelFile();
};
