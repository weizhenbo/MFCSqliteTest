#pragma once


// CModifyPwDlg 对话框

class CModifyPwDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModifyPwDlg)

public:
	CModifyPwDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyPwDlg();

// 对话框数据
	enum { IDD = IDD_FORGRT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnModify();
};
