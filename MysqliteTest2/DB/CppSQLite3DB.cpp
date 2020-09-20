#include "../stdafx.h"
#include "CppSQLite3DB.h"
#include <cstdlib>

static const bool DONT_DELETE_MSG=false;

CppSQLite3Exception::CppSQLite3Exception(const int nErrNode,
	const char *szErrMess,
	bool bDeleteMess)
{
	mnErrCode = nErrNode;
	mpszErrMessage = sqlite3_mprintf("%s[%d]: %s",
		ErrorCodeAsString(nErrNode),
		mnErrCode,
		szErrMess ? szErrMess : "");
	if (bDeleteMess && szErrMess)
	{
		sqlite3_free(mpszErrMessage);
	}
}

CppSQLite3Exception::CppSQLite3Exception(const CppSQLite3Exception &e)
{
	mnErrCode = e.mnErrCode;
	mpszErrMessage = 0;
	if (e.mpszErrMessage)
	{
		mpszErrMessage = sqlite3_mprintf("%s", e.mpszErrMessage);
	}
}

const char* CppSQLite3Exception::ErrorCodeAsString(const int nErrCode)
{
	switch (nErrCode)
	{
	case SQLITE_OK          : return "SQLITE_OK";
	case SQLITE_ERROR       : return "SQLITE_ERROR";
	case SQLITE_INTERNAL    : return "SQLITE_INTERNAL";
	case SQLITE_PERM        : return "SQLITE_PERM";
	case SQLITE_ABORT       : return "SQLITE_ABORT";
	case SQLITE_BUSY        : return "SQLITE_BUSY";
	case SQLITE_LOCKED      : return "SQLITE_LOCKED";
	case SQLITE_NOMEM       : return "SQLITE_NOMEM";
	case SQLITE_READONLY    : return "SQLITE_READONLY";
	case SQLITE_INTERRUPT   : return "SQLITE_INTERRUPT";
	case SQLITE_IOERR       : return "SQLITE_IOERR";
	case SQLITE_CORRUPT     : return "SQLITE_CORRUPT";
	case SQLITE_NOTFOUND    : return "SQLITE_NOTFOUND";
	case SQLITE_FULL        : return "SQLITE_FULL";
	case SQLITE_CANTOPEN    : return "SQLITE_CANTOPEN";
	case SQLITE_PROTOCOL    : return "SQLITE_PROTOCOL";
	case SQLITE_EMPTY       : return "SQLITE_EMPTY";
	case SQLITE_SCHEMA      : return "SQLITE_SCHEMA";
	case SQLITE_TOOBIG      : return "SQLITE_TOOBIG";
	case SQLITE_CONSTRAINT  : return "SQLITE_CONSTRAINT";
	case SQLITE_MISMATCH    : return "SQLITE_MISMATCH";
	case SQLITE_MISUSE      : return "SQLITE_MISUSE";
	case SQLITE_NOLFS       : return "SQLITE_NOLFS";
	case SQLITE_AUTH        : return "SQLITE_AUTH";
	case SQLITE_FORMAT      : return "SQLITE_FORMAT";
	case SQLITE_RANGE       : return "SQLITE_RANGE";
	case SQLITE_ROW         : return "SQLITE_ROW";
	case SQLITE_DONE        : return "SQLITE_DONE";
	case CPPSQLITE_ERROR    : return "CPPSQLITE_ERROR";
	default: return "UNKNOWN_ERROR";
	}
}

CppSQLite3Exception::~CppSQLite3Exception()
{
	if (mpszErrMessage)
	{
		sqlite3_free(mpszErrMessage);
		mpszErrMessage = 0;
	}
}
CppSQLite3Query::CppSQLite3Query()
{
	mpDB = 0;
	mpStmt = 0;
	mnCols = 0;
	mbEof = true;
}

