// ModifyPw.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MysqliteTest2.h"
#include "ModifyPwDlg.h"
#include "afxdialogex.h"
#include "DB/DBOperator.h"
#include "DB/cppsqlite3db.h"


// CModifyPwDlg �Ի���

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


// CModifyPwDlg ��Ϣ�������


void CModifyPwDlg::OnBnClickedBtnModify()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
