#pragma once
#include "redispp.h"
#include "gtest/gtest.h"

#pragma comment(lib, "RedisClient.lib")

using namespace redispp;

class TestConnection : public ::testing::Test
{
public:
	TestConnection();
	virtual ~TestConnection();

	virtual void SetUp();
	virtual void TearDown();

	Connection conn;
};