CppSQLite3Query::CppSQLite3Query(sqlite3 *pdb, sqlite3_stmt *pStmt, bool bEof)
{
	mpDB = pdb;
	mpStmt = pStmt;
	mbEof = bEof;
	mnCols = sqlite3_column_count(pStmt);
}

CppSQLite3Query::CppSQLite3Query(const CppSQLite3Query &rQuery)
{
	mpStmt = rQuery.mpStmt;
	const_cast<CppSQLite3Query &>(rQuery).mpStmt = 0;
	mnCols = rQuery.mnCols;
	mbEof = rQuery.mbEof;
}

CppSQLite3Query& CppSQLite3Query::operator =(const CppSQLite3Query &rQuery)
{
	mpStmt = rQuery.mpStmt;
	const_cast<CppSQLite3Query &>(rQuery).mpStmt = 0;
	mnCols = rQuery.mnCols;
	mbEof = rQuery.mbEof;

	return *this;
}

CppSQLite3Query::~CppSQLite3Query()
{
	Finalize();
}

void CppSQLite3Query::CheckStmt()
{
	if (mpStmt == 0)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid Stmt Pointer",
			DONT_DELETE_MSG);
	}
}

int CppSQLite3Query::FieldNums()//列数
{
	CheckStmt();

	return mnCols;
}

//根据字段名返回列索引
int CppSQLite3Query::FieldIndex(const char* szField)
{
	CheckStmt();

	if (szField)
	{
		for (int nField = 0; nField < mnCols; nField++)
		{
			//后面还有很多类似的函数，参数差不多，需要一个sqlite3_stmt*和列索引值，这应该是内部查询了之后返回的结果，而不是事先保存的
			const char *szTemp = sqlite3_column_name(mpStmt, nField);
			if (strcmp(szTemp, szField) == 0)
			{
				return nField;
			}
		}
	}

	throw CppSQLite3Exception(CPPSQLITE_ERROR,
		"Invalid field name requested",
		DONT_DELETE_MSG);
}
const char* CppSQLite3Query::FieldName(int nField)//列名
{
	CheckStmt();

	if (nField < 0 || nField > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid field index requested",
			DONT_DELETE_MSG);
	}

	return sqlite3_column_name(mpStmt, nField);
}

int CppSQLite3Query::FieldDataType(int nField)//列数据类型
{
	CheckStmt();

	if (nField < 0 || nField > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid field index requested",
			DONT_DELETE_MSG);
	}

	return sqlite3_column_type(mpStmt, nField);
}
const char* CppSQLite3Query::FieldDeclType(int nField)
{
	CheckStmt();

	if (nField < 0 || nField > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid field index requested",
			DONT_DELETE_MSG);
	}

	return sqlite3_column_decltype(mpStmt, nField);
}

const char* CppSQLite3Query::FieldValue(int nField)//根据列索引访问某一列的数据
{
	CheckStmt();

	if (nField < 0 || nField > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid field index requested",
			DONT_DELETE_MSG);
	}

	return (const char*)sqlite3_column_text(mpStmt, nField);

}

const char* CppSQLite3Query::FieldValue(const char *szField)//根据字段名称来访问某一列的数据
{
	int nField = FieldIndex(szField);
	return FieldValue(nField);
}

bool CppSQLite3Query::FieldIsNull(int nField)
{
	return (FieldDataType(nField) == SQLITE_NULL);
}

bool CppSQLite3Query::FieldIsNull(const char *szField)//根据字段名称判断某一列是否为空
{
	int nField = FieldIndex(szField);
	return (FieldDataType(nField) == SQLITE_NULL);
}

bool CppSQLite3Query::GetIntValue(int nField, int &rDest)
{
	if (FieldDataType(nField) == SQLITE_NULL)
	{
		return false;
	}
	else
	{
		rDest = sqlite3_column_int(mpStmt, nField);
		return true;
	}
}

bool CppSQLite3Query::GetIntValue(const char *szField, int &rDest)//根据字段名称获取某一列是否的整形值
{
	int nField = FieldIndex(szField);
	return GetIntValue(nField, rDest);
}

