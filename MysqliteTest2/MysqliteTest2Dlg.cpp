
// MysqliteTest2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MysqliteTest2.h"
#include "MysqliteTest2Dlg.h"
#include "afxdialogex.h"
#include "DB/DBOperator.h"
#include "DB/cppsqlite3db.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif





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


// CMysqliteTest2Dlg 对话框



CMysqliteTest2Dlg::CMysqliteTest2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMysqliteTest2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMysqliteTest2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_REG_FROM, m_regFrom);
	DDX_Control(pDX, IDC_EDIT_REG_ID, m_regId);
	DDX_Control(pDX, IDC_EDIT_REG_LEVEL, m_regLevel);
	DDX_Control(pDX, IDC_EDIT_REG_NAME, m_regName);
	DDX_Control(pDX, IDC_EDIT_REG_PASS, m_regPass);
	DDX_Control(pDX, IDC_EDIT_REG_USER, m_regUser);
}

BEGIN_MESSAGE_MAP(CMysqliteTest2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_REGISOK, &CMysqliteTest2Dlg::OnBnClickedBtnRegisok)
//	ON_EN_CHANGE(IDC_EDIT_REG_ID, &CMysqliteTest2Dlg::OnEnChangeEditRegId)
END_MESSAGE_MAP()


// CMysqliteTest2Dlg 消息处理程序

BOOL CMysqliteTest2Dlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	InitDataBase();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMysqliteTest2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMysqliteTest2Dlg::OnPaint()
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
HCURSOR CMysqliteTest2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int CMysqliteTest2Dlg::InitDataBase()
{
	USES_CONVERSION;
	CString strsqlCmd;
	//CString strpath;
	//strpath.Format(_T(DATEPATH));
	if (!PathIsDirectory(theApp.m_path))
	{
		CreateDirectory(theApp.m_path, NULL);
	}
	m_db.Open(CStringA(theApp.m_path));
	strsqlCmd.Format(_T("create table if not exists USERINFO (ID INT PRIMARY KEY,LEVEL TEXT,NAME TEXT,ORGANIZATION TEXT,USER TEXT,PASSWORD TEXT)"));
	char *sqlCmd = W2A(strsqlCmd);
	return m_db.ExecDML(sqlCmd);
}



void CMysqliteTest2Dlg::OnBnClickedBtnRegisok()
{
	// TODO:  在此添加控件通知处理程序代码
	//必须输入每一项信息才可登录
	if (noInput())
	{
		AfxMessageBox(_T("请输入完整的个人信息"), MB_OK);
	}
	else
	{
		if (checkID())
		{
			insertData();
			EndDialog(-1);
			m_db.Close();
		}
	}
}

bool CMysqliteTest2Dlg::noInput()
{
	USES_CONVERSION;

	CString str1, str2, str3, str4, str5, str6;
	m_regId.GetWindowText(str1);
	m_regFrom.GetWindowText(str2);
	m_regLevel.GetWindowText(str3);
	m_regName.GetWindowText(str4);
	m_regPass.GetWindowText(str5);
	m_regUser.GetWindowText(str6);

	if (str1 == "" || str2 == "" || str3 == "" || str4 == "" || str5 == "" || str6 == "")
	{
		return true;
	}
	return false;
}

int CMysqliteTest2Dlg::insertData()
{
	USES_CONVERSION;
	CString strsqlCmd("");
	CString user_ID("");
	CString user_NAME("");
	CString user_FROM("");
	CString user_LEVEL("");
	CString user_USER("");
	CString user_PASS("");

	GetDlgItemText(IDC_EDIT_REG_ID, user_ID);
	GetDlgItemText(IDC_EDIT_REG_NAME, user_NAME);
	GetDlgItemText(IDC_EDIT_REG_FROM, user_FROM);
	GetDlgItemText(IDC_EDIT_REG_LEVEL, user_LEVEL);
	GetDlgItemText(IDC_EDIT_REG_PASS, user_PASS);
	GetDlgItemText(IDC_EDIT_REG_USER, user_USER);

	strsqlCmd.Format(_T("insert into USERINFO(ID,LEVEL,NAME,ORGANIZATION,USER,PASSWORD) values('%s','%s','%s','%s','%s','%s')"), user_ID, user_LEVEL, user_NAME, user_FROM, user_USER, user_PASS);
	char *sqlCmd = W2A(strsqlCmd);
	return m_db.ExecDML(sqlCmd);
}

int CMysqliteTest2Dlg::modifyData()//修改个人信息
{
	return 0;
}

bool CMysqliteTest2Dlg::checkID()
{
	USES_CONVERSION;
	CString strID("");
	GetDlgItemText(IDC_EDIT_REG_ID, strID);
	CString strsqlcmd("");
	m_db.Open(CStringA(theApp.m_path));
	strsqlcmd.Format(_T("select *from USERINFO where ID like '%s'"), strID);
	char *sqlCmd = W2A(strsqlcmd);
	CppSQLite3Query cq = m_db.ExecQuery(sqlCmd);
	while (!cq.Eof())
	{
		CString pID = cq.GetStringValue(0);
		if (pID == strID)
		{
			AfxMessageBox(_T("ID号重复，请核对后重新输入！"), MB_OK);
			SetDlgItemText(IDC_EDIT_REG_ID, _T(""));
			return false;
		}
		cq.NextRow();
	}
	return true;
}

