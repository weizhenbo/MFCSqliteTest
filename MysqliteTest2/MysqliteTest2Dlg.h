
// MysqliteTest2Dlg.h : ͷ�ļ�
//

#pragma once
#include "DB/cppsqlite3db.h"
#include "afxwin.h"
// CMysqliteTest2Dlg �Ի���
class CMysqliteTest2Dlg : public CDialogEx
{
// ����
public:
	CMysqliteTest2Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYSQLITETEST2_DIALOG };

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
	afx_msg void OnBnClickedBtnRegisok();
public:
	int		InitDataBase();
	int		insertData();
	int		modifyData();
	bool	checkID();
	bool	noInput();

public:
	CppSQLite3DB m_db;
	
	CEdit m_regFrom;
	CEdit m_regId;
	CEdit m_regLevel;
	CEdit m_regName;
	CEdit m_regPass;
	CEdit m_regUser;
};