bool CppSQLite3Query::GetFloatValue(int nField, double &rDest)
{
	if (FieldDataType(nField) == SQLITE_NULL)
	{
		return false;
	}
	else
	{
		rDest = sqlite3_column_double(mpStmt, nField);
		return true;
	}
}
bool CppSQLite3Query::GetFloatValue(const char *szField, double &rDest)//根据字段名称获取某一列是否的浮点值
{
	int nField = FieldIndex(szField);
	return GetFloatValue(nField, rDest);
}


//bool CppSQLite3Query::GetStringValue(int nField, char *&rDest)
//{
//	if (FieldDataType(nField) == SQLITE_NULL)
//	{
//		return false;
//	}
//	else
//	{
//		rDest = const_cast<char *>((const char*)sqlite3_column_text(mpStmt, nField));
//		return true;
//	}
//}
//bool CppSQLite3Query::GetStringValue(const char *szField, char *&rDest)//根据字段名称获取某一列是否的字符串值
//{
//	int nField = FieldIndex(szField);
//	return GetStringValue(nField, rDest);
//}

LPCTSTR CppSQLite3Query::GetStringValue(int nCol)
{
	if (NULL == mpStmt)
		return NULL;

#ifdef  UNICODE 
	return (LPCTSTR)sqlite3_column_text16(mpStmt, nCol);
#else
	return (LPCTSTR)sqlite3_column_text(mpStmt, nCol);
#endif
}


bool CppSQLite3Query::Eof()//判断是否达到最后一行的结尾
{
	CheckStmt();

	return mbEof;
}

void CppSQLite3Query::NextRow()//判断下一行是否为空
{
	CheckStmt();

	int nRet = sqlite3_step(mpStmt);

	if (nRet == SQLITE_DONE)//为空
	{
		// no rows
		mbEof = true;
	}
	else if (nRet == SQLITE_ROW)//不为空
	{
		// more rows, nothing to do
	}
	else
	{
		nRet = sqlite3_finalize(mpStmt);
		mpStmt = 0;
		const char* szError = sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}
}

void CppSQLite3Query::Finalize()//释放数组内存
{
	if (mpStmt)
	{
		int nRet = sqlite3_finalize(mpStmt);
		mpStmt = 0;
		if (nRet != SQLITE_OK)
		{
			const char* szError = sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
		}
	}
}
CppSQLite3Statement::CppSQLite3Statement()
{
	mpDB = 0;
	mpStmt = 0;
}

CppSQLite3Statement::CppSQLite3Statement(sqlite3 *pdb, sqlite3_stmt *pstmt)
{
	mpDB = pdb;
	mpStmt = pstmt;
}

CppSQLite3Statement::CppSQLite3Statement(const CppSQLite3Statement &rStatement)
{
	mpDB = rStatement.mpDB;
	mpStmt = rStatement.mpStmt;
	const_cast<CppSQLite3Statement &>(rStatement).mpStmt = 0;
}

CppSQLite3Statement::~CppSQLite3Statement()
{

}

CppSQLite3Statement& CppSQLite3Statement::operator = (const CppSQLite3Statement &rStatement)
{
	mpDB = rStatement.mpDB;
	mpStmt = rStatement.mpStmt;
	const_cast<CppSQLite3Statement &>(rStatement).mpStmt = 0;
	return *this;
}

int CppSQLite3Statement::ExecDML()
{
	CheckDB();
	CheckStmt();

	const char *szError = 0;

	int nRet = sqlite3_step(mpStmt);

	if (nRet == SQLITE_DONE)
	{
		int nRowChanged = sqlite3_changes(mpDB);  //返回受影响的行数

		nRet = sqlite3_reset(mpStmt);  //重置sqlite3_stmt*对象
		if (nRet != SQLITE_OK)
		{
			szError = sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
		}

		return nRowChanged;
	} 
	else
	{
		nRet = sqlite3_reset(mpStmt);
		szError = sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}
}


