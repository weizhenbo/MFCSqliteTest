#pragma once
#include "DB/cppsqlite3db.h"

// CRegister 对话框

class CRegister : public CDialogEx
{
	DECLARE_DYNAMIC(CRegister)

public:
	CRegister(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegister();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRegister();
	afx_msg void OnBnClickedBtnLogin();
public:
	bool	Login();
	int		logTime();
	int		initLogtime();
public:
	CppSQLite3DB m_db;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickSyslinkForget(NMHDR *pNMHDR, LRESULT *pResult);
};
