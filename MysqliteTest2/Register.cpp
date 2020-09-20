// Register.cpp : 实现文件
//

#include "stdafx.h"
#include "MysqliteTest2.h"
#include "Register.h"
#include "afxdialogex.h"
#include "MysqliteTest2Dlg.h"
#include "ModifyPwDlg.h"


// CRegister 对话框

IMPLEMENT_DYNAMIC(CRegister, CDialogEx)

CRegister::CRegister(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRegister::IDD, pParent)
{

}

CRegister::~CRegister()
{
}

void CRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegister, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_REGISTER, &CRegister::OnBnClickedBtnRegister)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CRegister::OnBnClickedBtnLogin)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_FORGET, &CRegister::OnNMClickSyslinkForget)
END_MESSAGE_MAP()


// CRegister 消息处理程序


void CRegister::OnBnClickedBtnRegister()
{
	// TODO:  在此添加控件通知处理程序代码
	CMysqliteTest2Dlg dlg;
	dlg.DoModal();
}


void CRegister::OnBnClickedBtnLogin()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!Login())
	{
		AfxMessageBox(_T("用户名或密码错误！"), MB_OK);
		SetDlgItemText(IDC_EDIT_PASS, _T(""));
	}
	else
	{
		logTime();
		AfxMessageBox(_T("登陆成功!"), MB_OK);
		m_db.Close();
	}

}

bool CRegister::Login()
{
	USES_CONVERSION;
	CString user;
	CString password;
	CString strsqlcmd("");
	GetDlgItemText(IDC_EDIT_USER, user);
	GetDlgItemText(IDC_EDIT_PASS, password);
	m_db.Open(CStringA(theApp.m_path));
	strsqlcmd.Format(_T("select *from USERINFO where USER like '%s' AND PASSWORD like '%s'"), user, password);
	char *sqlCmd = W2A(strsqlcmd);
	CppSQLite3Query cq = m_db.ExecQuery(sqlCmd);
	while (!cq.Eof())
	{
		CString pUser = cq.GetStringValue(4);
		CString pPass = cq.GetStringValue(5);
		if (pUser == user && pPass == password)
		{
			return true;
		}
		cq.NextRow();
	}
	return false;
}

//记录登录的时间
int CRegister::logTime()
{
	USES_CONVERSION;
	CTime CurrentTime = CTime::GetCurrentTime();
	CString date;
	CString time;
	date.Format(_T("%04d-%02d-%02d"),
		CurrentTime.GetYear(),
		CurrentTime.GetMonth(),
		CurrentTime.GetDay());
	time.Format(_T("%02d:%02d:%02d"),
		CurrentTime.GetHour(),
		CurrentTime.GetMinute(),
		CurrentTime.GetSecond());
	CString strTime;
	strTime = date + " " + time;

	CString user_ID("");
	GetDlgItemText(IDC_EDIT_USER, user_ID);

	CString strsqlCmd;
	strsqlCmd.Format(_T("insert into LOGINTIME(ID,TIME) values('%s','%s')"), user_ID, strTime);
	char *sqlCmd = W2A(strsqlCmd);
	return m_db.ExecDML(sqlCmd);
}

BOOL CRegister::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	initLogtime();
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

int CRegister::initLogtime()
{
	USES_CONVERSION;
	CString strsqlCmd;
	m_db.Open(CStringA(theApp.m_path));
	strsqlCmd.Format(_T("create table if not exists LOGINTIME (ID INT,TIME TEXT PRIMARY KEY)"));
	char *sqlCmd1 = W2A(strsqlCmd);
	return m_db.ExecDML(sqlCmd1);
}

void CRegister::OnNMClickSyslinkForget(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	CModifyPwDlg dlg;
	dlg.DoModal();
	*pResult = 0;
}