CppSQLite3Query CppSQLite3Statement::ExecQuery()
{
	CheckDB();
	CheckStmt();

	int nRet = sqlite3_step(mpStmt);
	if (nRet == SQLITE_DONE)
	{
		return CppSQLite3Query(mpDB, mpStmt, true);//返回一个查询，没有查询结果，所以置为true
	}
	else if (nRet == SQLITE_ROW)
	{
		return CppSQLite3Query(mpDB, mpStmt, false);//查询是有结果的，返回false代表这不允许删除次查询对象
	}
	else
	{
		nRet = sqlite3_reset(mpStmt);
		const char* szError = sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}
}

void CppSQLite3Statement::CheckDB()
{
	if (mpDB == 0)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Database not open",
			DONT_DELETE_MSG);
	}
}

void CppSQLite3Statement::CheckStmt()
{
	if (mpStmt == 0)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Null stmt pointer",
			DONT_DELETE_MSG);
	}
}

void CppSQLite3Statement::Reset()
{
	if (mpStmt)
	{
		int nRet = sqlite3_reset(mpStmt);

		if (nRet != SQLITE_OK)
		{
			const char* szError = sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
		}
	}
}

void CppSQLite3Statement::Finalize()
{
	if (mpStmt)
	{
		int nRet = sqlite3_finalize(mpStmt);
		mpStmt = 0;

		if (nRet != SQLITE_OK)
		{
			const char* szError = sqlite3_errmsg(mpDB);
			throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
		}
	}
}
CppSQLite3DB::CppSQLite3DB()
{
	mpDB = 0;
}

CppSQLite3DB::CppSQLite3DB(const CppSQLite3DB &rDB)
{
	mpDB = rDB.mpDB;
}

CppSQLite3DB::~CppSQLite3DB()
{
	Close();
}

CppSQLite3DB& CppSQLite3DB::operator =(const CppSQLite3DB &rDB)
{
	mpDB = rDB.mpDB;
	return *this;
}

void CppSQLite3DB::CheckDB()
{
	if (!mpDB)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Database not open",
			DONT_DELETE_MSG);
	}
}

sqlite3_stmt* CppSQLite3DB::Compile(const char *szSQL)
{
	CheckDB();

	const char *szTail = 0;
	sqlite3_stmt *pStmt;

	int nRet = sqlite3_prepare(mpDB, szSQL, -1, &pStmt, &szTail);

	if (nRet != SQLITE_OK)
	{
		const char *szError = sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}

	return pStmt;
}

void CppSQLite3DB::Open(const char *szFileName)
{
	int nRet = sqlite3_open(szFileName, &mpDB);
	if (nRet != SQLITE_OK)
	{
		const char *szError = sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}
}

void CppSQLite3DB::Close()
{
	if (mpDB)
	{
		if (sqlite3_close(mpDB) == SQLITE_OK)
		{
			mpDB = 0; //一旦关闭数据库指针，要置为0，防止多次关闭出错
		}
		else
		{
			throw CppSQLite3Exception(CPPSQLITE_ERROR,
				"Unable to close database",
				DONT_DELETE_MSG);
		}
	}
}

int CppSQLite3DB::ExecDML(const char *szSQL)
{
	CheckDB();

	char* szError=0;

	int nRet = sqlite3_exec(mpDB, szSQL, 0, 0, &szError);

	if (nRet == SQLITE_OK)
	{
		return sqlite3_changes(mpDB);  //返回这个执行影响的行数
	}
	else
	{
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}
}

