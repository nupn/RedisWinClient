#include <stdio.h>
#include <string>
#include "redispp.h"
#include <Windows.h>
#include "cstringt.h"
#include "atlstr.h"

#pragma comment(lib, "RedisClient.lib")
#define _ConsoleDebugInfo

using namespace redispp;
using namespace std;


void GetConnect(ConnectionValue& value, const CString fileName)
{
	/*
	TCHAR Init_path[1024];
	GetCurrentDirectory(1024, Init_path);
	printf("path: %S\n", Init_path);
	*/

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

int main(int argc, char* argv[])
{

WSADATA wsaData;
WORD version;
version = MAKEWORD(2, 0);
WSAStartup(version, &wsaData);


ConnectionValue conninfo;
GetConnect(conninfo, _T("..\\connect.ini"));

Connection conn(conninfo);

conn.set("hello", "world");
StringReply stringReply = conn.get("hello");
stringReply.result().is_initialized();
printf("%s", ((std::string)conn.get("hello")).c_str());

//BOOST_CHECK((bool)conn.exists("hello"));
//BOOST_CHECK((bool)conn.del("hello"));
//BOOST_CHECK(!conn.exists("hello"));
//BOOST_CHECK(!conn.del("hello"));
return 0;
}
