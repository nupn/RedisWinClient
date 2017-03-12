
#include "gtest/gtest.h"

#include <Windows.h>

#include <tchar.h>

#pragma comment(lib, "gtest_maind.lib")
#pragma comment(lib, "gtestd.lib")

TEST_F(AddTest, Add11)
{
	EXPECT_EQ(3, Add(1, 1));
}

int _tmain(int argc, _TCHAR* argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}