CppSQLite3Query CppSQLite3DB::ExecQuery(const char *szSQL)
{
	CheckDB();

	//编译一条指针对象，用临时变量存储，传递给CppSQLite3Query后，
	//这个临时sqlite3_stmt*对象会自动消失，最后只有一份保留在CppSQLite3Query中
	sqlite3_stmt *pStmt = Compile(szSQL);  

	int nRet = sqlite3_step(pStmt);

	if (nRet == SQLITE_DONE) //表明这个查询没有返回结果
	{
		return CppSQLite3Query(mpDB, pStmt, true); //所以将第三个字段设置为true，表明到达查询结果末尾处
	}
	else if (nRet == SQLITE_ROW) //这个查询中至少有一行记录
	{
		return CppSQLite3Query(mpDB, pStmt, false); //false表明没有到达查询结果末尾
	}
	else
	{
		nRet = sqlite3_finalize(pStmt);
		const char *szError = sqlite3_errmsg(mpDB);
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}
}

CppSQLite3Table CppSQLite3DB::GetTable(const char *szSQL)
{
	CheckDB();

	char* szError=0;
	char** paszResults=0;
	int nRet;
	int nRows(0);
	int nCols(0);

	nRet = sqlite3_get_table(mpDB, szSQL, &paszResults, &nRows, &nCols, &szError);
	//比较难理解的是第三个参数，是一个三重指针。
	//pazResult指向的是一个一维指针数组，char*result[Num];
	//在SQLite中所有的字段都是被解释为字符串类型存储的，采用的是UTF8编码方式。
	//并且将一个二维表存储在了一维数组中，就是将一个一维数组分割成了mnRows个段，
	//每一段代表二维表中的一行，一行有mnCols个元素。
	//只要给出确切的行值与列值，就能根据公式算出在一位数组中的索引下标。
	
	if (nRet == SQLITE_OK)
	{
		return CppSQLite3Table(paszResults, nRows, nCols);
	}
	else
	{
		throw CppSQLite3Exception(nRet, szError,DONT_DELETE_MSG);
	}
}

CppSQLite3Statement CppSQLite3DB::CompileStatement(const char *szSQL)
{
	CheckDB();

	sqlite3_stmt *pStmt = Compile(szSQL);
	return CppSQLite3Statement(mpDB, pStmt);
}
CppSQLite3Table::CppSQLite3Table()
{
	mnCols = 0;
	mnRows = 0;
	mnCurrentRow = 0;
	mpaszResults = 0;
}

CppSQLite3Table::CppSQLite3Table(const CppSQLite3Table &rTable)
{
	mnCols = rTable.mnCols;
	mnRows = rTable.mnRows;
	mnCurrentRow = rTable.mnCurrentRow;
	mpaszResults = rTable.mpaszResults;
	const_cast<CppSQLite3Table&>(rTable).mpaszResults = 0; //当表复制时，销毁原来的表，防止丢失修改
}

CppSQLite3Table::CppSQLite3Table(char **paszResults, int nRows, int nCols)
{
	mpaszResults = paszResults; //给出一个一维指针数组，初始化一个表
	mnCols = nCols;
	mnRows = nRows;
	mnCurrentRow = 0;
}

CppSQLite3Table::~CppSQLite3Table()
{
	finalize();
}

CppSQLite3Table& CppSQLite3Table::operator= (const CppSQLite3Table &rTable)
{
	finalize();

	mpaszResults = rTable.mpaszResults;
	const_cast<CppSQLite3Table &>(rTable).mpaszResults = 0;
	mnCols = rTable.mnCols;
	mnRows = rTable.mnRows;
	mnCurrentRow = rTable.mnCurrentRow;

	return *this;
}

void CppSQLite3Table::finalize()
{
	if (mpaszResults)
	{
		sqlite3_free_table(mpaszResults);  //利用库函数销毁表存储内容
		mpaszResults = 0;
	}
}

int CppSQLite3Table::NumOfFields()
{
	CheckResluts();

	return mnCols;
}

int CppSQLite3Table::NumOfRows()
{
	CheckResluts();

	return mnRows;
}

