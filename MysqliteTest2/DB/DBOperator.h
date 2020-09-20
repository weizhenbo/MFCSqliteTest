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
	// 读取一行数据
	bool Read();
	// 关闭Reader，读取结束后调用
	void Close();
	// 记录数
	int GetRecordCount();
	// 总的列数
	int GetColumnCount(void);
	// 获取某列的名称 
	LPCTSTR GetName(int nCol);
	// 获取某列的数据类型
	SQLITE_DATATYPE GetDataType(int nCol);
	// 获取某列的值(字符串)
	LPCTSTR GetStringValue(int nCol);
	// 获取某列的值(整形)
	int GetIntValue(int nCol);
	// 获取某列的值(长整形)
	ULONG64 GetInt64Value(int nCol);
	// 获取某列的值(浮点形)
	double GetFloatValue(int nCol);
	// 获取某列的值(二进制数据)
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
	// 设置命令
	bool SetCommandText(LPCTSTR lpSql);
	bool SetCommandTextA(const char* lpSql);
	bool SetCommandTextW(const wchar_t* lpSql);
	// 绑定参数（index为要绑定参数的序号，从1开始）
	bool BindParam(int index, LPCTSTR value);
	bool BindParam(int index, const int value);
	bool BindParam(int index, ULONG64 value);
	bool BindParam(int index, const double value);
	bool BindParam(int index, const void* blobValue, int nLen);
	// 执行命令
	bool Excute();
	// 清除命令（命令不再使用时需调用该接口清除）
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
	
	// 打开数据库
	bool Open(LPCTSTR lpDbFlie);
	bool OpenA(const char* lpDbFlie);
	bool OpenW(const wchar_t* lpDbFlie);

	// 关闭数据库
	void Close();

	// 执行非查询操作（更新或删除）
	bool ExcuteNonQuery(LPCTSTR lpSql);
	bool ExcuteNonQueryA(const char* lpSql);
	bool ExcuteNonQueryW(const wchar_t* lpSql);

	bool ExcuteNonQuery(SQLiteCommand* pCmd);

	// 查询
	SQLiteDataReader ExcuteQuery(LPCTSTR lpSql);
	SQLiteDataReader ExcuteQueryA(const char* lpSql);
	SQLiteDataReader ExcuteQueryW(const wchar_t* lpSql);

	// 查询（回调方式）
	bool ExcuteQuery(LPCTSTR lpSql, QueryCallback pCallBack);
	bool ExcuteQueryA(const char* lpSql, QueryCallback pCallBack);
	bool ExcuteQueryW(const wchar_t* lpSql, QueryCallback pCallBack);

	// 开始事务
	bool BeginTransaction();
	// 提交事务
	bool CommitTransaction();
	// 回滚事务
	bool RollbackTransaction();

	// 获取上一条错误信息
	LPCSTR GetLastErrorMsg();

	sqlite3* Get() { return m_db; }
private:
	sqlite3 *m_db;
};