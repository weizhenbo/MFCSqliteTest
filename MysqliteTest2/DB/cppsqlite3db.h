#pragma once
#include <cstdio>
#include <cstring>
#include "..//sqlite3//sqlite3.h"

#define CPPSQLITE_ERROR 1000  //这个宏定义是为了区分库函数中定义的错误代码，专用于自己程序中的异常标示

class CppSQLite3Exception
{
private:
	int mnErrCode;
	char *mpszErrMessage;

public:
	CppSQLite3Exception(const int nErrNode,
		const char *szErrMess,
		bool bDeleteMess);
	CppSQLite3Exception(const CppSQLite3Exception &e);
	~CppSQLite3Exception();

	const int ErrorCode() { return mnErrCode; }
	const char *ErrorMessage() { return mpszErrMessage; }

	static const char* ErrorCodeAsString(const int nErrCode);
};
class CppSQLite3Query
{
private:
	sqlite3 *mpDB;
	sqlite3_stmt *mpStmt;
	//是一个SQL语句对象，或是说一条原生的SQL查询语句select * from XXX;经过SQLite处理后就生成了一个sqlite3_stmt对象。
	//以后就可以不用原生的语句来做查询，就直接使用这个sqlite3_stmt*就可以查询了，
	//当查询结束后，要将sqlite3_stmt*指针对象释放掉。
	//释放函数sqlite3_finalize()和sqlite3_free();函数功能类似，
	//只不过后者是释放sqlite3*对象的，而前者是释放sqlite3_stmt*对象的。
	bool mbEof;		//由于查询的结果需要一行一行的访问，所以设置一个bool值来表示是否达到最后一行的结尾
	int mnCols;		//表示这个查询结果的列数，行数是无法得知的

	void CheckStmt();
public:
	CppSQLite3Query();
	CppSQLite3Query(sqlite3 *pdb, sqlite3_stmt *pStmt, bool bEof);
	CppSQLite3Query(const CppSQLite3Query &rQuery);
	CppSQLite3Query& operator= (const CppSQLite3Query &rQuery);
	~CppSQLite3Query();

	int FieldNums();

	int FieldIndex(const char* szField);
	const char* FieldName(int nField);

	int FieldDataType(int nField);
	const char* FieldDeclType(int nField);

	const char* FieldValue(int nField);
	const char* FieldValue(const char *szField);

	bool FieldIsNull(int nField);
	bool FieldIsNull(const char *szField);

	bool GetIntValue(int nField, int &rDest);
	bool GetIntValue(const char *szField, int &rDest);

	bool GetFloatValue(int nField, double &rDest);
	bool GetFloatValue(const char *szField, double &rDest);

	bool GetStringValue(int nField, char *&rDest);
	bool GetStringValue(const char *szField, char *&rDest);

	LPCTSTR GetStringValue(int nCol);

	bool Eof();

	void NextRow();

	void Finalize();
};
class CppSQLite3Table
{
private:
	int mnCurrentRow;	//当前行
	int mnRows;			//行数	
	int mnCols;			//列数
	char **mpaszResults;//指向一个一维指针数组，char*result[Num] 即二维数组转换为一维后的那个数组

	void CheckResluts();

public:
	CppSQLite3Table();  
	CppSQLite3Table(const CppSQLite3Table &rTable);
	CppSQLite3Table(char **paszResults, int nRows, int nCols);
	~CppSQLite3Table();

	CppSQLite3Table& operator= (const CppSQLite3Table &rTable);

	int NumOfFields();
	int NumOfRows();

	const char* NameOfField(int nField);

	const char* ValueOfField(int nField);
	const char* ValueOfField(const char *szField);//根据字段名称来访问某一列的数据

	bool FieldIsNull(int nField);
	bool FieldIsNull(const char *szField);

	bool GetIntField(int nField, int &rDest);
	bool GetIntField(const char *szField, int &rDest);

	bool GetFloatField(int nField, double &rDest);
	bool GetFloatField(const char *szField, double &rDest);

	bool GetStringField(int nField, char *&rDest);
	bool GetStringField(const char *szField, char *&rDset);

	void SetRow(int nRow);

	void finalize();
};
class CppSQLite3Statement
{
private:
	sqlite3 *mpDB;
	sqlite3_stmt *mpStmt;

	void CheckDB();
	void CheckStmt();
public:
	CppSQLite3Statement();
	CppSQLite3Statement(sqlite3 *pdb, sqlite3_stmt *pstmt);
	CppSQLite3Statement(const CppSQLite3Statement &rStatement);
	CppSQLite3Statement& operator = (const CppSQLite3Statement &rStatement);
	~CppSQLite3Statement();

	int ExecDML();
	CppSQLite3Query ExecQuery();

	void Reset();

	void Finalize(); 
};
class CppSQLite3DB
{
private:
	sqlite3 *mpDB;
	sqlite3_stmt *mpStmt;

	void CheckDB();
	CppSQLite3DB(const CppSQLite3DB &rDB);
	CppSQLite3DB& operator = (const CppSQLite3DB &rDB);

	sqlite3_stmt* Compile(const char *szSQL);

public:
	CppSQLite3DB();
	~CppSQLite3DB();

	void Open(const char *szFileName);
	void Close();

	int ExecDML(const char *szSQL);
	CppSQLite3Query ExecQuery(const char *szSQL);

	CppSQLite3Table GetTable(const char *szSQL);

	CppSQLite3Statement CompileStatement(const char *szSQL);

};

