
// QueryBwDatabaseDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CQueryBwDatabaseDlg �Ի���




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


// CQueryBwDatabaseDlg ��Ϣ�������

BOOL CQueryBwDatabaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	GetDlgItem(IDC_EDIT_SQL)->SetWindowText("select * from zzs_fpkj");

	vector<string> vctCol;
	vctCol.push_back("Col1");
	vctCol.push_back("Col2");
	CreateList(vctCol);
	m_pQueryRstList->ShowWindow(SW_SHOW);

	EnableCtrl(false);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CQueryBwDatabaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CQueryBwDatabaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CQueryBwDatabaseDlg::OnBnClickedBtnQuery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
				AfxMessageBox("ȱ��������");
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

	// TODO: �ڴ˴������Ϣ����������
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
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и���(ֻ������report����ListCtrl)
	dwStyle |= LVS_EX_GRIDLINES;//������(ֻ������report����ListCtrl)
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
		OutputDebugString("�����ݿ�ʧ��");
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
					AfxMessageBox("ȱ��������");
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
		ErrorHandle->SetLastError("�����ݿ�ʧ��");
		return false;
	}

	pQueryBwDb->Query();
	pQueryBwDb->Close();

	return true;
}

void CQueryBwDatabaseDlg::OnBnClickedBtnCollect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TRACE("CQueryBwDatabaseDlg::OnBnClickedBtnCollect\n");
	GetDlgItem(IDC_BTN_COLLECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_QUERY)->EnableWindow(FALSE);

	Collect();

	GetDlgItem(IDC_BTN_QUERY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_COLLECT)->EnableWindow(TRUE);
}


void CQueryBwDatabaseDlg::OnBnClickedBtnSelFile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	//string strDbFile = "D:\\Program Files (x86)\\˰�ط�Ʊ��Ʊ���(˰���̰�)\\91320102MA1MB3KE9J\\539903785645.dat";
	if(strDbFile.empty())
	{
		ErrorHandle->SetLastError("���ݿ��ļ���IsNull");
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
	//string strDllFile = "D:\\Program Files (x86)\\˰�ط�Ʊ��Ʊ���(˰���̰�)\\sqlite3.dll";
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
		ErrorHandle->SetLastError("��˰��ʶ���IsNull");
		return false;
	}

	if(strTaxCode.size() != 15 && strTaxCode.size() != 18)
	{
		return false;
	}

	return true;
}
