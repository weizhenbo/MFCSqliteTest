#pragma once
#include <cstdio>
#include <cstring>
#include "..//sqlite3//sqlite3.h"

#define CPPSQLITE_ERROR 1000  //����궨����Ϊ�����ֿ⺯���ж���Ĵ�����룬ר�����Լ������е��쳣��ʾ

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
	//��һ��SQL�����󣬻���˵һ��ԭ����SQL��ѯ���select * from XXX;����SQLite������������һ��sqlite3_stmt����
	//�Ժ�Ϳ��Բ���ԭ�������������ѯ����ֱ��ʹ�����sqlite3_stmt*�Ϳ��Բ�ѯ�ˣ�
	//����ѯ������Ҫ��sqlite3_stmt*ָ������ͷŵ���
	//�ͷź���sqlite3_finalize()��sqlite3_free();�����������ƣ�
	//ֻ�����������ͷ�sqlite3*����ģ���ǰ�����ͷ�sqlite3_stmt*����ġ�
	bool mbEof;		//���ڲ�ѯ�Ľ����Ҫһ��һ�еķ��ʣ���������һ��boolֵ����ʾ�Ƿ�ﵽ���һ�еĽ�β
	int mnCols;		//��ʾ�����ѯ������������������޷���֪��

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
	int mnCurrentRow;	//��ǰ��
	int mnRows;			//����	
	int mnCols;			//����
	char **mpaszResults;//ָ��һ��һάָ�����飬char*result[Num] ����ά����ת��Ϊһά����Ǹ�����

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
	const char* ValueOfField(const char *szField);//�����ֶ�����������ĳһ�е�����

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

