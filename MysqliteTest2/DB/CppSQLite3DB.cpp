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

int CppSQLite3Query::FieldNums()//����
{
	CheckStmt();

	return mnCols;
}

//�����ֶ�������������
int CppSQLite3Query::FieldIndex(const char* szField)
{
	CheckStmt();

	if (szField)
	{
		for (int nField = 0; nField < mnCols; nField++)
		{
			//���滹�кܶ����Ƶĺ�����������࣬��Ҫһ��sqlite3_stmt*��������ֵ����Ӧ�����ڲ���ѯ��֮�󷵻صĽ�������������ȱ����
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
const char* CppSQLite3Query::FieldName(int nField)//����
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

int CppSQLite3Query::FieldDataType(int nField)//����������
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

const char* CppSQLite3Query::FieldValue(int nField)//��������������ĳһ�е�����
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

const char* CppSQLite3Query::FieldValue(const char *szField)//�����ֶ�����������ĳһ�е�����
{
	int nField = FieldIndex(szField);
	return FieldValue(nField);
}

bool CppSQLite3Query::FieldIsNull(int nField)
{
	return (FieldDataType(nField) == SQLITE_NULL);
}

bool CppSQLite3Query::FieldIsNull(const char *szField)//�����ֶ������ж�ĳһ���Ƿ�Ϊ��
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

bool CppSQLite3Query::GetIntValue(const char *szField, int &rDest)//�����ֶ����ƻ�ȡĳһ���Ƿ������ֵ
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
bool CppSQLite3Query::GetFloatValue(const char *szField, double &rDest)//�����ֶ����ƻ�ȡĳһ���Ƿ�ĸ���ֵ
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
//bool CppSQLite3Query::GetStringValue(const char *szField, char *&rDest)//�����ֶ����ƻ�ȡĳһ���Ƿ���ַ���ֵ
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


bool CppSQLite3Query::Eof()//�ж��Ƿ�ﵽ���һ�еĽ�β
{
	CheckStmt();

	return mbEof;
}

void CppSQLite3Query::NextRow()//�ж���һ���Ƿ�Ϊ��
{
	CheckStmt();

	int nRet = sqlite3_step(mpStmt);

	if (nRet == SQLITE_DONE)//Ϊ��
	{
		// no rows
		mbEof = true;
	}
	else if (nRet == SQLITE_ROW)//��Ϊ��
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

void CppSQLite3Query::Finalize()//�ͷ������ڴ�
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
		int nRowChanged = sqlite3_changes(mpDB);  //������Ӱ�������

		nRet = sqlite3_reset(mpStmt);  //����sqlite3_stmt*����
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
		return CppSQLite3Query(mpDB, mpStmt, true);//����һ����ѯ��û�в�ѯ�����������Ϊtrue
	}
	else if (nRet == SQLITE_ROW)
	{
		return CppSQLite3Query(mpDB, mpStmt, false);//��ѯ���н���ģ�����false�����ⲻ����ɾ���β�ѯ����
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
			mpDB = 0; //һ���ر����ݿ�ָ�룬Ҫ��Ϊ0����ֹ��ιرճ���
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
		return sqlite3_changes(mpDB);  //�������ִ��Ӱ�������
	}
	else
	{
		throw CppSQLite3Exception(nRet, szError, DONT_DELETE_MSG);
	}
}

CppSQLite3Query CppSQLite3DB::ExecQuery(const char *szSQL)
{
	CheckDB();

	//����һ��ָ���������ʱ�����洢�����ݸ�CppSQLite3Query��
	//�����ʱsqlite3_stmt*������Զ���ʧ�����ֻ��һ�ݱ�����CppSQLite3Query��
	sqlite3_stmt *pStmt = Compile(szSQL);  

	int nRet = sqlite3_step(pStmt);

	if (nRet == SQLITE_DONE) //���������ѯû�з��ؽ��
	{
		return CppSQLite3Query(mpDB, pStmt, true); //���Խ��������ֶ�����Ϊtrue�����������ѯ���ĩβ��
	}
	else if (nRet == SQLITE_ROW) //�����ѯ��������һ�м�¼
	{
		return CppSQLite3Query(mpDB, pStmt, false); //false����û�е����ѯ���ĩβ
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
	//�Ƚ��������ǵ�������������һ������ָ�롣
	//pazResultָ�����һ��һάָ�����飬char*result[Num];
	//��SQLite�����е��ֶζ��Ǳ�����Ϊ�ַ������ʹ洢�ģ����õ���UTF8���뷽ʽ��
	//���ҽ�һ����ά��洢����һά�����У����ǽ�һ��һά����ָ����mnRows���Σ�
	//ÿһ�δ����ά���е�һ�У�һ����mnCols��Ԫ�ء�
	//ֻҪ����ȷ�е���ֵ����ֵ�����ܸ��ݹ�ʽ�����һλ�����е������±ꡣ
	
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
	const_cast<CppSQLite3Table&>(rTable).mpaszResults = 0; //������ʱ������ԭ���ı���ֹ��ʧ�޸�
}

CppSQLite3Table::CppSQLite3Table(char **paszResults, int nRows, int nCols)
{
	mpaszResults = paszResults; //����һ��һάָ�����飬��ʼ��һ����
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
		sqlite3_free_table(mpaszResults);  //���ÿ⺯�����ٱ�洢����
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

	return mpaszResults[nField]; //һλ�����ͷmnCols��Ԫ�ش�ŵ��Ǳ���ֶ����ƣ��洢����λ����mpaszResults[0,,,mnCols-1]��
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

	//����Ҫ��ѯ�ĵ�ǰ������ֵ�����һλ�����е������±꣬�����һ��mnCols�ǵ�һ�д洢�����ֶ���
	int nIndex = mnCurrentRow*mnCols + mnCols + nField;
	return mpaszResults[nIndex];
}

//�����ֶ�����������ĳһ�е�����
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

//����Ļ�ȡ����������ֵ��������Ҫ�û������ݿ��еı���һ�����˽⣬֪����Щ�ֶδ洢����ʲô����
//����ʹ�õ����ⲿ�������õ���ʽ
bool CppSQLite3Table::GetIntField(int nField, int &rDest)
{
	if (FieldIsNull(nField))
	{
		return false;
	} 
	else
	{
		//atoi()������C�⺯��������ֵ���ַ���ת��Ϊ����ֵ
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
		//C�⺯��������ֵ���ַ���ת��Ϊ������
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

//��ÿһ����Ҫ��ȡ���ݵ�ʱ��Ҫ����Ҫ���ʵ���ֵ
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
