
// QueryBwDatabaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QueryBwDatabase.h"
#include "QueryBwDatabaseDlg.h"
#include "afxdialogex.h"
#include "QueryBwDb.h"
#include "Utility.h"
#include <boost/filesystem.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define LIST_CTRL_ID				1200
#define COUNT_OF(arr)   (sizeof(arr) / sizeof(arr[0]))

using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CQueryBwDatabaseDlg 对话框




CQueryBwDatabaseDlg::CQueryBwDatabaseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQueryBwDatabaseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pQueryRstList = NULL;
}

CQueryBwDatabaseDlg::~CQueryBwDatabaseDlg(void)
{
	if(m_pQueryRstList)
	{
		delete m_pQueryRstList;
		m_pQueryRstList = NULL;
	}
}

void CQueryBwDatabaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQueryBwDatabaseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_QUERY, &CQueryBwDatabaseDlg::OnBnClickedBtnQuery)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_COLLECT, &CQueryBwDatabaseDlg::OnBnClickedBtnCollect)
	ON_BN_CLICKED(IDC_BTN_SEL_FILE, &CQueryBwDatabaseDlg::OnBnClickedBtnSelFile)
END_MESSAGE_MAP()


// CQueryBwDatabaseDlg 消息处理程序

BOOL CQueryBwDatabaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	GetDlgItem(IDC_EDIT_SQL)->SetWindowText("select * from zzs_fpkj");

	vector<string> vctCol;
	vctCol.push_back("Col1");
	vctCol.push_back("Col2");
	CreateList(vctCol);
	m_pQueryRstList->ShowWindow(SW_SHOW);

	EnableCtrl(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CQueryBwDatabaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQueryBwDatabaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CQueryBwDatabaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CQueryBwDatabaseDlg::OnBnClickedBtnQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_COLLECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_QUERY)->EnableWindow(FALSE);
	CString strText;
	GetDlgItem(IDC_EDIT_SQL)->GetWindowText(strText);
	string strSql = strText;

	do 
	{
		if(strSql.empty())
		{
			break;
		}
		map<int, string> mapColIndex2ColName;
		vector<map<int, string>> vctQueryResult;
		CUtility::GbkToUtf8(strSql);

		if(!QueryDatabase(strSql, mapColIndex2ColName, vctQueryResult))
		{
			break;
		}

		if(mapColIndex2ColName.empty())
		{
			break;
		}

		vector<string> vctColName;
		int nColCnt = mapColIndex2ColName.size();
		for(int i = 0; i < nColCnt; i++)
		{
			auto iter = mapColIndex2ColName.find(i);
			if(iter != mapColIndex2ColName.end())
			{
				vctColName.push_back(iter->second);
			}
			else
			{
				AfxMessageBox("缺少列名称");
				break;
			}
		}
		CreateList(vctColName);
		SetListData(vctQueryResult);
		m_pQueryRstList->ShowWindow(SW_SHOW);

	} while (false);

	GetDlgItem(IDC_BTN_QUERY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_COLLECT)->EnableWindow(TRUE);
}

void CQueryBwDatabaseDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ResizeList();
}

void CQueryBwDatabaseDlg::CreateList(const vector<string> &vctColName)
{
	if(vctColName.empty())
	{
		return;
	}

	const int LIST_CTRL_ID = 1200;
	//static int nListCtrlId = LIST_CTRL_ID;

	if(m_pQueryRstList != NULL)
	{
		delete m_pQueryRstList;
		m_pQueryRstList = NULL;
	}
	m_pQueryRstList = new CListCtrl;
	if(m_pQueryRstList != NULL)
	{
		m_pQueryRstList->Create(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_EDITLABELS, CRect(5, 5, 100, 100), this, LIST_CTRL_ID/*nListCtrlId++*/);
	}

	DWORD dwStyle = 0;
	dwStyle = m_pQueryRstList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮(只适用与report风格的ListCtrl)
	dwStyle |= LVS_EX_GRIDLINES;//网格线(只适用与report风格的ListCtrl)
	m_pQueryRstList->SetExtendedStyle(dwStyle);

	int i = 0;
	for(auto iter = vctColName.begin(); iter != vctColName.end(); iter++, i++)
	{
		const string &strName = *iter;
		m_pQueryRstList->InsertColumn(i, strName.c_str(), LVCFMT_LEFT);
		m_pQueryRstList->SetColumnWidth(i, 80);
	}

	ResizeList();
}

void CQueryBwDatabaseDlg::ResizeList(void)
{
	do 
	{
		if(m_pQueryRstList == NULL)
		{
			break;
		}
		if(m_pQueryRstList->GetSafeHwnd() == NULL)
		{
			break;
		}

		CRect rc;
		GetClientRect(&rc);

		m_pQueryRstList->MoveWindow(16, 60, rc.Width() - 32, rc.Height() - 80);

	} while (false);
}

