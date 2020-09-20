#include "DBOperator.h"


const char* WcharToACP(const wchar_t *pwStr)
{
	if (pwStr == NULL)
	{
		return NULL;
	}

	int len = WideCharToMultiByte(CP_ACP, 0, pwStr, -1, NULL, 0, NULL, NULL);
	if (len <= 0)
	{
		return NULL;
	}
	char *pStr = new char[len];
	WideCharToMultiByte(CP_ACP, 0, pwStr, -1, pStr, len, NULL, NULL);
	return pStr;
}

const wchar_t* Utf8ToWchar(const char *pStr)
{
	if (pStr == NULL)
	{
		return NULL;
	}

	int len = MultiByteToWideChar(CP_UTF8, 0, pStr, -1, NULL, 0);
	if (len <= 0)
	{
		return NULL;
	}
	wchar_t *pwStr = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, pStr, -1, pwStr, len);
	return pwStr;
}

CDBOperator::CDBOperator(void):
	m_db(NULL)
{
}

CDBOperator::~CDBOperator(void)
{
	Close();
}

bool CDBOperator::Open(LPCTSTR lpDbFlie)
{
#ifdef  UNICODE 
	return OpenW(lpDbFlie);
#else
	return OpenA(lpDbFlie);
#endif
}

bool CDBOperator::OpenA(const char* lpDbFlie)
{
	if( NULL ==lpDbFlie )
		return false;

	return ( SQLITE_OK == sqlite3_open(lpDbFlie,&m_db) );
}

bool CDBOperator::OpenW(const wchar_t* lpDbFlie)
{
	if( NULL ==lpDbFlie )
		return false;

	return ( SQLITE_OK == sqlite3_open16(lpDbFlie,&m_db) );
}

void CDBOperator::Close()
{
	if( NULL != m_db )
		sqlite3_close(m_db);
	
	m_db = NULL;
}

bool CDBOperator::ExcuteNonQuery(LPCTSTR lpSql)
{
#ifdef  UNICODE 
	return ExcuteNonQueryW(lpSql);
#else
	return ExcuteNonQueryA(lpSql);
#endif
}

bool CDBOperator::ExcuteNonQueryA(const char* lpSql)
{
	sqlite3_stmt* stmt;
	if ( sqlite3_prepare_v2(m_db, lpSql, -1, &stmt, NULL) != SQLITE_OK )
	{
		return false;
	}
	sqlite3_step(stmt);

	return (sqlite3_finalize(stmt) == SQLITE_OK) ? true : false;
}



bool CDBOperator::ExcuteNonQueryW(const wchar_t* lpSql)
{
	sqlite3_stmt* stmt;
	if ( sqlite3_prepare16_v2(m_db, lpSql, -1, &stmt, NULL) != SQLITE_OK )
	{ 
		return false;
	}
	sqlite3_step(stmt);

	return (sqlite3_finalize(stmt) == SQLITE_OK) ? true : false;
}

bool CDBOperator::ExcuteNonQuery(SQLiteCommand* pCmd)
{
	if( NULL == pCmd )
		return false;
	
	return pCmd->Excute();
}

// 查询
SQLiteDataReader CDBOperator::ExcuteQuery(LPCTSTR lpSql)
{
#ifdef  UNICODE 
	return ExcuteQueryW(lpSql);
#else
	return ExcuteQueryA(lpSql);
#endif
}


SQLiteDataReader CDBOperator::ExcuteQueryA(const char* lpSql)
{
	sqlite3_stmt* stmt = NULL;
	
	if( NULL == lpSql )
		return SQLiteDataReader(stmt);

	if( sqlite3_prepare_v2(m_db, lpSql, -1, &stmt, NULL) != SQLITE_OK )
		return SQLiteDataReader(NULL);
	return SQLiteDataReader(stmt);
}

SQLiteDataReader CDBOperator::ExcuteQueryW(const wchar_t* lpSql)
{
	if( NULL == lpSql )
		return false;

	sqlite3_stmt* stmt;
	if( sqlite3_prepare16_v2(m_db, lpSql, -1, &stmt, NULL) != SQLITE_OK )
		return false;
	return SQLiteDataReader(stmt);
}


