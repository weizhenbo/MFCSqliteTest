
// MysqliteTest2.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMysqliteTest2App: 
// �йش����ʵ�֣������ MysqliteTest2.cpp
//

class CMysqliteTest2App : public CWinApp
{
public:
	CMysqliteTest2App();

// ��д
public:
	virtual BOOL InitInstance();
	CString m_path;
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMysqliteTest2App theApp;