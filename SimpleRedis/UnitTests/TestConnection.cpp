#include "TestConnection.h"

/*
void GetConnect(ConnectionValue& value, const CString fileName)
{


	TCHAR cbuf[MAX_PATH];
	CString strSection, strKey;
	strSection = _T("Server");

	strKey = _T("HOST");
	GetPrivateProfileString(strSection, strKey, _T(""), cbuf, MAX_PATH, fileName);
	value.host = cbuf;

	cbuf[0] = '\0';
	strKey = _T("PORT");
	GetPrivateProfileString(strSection, strKey, _T(""), cbuf, MAX_PATH, fileName);
	value.port = cbuf;

	cbuf[0] = '\0';
	strKey = _T("PWD");
	GetPrivateProfileString(strSection, strKey, _T(""), cbuf, MAX_PATH, fileName);
	value.auth = cbuf;

#ifdef _ConsoleDebugInfo
	printf("ConnectionInfo------------\n");
	printf("host : %s\n", value.host.c_str());
	printf("port : %s\n", value.port.c_str());
	printf("auth : %s\n", value.auth.c_str());
	printf("--------------------------\n");
#endif

};
*/
TestConnection::TestConnection()
{
}


TestConnection::~TestConnection()
{
}

void TestConnection::SetUp()
{

}

void TestConnection::TearDown()
{

}