// 查询(回调方式)
bool CDBOperator::ExcuteQuery(LPCTSTR lpSql, QueryCallback pCallBack)
{
#ifdef  UNICODE 
	return ExcuteQueryW(lpSql, pCallBack);
#else
	return ExcuteQueryA(lpSql, pCallBack);
#endif
}

bool CDBOperator::ExcuteQueryA(const char* lpSql, QueryCallback pCallBack)
{
	if( NULL == lpSql || NULL == pCallBack )
		return false;

	return (SQLITE_OK == sqlite3_exec(m_db, lpSql, pCallBack, NULL, NULL));	//参数：已经打开的数据库，SQL语句，回调函数，传递给回调函数的参数（指针），错误信息
}

bool CDBOperator::ExcuteQueryW(const wchar_t* lpSql, QueryCallback pCallBack)
{
	const char *szSql = WcharToACP(lpSql);
	bool bOk = ExcuteQueryA(szSql, pCallBack);
	delete[] szSql;

	return bOk;
}


// 开始事务
bool CDBOperator::BeginTransaction()
{
	return (sqlite3_exec(m_db, "BEGIN TRANSACTION;", NULL, NULL, NULL) == SQLITE_OK);
}

// 提交事务
bool CDBOperator::CommitTransaction()
{
	return (sqlite3_exec(m_db, "COMMIT TRANSACTION;", NULL, NULL, NULL) == SQLITE_OK);
}

// 回滚事务
bool CDBOperator::RollbackTransaction()
{
	return (sqlite3_exec(m_db, "ROLLBACK  TRANSACTION;", NULL, NULL, NULL) == SQLITE_OK);
}

// 获取上一条错误信息
LPCSTR CDBOperator::GetLastErrorMsg()
{
	return sqlite3_errmsg(m_db);
}

SQLiteDataReader::SQLiteDataReader(sqlite3_stmt *pStmt):
	m_pStmt(pStmt)
{
}

SQLiteDataReader::~SQLiteDataReader()
{
	Close();
}

// 读取一行数据
bool SQLiteDataReader::Read()
{
	if( NULL == m_pStmt )
		return false;
	
	return (sqlite3_step(m_pStmt) == SQLITE_ROW);
}

// 关闭Reader，读取结束后调用
void SQLiteDataReader::Close()
{
	if( NULL != m_pStmt )
		sqlite3_finalize(m_pStmt);

	m_pStmt = NULL;
}

// fuck this!!! return column count not record count!!!
int SQLiteDataReader::GetRecordCount(void)
{
	if( NULL == m_pStmt )
		return 0;

	return sqlite3_data_count(m_pStmt);
}

// 总的列数
int SQLiteDataReader::GetColumnCount(void)
{
	if( NULL == m_pStmt )
		return 0;

	return sqlite3_column_count(m_pStmt);
}

// 获取某列的名称 
LPCTSTR SQLiteDataReader::GetName(int nCol)
{
	if( NULL == m_pStmt )
		return 0;

#ifdef  UNICODE 
	return (LPCTSTR)sqlite3_column_name16(m_pStmt, nCol);
#else
	return (LPCTSTR)sqlite3_column_name(m_pStmt, nCol);
#endif
}

// 获取某列的数据类型
SQLITE_DATATYPE SQLiteDataReader::GetDataType(int nCol)
{
	if( NULL == m_pStmt )
		return SQLITE_DATATYPE_NULL;

	return (SQLITE_DATATYPE)sqlite3_column_type(m_pStmt, nCol);
}

// 获取某列的值(字符串)
LPCTSTR SQLiteDataReader::GetStringValue(int nCol)
{
	if( NULL == m_pStmt )
		return NULL;

#ifdef  UNICODE 
	return (LPCTSTR)sqlite3_column_text16(m_pStmt, nCol);
#else
	return (LPCTSTR)sqlite3_column_text(m_pStmt, nCol);
#endif
}

// 获取某列的值(整形)
int SQLiteDataReader::GetIntValue(int nCol)
{
	if( NULL == m_pStmt )
		return 0;

	return sqlite3_column_int(m_pStmt, nCol);
}

