#pragma once
#include <windows.h>
#include "..//SQLite3//sqlite3.h"

typedef int(*QueryCallback) (void *para, int n_column, char **column_value, char **column_name);

typedef enum _SQLITE_DATATYPE
{
	SQLITE_DATATYPE_INTEGER = SQLITE_INTEGER,
	SQLITE_DATATYPE_FLOAT  = SQLITE_FLOAT,
	SQLITE_DATATYPE_TEXT  = SQLITE_TEXT,
	SQLITE_DATATYPE_BLOB = SQLITE_BLOB,
	SQLITE_DATATYPE_NULL= SQLITE_NULL,
}SQLITE_DATATYPE;

class CDBOperator;

class SQLiteDataReader
{
public:
	SQLiteDataReader(sqlite3_stmt *pStmt);
	~SQLiteDataReader();
public:
	// ��ȡһ������
	bool Read();
	// �ر�Reader����ȡ���������
	void Close();
	// ��¼��
	int GetRecordCount();
	// �ܵ�����
	int GetColumnCount(void);
	// ��ȡĳ�е����� 
	LPCTSTR GetName(int nCol);
	// ��ȡĳ�е���������
	SQLITE_DATATYPE GetDataType(int nCol);
	// ��ȡĳ�е�ֵ(�ַ���)
	LPCTSTR GetStringValue(int nCol);
	// ��ȡĳ�е�ֵ(����)
	int GetIntValue(int nCol);
	// ��ȡĳ�е�ֵ(������)
	ULONG64 GetInt64Value(int nCol);
	// ��ȡĳ�е�ֵ(������)
	double GetFloatValue(int nCol);
	// ��ȡĳ�е�ֵ(����������)
	const void* GetBlobValue(int nCol, int &nLen);
private:
	sqlite3_stmt *m_pStmt;
};

class SQLiteCommand
{
public:
	SQLiteCommand(CDBOperator* pSqlite);
	SQLiteCommand(CDBOperator* pSqlite, const char* lpSql);
	SQLiteCommand(CDBOperator* pSqlite, const wchar_t* lpSql);
	~SQLiteCommand();
public:
	// ��������
	bool SetCommandText(LPCTSTR lpSql);
	bool SetCommandTextA(const char* lpSql);
	bool SetCommandTextW(const wchar_t* lpSql);
	// �󶨲�����indexΪҪ�󶨲�������ţ���1��ʼ��
	bool BindParam(int index, LPCTSTR value);
	bool BindParam(int index, const int value);
	bool BindParam(int index, ULONG64 value);
	bool BindParam(int index, const double value);
	bool BindParam(int index, const void* blobValue, int nLen);
	// ִ������
	bool Excute();
	// �����������ʹ��ʱ����øýӿ������
	void Clear();

	bool IsOk() { return m_bOk; }
private:
	CDBOperator *m_pSqlite;
	sqlite3_stmt *m_pStmt;
	bool m_bOk;
};

class CDBOperator
{
public:
	friend class SQLiteCommand;

public:
	CDBOperator(void);
	~CDBOperator(void);
	
	// �����ݿ�
	bool Open(LPCTSTR lpDbFlie);
	bool OpenA(const char* lpDbFlie);
	bool OpenW(const wchar_t* lpDbFlie);

	// �ر����ݿ�
	void Close();

	// ִ�зǲ�ѯ���������»�ɾ����
	bool ExcuteNonQuery(LPCTSTR lpSql);
	bool ExcuteNonQueryA(const char* lpSql);
	bool ExcuteNonQueryW(const wchar_t* lpSql);

	bool ExcuteNonQuery(SQLiteCommand* pCmd);

	// ��ѯ
	SQLiteDataReader ExcuteQuery(LPCTSTR lpSql);
	SQLiteDataReader ExcuteQueryA(const char* lpSql);
	SQLiteDataReader ExcuteQueryW(const wchar_t* lpSql);

	// ��ѯ���ص���ʽ��
	bool ExcuteQuery(LPCTSTR lpSql, QueryCallback pCallBack);
	bool ExcuteQueryA(const char* lpSql, QueryCallback pCallBack);
	bool ExcuteQueryW(const wchar_t* lpSql, QueryCallback pCallBack);

	// ��ʼ����
	bool BeginTransaction();
	// �ύ����
	bool CommitTransaction();
	// �ع�����
	bool RollbackTransaction();

	// ��ȡ��һ��������Ϣ
	LPCSTR GetLastErrorMsg();

	sqlite3* Get() { return m_db; }
private:
	sqlite3 *m_db;
};