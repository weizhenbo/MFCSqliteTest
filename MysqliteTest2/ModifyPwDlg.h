#pragma once


// CModifyPwDlg �Ի���

class CModifyPwDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModifyPwDlg)

public:
	CModifyPwDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CModifyPwDlg();

// �Ի�������
	enum { IDD = IDD_FORGRT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnModify();
};
