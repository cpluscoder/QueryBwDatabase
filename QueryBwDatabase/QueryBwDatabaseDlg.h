
// QueryBwDatabaseDlg.h : 头文件
//

#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>

// CQueryBwDatabaseDlg 对话框
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

// 构造
public:
	CQueryBwDatabaseDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CQueryBwDatabaseDlg(void);

// 对话框数据
	enum { IDD = IDD_QUERYBWDATABASE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