// 获取某列的值(长整形)
ULONG64 SQLiteDataReader::GetInt64Value(int nCol)
{
	if( NULL == m_pStmt )
		return 0;

	return sqlite3_column_int64(m_pStmt, nCol);
}

// 获取某列的值(浮点形)
double SQLiteDataReader::GetFloatValue(int nCol)
{
	if( NULL == m_pStmt )
		return 0;

	return sqlite3_column_double(m_pStmt, nCol);
}

// 获取某列的值(二进制数据)
const void* SQLiteDataReader::GetBlobValue(int nCol, int &nLen)
{
	if( NULL == m_pStmt )
		return NULL;

	nLen = sqlite3_column_bytes(m_pStmt, nCol);
	return sqlite3_column_blob(m_pStmt, nCol);
}


//////////////////////////////////////////////////////////////////////////
SQLiteCommand::SQLiteCommand(CDBOperator* pSqlite):
	m_pSqlite(pSqlite),
	m_pStmt(NULL)
{
	m_bOk = true;
}

SQLiteCommand::SQLiteCommand(CDBOperator* pSqlite, const char* lpSql):
	m_pSqlite(pSqlite),
	m_pStmt(NULL)
{
	m_bOk = SetCommandTextA(lpSql);
}

SQLiteCommand::SQLiteCommand(CDBOperator* pSqlite, const wchar_t* lpSql):
	m_pSqlite(pSqlite),
	m_pStmt(NULL)
{
	m_bOk = SetCommandTextW(lpSql);
}

SQLiteCommand::~SQLiteCommand()
{
	Clear();
}

bool SQLiteCommand::SetCommandText(LPCTSTR lpSql)
{
#ifdef UNICODE 
	return SetCommandTextW(lpSql);
#else
	return SetCommandTextA(lpSql);
#endif
}

bool SQLiteCommand::SetCommandTextA(const char* lpSql)
{
	return (SQLITE_OK == sqlite3_prepare_v2(m_pSqlite->m_db, lpSql, -1, &m_pStmt, NULL));
}

bool SQLiteCommand::SetCommandTextW(const wchar_t* lpSql)
{
	return (SQLITE_OK == sqlite3_prepare16_v2(m_pSqlite->m_db, lpSql, -1, &m_pStmt, NULL));
}

bool SQLiteCommand::BindParam(int index, LPCTSTR value)
{
	if( NULL == m_pStmt )
		return false;

#ifdef  UNICODE 
	return (SQLITE_OK == sqlite3_bind_text16(m_pStmt, index, value, -1, SQLITE_TRANSIENT));
#else
	return (SQLITE_OK == sqlite3_bind_text(m_pStmt, index, value,-1, SQLITE_TRANSIENT));
#endif
}

bool SQLiteCommand::BindParam(int index, int value)
{
	if( NULL == m_pStmt )
		return false;

	return (SQLITE_OK == sqlite3_bind_int(m_pStmt, index, value));
}

bool SQLiteCommand::BindParam(int index, ULONG64 value)
{
	if( NULL == m_pStmt )
		return false;

	return (SQLITE_OK == sqlite3_bind_int64(m_pStmt, index, value));
}

bool SQLiteCommand::BindParam(int index, const double value)
{
	if( NULL == m_pStmt )
		return false;

	return (SQLITE_OK == sqlite3_bind_double(m_pStmt, index, value));
}

bool SQLiteCommand::BindParam(int index, const void* blobBuf, int nLen)
{
	if( NULL == m_pStmt )
		return false;

	return (SQLITE_OK == sqlite3_bind_blob(m_pStmt, index, blobBuf, nLen, NULL));
}

bool SQLiteCommand::Excute()
{
	if( NULL == m_pStmt )
		return false;

	sqlite3_step(m_pStmt);
	return (SQLITE_OK == sqlite3_reset(m_pStmt));
}

void SQLiteCommand::Clear()
{
	if(NULL != m_pStmt)
		sqlite3_finalize(m_pStmt);
	
	m_pStmt = NULL;
}