const char* CppSQLite3Table::NameOfField(int nField)
{
	CheckResluts();

	if (nField < 0 || nField > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid field index requested",
			DONT_DELETE_MSG);
	}

	return mpaszResults[nField]; //一位数组的头mnCols个元素存放的是表的字段名称，存储具体位置是mpaszResults[0,,,mnCols-1]。
}

const char* CppSQLite3Table::ValueOfField(int nField)
{
	CheckResluts();

	if (nField < 0 || nField > mnCols-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid field index requested",
			DONT_DELETE_MSG);
	}

	//根据要查询的当前行与列值算出在一位数组中的索引下标，额外加一个mnCols是第一行存储的是字段名
	int nIndex = mnCurrentRow*mnCols + mnCols + nField;
	return mpaszResults[nIndex];
}

//根据字段名称来访问某一列的数据
const char* CppSQLite3Table::ValueOfField(const char *szField)
{
	CheckResluts();

	if (szField)
	{
		for (int nField = 0; nField < mnCols; nField++)
		{
			if (strcmp(szField, mpaszResults[nField]) == 0)
			{
				int nIndex = mnCurrentRow*mnCols + mnCols + nField;
				return mpaszResults[nIndex];
			}
		}
	}

	throw CppSQLite3Exception(CPPSQLITE_ERROR,
		"Invalid field name requested",
		DONT_DELETE_MSG);
}

bool CppSQLite3Table::FieldIsNull(int nField)
{
	CheckResluts();

	return (ValueOfField(nField) == 0);
}

bool CppSQLite3Table::FieldIsNull(const char* szField)
{
	CheckResluts();

	return (ValueOfField(szField) == 0);
}

//这里的获取具体类型数值函数，需要用户对数据库中的表有一定的了解，知道哪些字段存储的是什么内容
//并且使用的是外部传递引用的形式
bool CppSQLite3Table::GetIntField(int nField, int &rDest)
{
	if (FieldIsNull(nField))
	{
		return false;
	} 
	else
	{
		//atoi()函数是C库函数，将数值型字符串转换为整型值
		rDest = atoi(ValueOfField(nField));
		return true;
	}
}

bool CppSQLite3Table::GetIntField(const char *szField, int &rDest)
{
	if (FieldIsNull(szField))
	{
		return false;
	} 
	else
	{
		rDest = atoi(ValueOfField(szField));
		return true;
	}
}

bool CppSQLite3Table::GetFloatField(int nField, double &rDest)
{
	if (FieldIsNull(nField))
	{
		return false;
	} 
	else
	{
		//C库函数，将数值型字符串转换为浮点数
		rDest = atof(ValueOfField(nField));
		return true;
	}
}

bool CppSQLite3Table::GetFloatField(const char *szField, double &rDest)
{
	if (FieldIsNull(szField))
	{
		return false;
	} 
	else
	{
		rDest = atof(ValueOfField(szField));
		return true;
	}
}

bool CppSQLite3Table::GetStringField(int nField, char *&rDest)
{
	if (FieldIsNull(nField))
	{
		return false;
	} 
	else
	{
		rDest = const_cast<char *>(ValueOfField(nField));
		return true;
	}
}
bool CppSQLite3Table::GetStringField(const char *szField, char *&rDset)
{
	if (FieldIsNull(szField))
	{
		return false;
	}
	else
	{
		rDset = const_cast<char *>(ValueOfField(szField));
		return true;
	}
}

//在每一次需要获取数据的时候都要设置要访问的行值
void CppSQLite3Table::SetRow(int nRow)
{
	CheckResluts();

	if (nRow < 0 || nRow > mnRows-1)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Invalid row index requested",
			DONT_DELETE_MSG);
	}

	mnCurrentRow = nRow;
}

void CppSQLite3Table::CheckResluts()
{
	if (mpaszResults == 0)
	{
		throw CppSQLite3Exception(CPPSQLITE_ERROR,
			"Null Results pointer",
			DONT_DELETE_MSG);
	}
}
