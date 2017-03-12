#include "TestConnection.h"
#include "redispp.h"
#include "gtest/gtest.h"
using namespace redispp;

#include <Windows.h>

#include <tchar.h>

#pragma comment(lib, "gtest_main-mdd.lib")
//#pragma comment(lib, "gtest_maind.lib")
#pragma comment(lib, "gtestd.lib")

void GlobalInit()
{
	WSADATA wsaData;
	WORD version;
	version = MAKEWORD(2, 0);
	WSAStartup(version, &wsaData);
}

TEST_F(TestConnection, hellowWorld)
{
	conn.set("hello", "world");
	StringReply stringReply = conn.get("hello");
	EXPECT_TRUE(stringReply.result().is_initialized());
	EXPECT_STREQ(((std::string)conn.get("hello")).c_str(), "world");
	EXPECT_TRUE((bool)conn.exists("hello"));
	EXPECT_TRUE((bool)conn.del("hello"));
	EXPECT_TRUE(!conn.exists("hello"));
	EXPECT_TRUE(!conn.del("hello"));
}

int _tmain(int argc, _TCHAR* argv[])
{
	GlobalInit();
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}