bool CQueryBwDatabaseDlg::QueryDatabase(const string &strSql, map<int, string> &mapColIndex2ColName, vector<map<int, string>> &vctQueryResult)
{
	string strDllFile, strTaxCode;
	if(!GetQueryParam(m_strDbFile, strDllFile, strTaxCode))
	{
		return false;
	}

	CQueryBwDb::Pointer pQueryBwDb = CQueryBwDb::Create();

	OutputDebugString("----Query Database----\n");
	CString strMsg;
	strMsg.Format("File:%s\nTaxCode:%s\nDll:%s\n", m_strDbFile.c_str(), strTaxCode.c_str(), strDllFile.c_str());
	OutputDebugString(strMsg);
	OutputDebugString("\n");

	bool bGetNewData = false;
	if(!pQueryBwDb->Open(m_strDbFile, strTaxCode, strDllFile))
	{
		OutputDebugString("打开数据库失败");
		return false;
	}

	pQueryBwDb->Query(strSql, mapColIndex2ColName, vctQueryResult);
	pQueryBwDb->Close();

	return true;
}

void CQueryBwDatabaseDlg::SetListData(const vector<map<int, string>> &vctQueryResult)
{
	do 
	{
		if(m_pQueryRstList == NULL)
		{
			break;
		}
		if(m_pQueryRstList->GetSafeHwnd() == NULL)
		{
			break;
		}
		m_pQueryRstList->DeleteAllItems();

		int nListIndex = 0;
		for(auto iter = vctQueryResult.begin(); iter != vctQueryResult.end(); iter++)
		{
			const map<int, string> &mapRow = *iter;
			int nColIndex = 0;
			int nColCnt = mapRow.size();
			for(nColIndex = 0; nColIndex < nColCnt; nColIndex++)
			{
				auto iterTemp = mapRow.find(nColIndex);
				if(iterTemp == mapRow.end())
				{
					AfxMessageBox("缺少列数据");
					continue;
				}
				if(nColIndex == 0)
				{
					m_pQueryRstList->InsertItem(nListIndex, iterTemp->second.c_str());
				}
				else
				{
					m_pQueryRstList->SetItemText(nListIndex, iterTemp->first, iterTemp->second.c_str());
				}
			}
			nListIndex++;
		}
	} while (false);
}

bool CQueryBwDatabaseDlg::Collect(void)
{
	string strDllFile, strTaxCode;
	if(!GetQueryParam(m_strDbFile, strDllFile, strTaxCode))
	{
		return false;
	}

	TRACE("----Query Database----");
	TRACE("File:%s\nTaxCode:%s\nDll:%s\n", m_strDbFile.c_str(), strTaxCode.c_str(), strDllFile.c_str());

	CQueryBwDb::Pointer pQueryBwDb = CQueryBwDb::Create();
	bool bGetNewData = false;
	if(!pQueryBwDb->Open(m_strDbFile, strTaxCode, strDllFile))
	{
		ErrorHandle->SetLastError("打开数据库失败");
		return false;
	}

	pQueryBwDb->Query();
	pQueryBwDb->Close();

	return true;
}

void CQueryBwDatabaseDlg::OnBnClickedBtnCollect()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE("CQueryBwDatabaseDlg::OnBnClickedBtnCollect\n");
	GetDlgItem(IDC_BTN_COLLECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_QUERY)->EnableWindow(FALSE);

	Collect();

	GetDlgItem(IDC_BTN_QUERY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_COLLECT)->EnableWindow(TRUE);
}


void CQueryBwDatabaseDlg::OnBnClickedBtnSelFile()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilters[]= _T("All Files (*.*)|*.*||");

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if(fileDlg.DoModal() == IDOK)
	{
		m_strDbFile = fileDlg.GetPathName();
		if(!m_strDbFile.empty())
		{
			EnableCtrl();
		}
	}
}

void CQueryBwDatabaseDlg::EnableCtrl(bool bEnable)
{
	GetDlgItem(IDC_BTN_QUERY)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_COLLECT)->EnableWindow(bEnable);
}

bool CQueryBwDatabaseDlg::GetQueryParam(const string &strDbFile, string &strDllFile, string &strTaxCode)
{
	//string strDbFile = "D:\\Program Files (x86)\\税控发票开票软件(税控盘版)\\91320102MA1MB3KE9J\\539903785645.dat";
	if(strDbFile.empty())
	{
		ErrorHandle->SetLastError("数据库文件名IsNull");
		return false;
	}

	boost::filesystem::path fsPath(strDbFile);
	if(!fsPath.has_parent_path())
	{
		return false;
	}

	boost::filesystem::path fsTaxCode = fsPath.parent_path();
	if(!fsTaxCode.has_parent_path())
	{
		return false;
	}

	boost::filesystem::path fsInstall = fsTaxCode.parent_path();
	//string strDllFile = "D:\\Program Files (x86)\\税控发票开票软件(税控盘版)\\sqlite3.dll";
	strDllFile = fsInstall.string();
	strDllFile.append("\\sqlite3.dll");
	boost::filesystem::path fsDll(strDllFile);
	if(!boost::filesystem::exists(fsDll))
	{
		return false;
	}

	//string strTaxCode = "91320102MA1MB3KE9J";
	strTaxCode = fsTaxCode.stem().string();
	if(strTaxCode.empty())
	{
		ErrorHandle->SetLastError("纳税人识别号IsNull");
		return false;
	}

	if(strTaxCode.size() != 15 && strTaxCode.size() != 18)
	{
		return false;
	}

	return true;
}
