// ModifyPw.cpp : 实现文件
//

#include "stdafx.h"
#include "MysqliteTest2.h"
#include "ModifyPwDlg.h"
#include "afxdialogex.h"
#include "DB/DBOperator.h"
#include "DB/cppsqlite3db.h"


// CModifyPwDlg 对话框

IMPLEMENT_DYNAMIC(CModifyPwDlg, CDialogEx)

CModifyPwDlg::CModifyPwDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModifyPwDlg::IDD, pParent)
{

}

CModifyPwDlg::~CModifyPwDlg()
{
}

void CModifyPwDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModifyPwDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MODIFY, &CModifyPwDlg::OnBnClickedBtnModify)
END_MESSAGE_MAP()


// CModifyPwDlg 消息处理程序


void CModifyPwDlg::OnBnClickedBtnModify()
{
	// TODO:  在此添加控件通知处理程序代码
}
