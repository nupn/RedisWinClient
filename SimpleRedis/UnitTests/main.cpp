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

TEST_F(TestConnection, nullreplies)
{
	try {
		std::string str = conn.get("nonexistant");
	}
	catch(NullReplyException const & except) {
		//EXPECT_TRUE(false);
	}
	catch(...) {
		EXPECT_TRUE(false);
	}

	StringReply nullReply = conn.get("nonexistant");
	EXPECT_EQ(false, nullReply.result().is_initialized());

	// Connection still in good state:
	conn.set("one", "1");
	EXPECT_STREQ(((std::string)conn.get("one")).c_str(), "1");
}

TEST_F(TestConnection, Typet)
{
	conn.set("hello", "world");
	EXPECT_TRUE(conn.type("hello") == String);
}

TEST_F(TestConnection, Keys)
{
	conn.set("hello", "world");
	//MultiBulkEnumerator response = conn.keys("h?llo");
	MultiBulkEnumerator response = conn.keys("hello");
	std::string key;
	bool found = false;
	while(response.next(&key))
	{
		if(key == "hello")
			found = true;
	}
	EXPECT_TRUE(found);
}

TEST_F(TestConnection, RandomKey)
{
	conn.set("hello", "world");
	EXPECT_TRUE((bool)conn.exists(conn.randomKey()));
}


TEST_F(TestConnection, rename)
{
	conn.del("hello3");

	conn.set("hello", "world");
	conn.rename("hello", "hello1");
	EXPECT_TRUE((std::string)conn.get("hello1") == "world");
	conn.set("hello2", "one");
	EXPECT_TRUE(!conn.renameNX("hello1", "hello2"));
	EXPECT_TRUE((bool)conn.renameNX("hello1", "hello3"));

	conn.del("hello");
	conn.del("hello2");
	conn.del("hello3");
}

TEST_F(TestConnection, dbsize)
{
	conn.set("hello", "world");
	conn.del("hello1");
	int size = conn.dbSize();
	EXPECT_TRUE(size >= 1);
	conn.set("hello1", "one");
	EXPECT_TRUE(conn.dbSize() == size + 1);

	conn.del("hello");
	conn.del("hello1");
}


TEST_F(TestConnection, expire_ttl)
{
	conn.set("hello", "world");
	conn.set("hello1", "world");
	time_t now = time(NULL);
	EXPECT_TRUE((bool)conn.expire("hello", 3));
	EXPECT_TRUE((bool)conn.expireAt("hello1", now + 3));
	EXPECT_TRUE(conn.ttl("hello") <= 3);
	EXPECT_TRUE(conn.ttl("hello1") <= 3);
	//sleep(6);
	::Sleep(4*1000);
	EXPECT_TRUE(!conn.exists("hello"));
	EXPECT_TRUE(!conn.exists("hello1"));
}

TEST_F(TestConnection, select_move)
{
	conn.select(0);
	conn.del("hello");
	conn.select(1);
	conn.del("hello");

	conn.select(0);
	conn.set("hello", "world");
	EXPECT_TRUE((bool)conn.exists("hello"));
	conn.select(1);
	EXPECT_TRUE(!conn.exists("hello"));
	conn.select(0);
	conn.move("hello", 1);
	EXPECT_TRUE(!conn.exists("hello"));
	conn.select(1);
	EXPECT_TRUE((bool)conn.exists("hello"));
	conn.select(0);


	conn.select(0);
	conn.del("hello");
	conn.select(1);
	conn.del("hello");
}

TEST_F(TestConnection, flush)
{
	conn.select(0);
	conn.del("hello");
	conn.select(1);
	conn.del("hello");

	conn.set("hello", "world");
	EXPECT_TRUE(conn.dbSize() > 0);
	conn.flushDb();
	EXPECT_TRUE(conn.dbSize() == 0);
	conn.set("hello", "world");
	EXPECT_TRUE(conn.dbSize() > 0);
	conn.select(1);
	conn.set("hello", "world");
	EXPECT_TRUE(conn.dbSize() > 0);
	conn.flushAll();
	EXPECT_TRUE(conn.dbSize() == 0);
	conn.select(0);
	EXPECT_TRUE(conn.dbSize() == 0);

	conn.select(0);
	conn.del("hello");
	conn.select(1);
	conn.del("hello");
}

TEST_F(TestConnection, getset)
{
	conn.set("hello", "world");
	EXPECT_TRUE((std::string)conn.getSet("hello", "one") == "world");
	EXPECT_TRUE((std::string)conn.get("hello") == "one");
}

TEST_F(TestConnection, setnx)
{
	conn.set("hello", "world");
	EXPECT_TRUE(!conn.setNX("hello", "one"));
	conn.del("hello");
	EXPECT_TRUE((bool)conn.setNX("hello", "one"));
}

TEST_F(TestConnection, setex)
{
	conn.set("hello","one");

	conn.setEx("hello", 5, "world");
	EXPECT_TRUE(conn.ttl("hello") <= 5);

	::Sleep(3*1000);

	EXPECT_TRUE((bool)conn.exists("hello"));

	::Sleep(3*1000);

	EXPECT_TRUE((bool)!conn.exists("hello"));
}

TEST_F(TestConnection, incrdecr)
{
	conn.set("hello", "5");
	EXPECT_TRUE(conn.incr("hello") == 6);
	EXPECT_TRUE(conn.incrBy("hello", 2) == 8);
	EXPECT_TRUE(conn.decr("hello") == 7);
	EXPECT_TRUE(conn.decrBy("hello", 2) == 5);
}


TEST_F(TestConnection, append)
{
	conn.set("hello", "world");
	EXPECT_TRUE(conn.append("hello", "one") == 8);
	EXPECT_TRUE((std::string)conn.get("hello") == "worldone");
}

TEST_F(TestConnection, substr)
{
	conn.set("hello", "world");
	EXPECT_TRUE((std::string)conn.subStr("hello", 1, 3) == "orl");
}

TEST_F(TestConnection, lists)
{
	conn.del("hello");
	EXPECT_TRUE(conn.lpush("hello", "c") == 1);
	EXPECT_TRUE(conn.lpush("hello", "d") == 2);
	EXPECT_TRUE(conn.rpush("hello", "b") == 3);
	EXPECT_TRUE(conn.rpush("hello", "a") == 4);
	EXPECT_TRUE(conn.llen("hello") == 4);
	MultiBulkEnumerator result = conn.lrange("hello", 1, 3);
	std::string str;
	EXPECT_TRUE(result.next(&str));
	EXPECT_TRUE(str == "c");
	EXPECT_TRUE(result.next(&str));
	EXPECT_TRUE(str == "b");
	EXPECT_TRUE(result.next(&str));
	EXPECT_TRUE(str == "a");
	conn.ltrim("hello", 0, 1);
	result = conn.lrange("hello", 0, 10);
	EXPECT_TRUE(result.next(&str));
	EXPECT_TRUE(str == "d");
	EXPECT_TRUE(result.next(&str));
	EXPECT_TRUE(str == "c");
	EXPECT_TRUE((std::string)conn.lindex("hello", 0) == "d");
	EXPECT_TRUE((std::string)conn.lindex("hello", 1) == "c");
	conn.lset("hello", 1, "f");
	EXPECT_TRUE((std::string)conn.lindex("hello", 1) == "f");
	conn.lpush("hello", "f");
	conn.lpush("hello", "f");
	conn.lpush("hello", "f");
	EXPECT_TRUE(conn.lrem("hello", 2, "f") == 2);
	EXPECT_TRUE(conn.llen("hello") == 3);
	EXPECT_TRUE((std::string)conn.lpop("hello") == "f");
	EXPECT_TRUE(conn.llen("hello") == 2);
	conn.rpush("hello", "x");
	EXPECT_TRUE((std::string)conn.rpop("hello") == "x");
	conn.rpush("hello", "z");
	EXPECT_TRUE((std::string)conn.rpopLpush("hello", "hello") == "z");
}


TEST_F(TestConnection, sets)
{
	conn.del("hello");
	EXPECT_TRUE((bool)conn.sadd("hello", "world"));
	EXPECT_TRUE((bool)conn.sisMember("hello", "world"));
	EXPECT_TRUE(!conn.sisMember("hello", "mars"));
	EXPECT_TRUE(conn.scard("hello") == 1);
	EXPECT_TRUE((bool)conn.sadd("hello", "mars"));
	EXPECT_TRUE(conn.scard("hello") == 2);
	MultiBulkEnumerator result = conn.smembers("hello");
	std::string str1;
	std::string str2;
	EXPECT_TRUE(result.next(&str1));
	EXPECT_TRUE(result.next(&str2));
	EXPECT_TRUE((str1 == "world" && str2 == "mars") || (str2 == "world" && str1 == "mars"));
	std::string randomMember = conn.srandMember("hello");
	EXPECT_TRUE(randomMember == "world" || randomMember == "mars");
	EXPECT_TRUE((bool)conn.srem("hello", "mars"));
	EXPECT_TRUE(conn.scard("hello") == 1);
	EXPECT_TRUE((std::string)conn.spop("hello") == "world");
	EXPECT_TRUE(conn.scard("hello") == 0);
	conn.del("hello1");
	EXPECT_TRUE((bool)conn.sadd("hello", "world"));
	EXPECT_TRUE(conn.scard("hello") == 1);
	EXPECT_TRUE((bool)conn.smove("hello", "hello1", "world"));
	EXPECT_TRUE(conn.scard("hello") == 0);
	EXPECT_TRUE(conn.scard("hello1") == 1);
}

TEST_F(TestConnection, hashes)
{
	conn.del("hello");
	EXPECT_TRUE((bool)conn.hset("hello", "world", "one"));
	EXPECT_TRUE((bool)conn.hset("hello", "mars", "two"));
	EXPECT_TRUE((std::string)conn.hget("hello", "world") == "one");
	EXPECT_TRUE(!conn.hsetNX("hello", "mars", "two"));
	EXPECT_TRUE((bool)conn.hsetNX("hello", "venus", "1"));
	EXPECT_TRUE(conn.hincrBy("hello", "venus", 3) == 4);
	EXPECT_TRUE((bool)conn.hexists("hello", "venus"));
	EXPECT_TRUE((bool)conn.hdel("hello", "venus"));
	EXPECT_TRUE(!conn.hexists("hello", "venus"));
	EXPECT_TRUE(conn.hlen("hello") == 2);
	MultiBulkEnumerator result = conn.hkeys("hello");
	std::string str1;
	std::string str2;
	EXPECT_TRUE(result.next(&str1));
	EXPECT_TRUE(result.next(&str2));
	EXPECT_TRUE((str1 == "world" && str2 == "mars") || (str2 == "world" && str1 == "mars"));
	result = conn.hvals("hello");
	EXPECT_TRUE(result.next(&str1));
	EXPECT_TRUE(result.next(&str2));
	EXPECT_TRUE((str1 == "one" && str2 == "two") || (str2 == "one" && str1 == "two"));
	result = conn.hgetAll("hello");
	std::string str3;
	std::string str4;
	EXPECT_TRUE(result.next(&str1));
	EXPECT_TRUE(result.next(&str2));
	EXPECT_TRUE(result.next(&str3));
	EXPECT_TRUE(result.next(&str4));
	EXPECT_TRUE(
		(str1 == "world" && str2 == "one" && str3 == "mars" && str4 == "two")
		||
		(str1 == "mars" && str2 == "two" && str3 == "world" && str4 == "one")
		);
}

TEST_F(TestConnection, misc)
{
	time_t now = time(NULL);
	::Sleep(2);
	conn.save();
	EXPECT_TRUE(conn.lastSave() > now);
	conn.bgSave();
	conn.bgReWriteAOF();
	EXPECT_TRUE(((std::string)conn.info()).length() > 0);
}

//TODO: test for pipelined requests

TEST_F(TestConnection, pipelined)
{
	{
		VoidReply a = conn.set("one", "a");
		StringReply readA = conn.get("one");
		{
			BoolReply b = conn.hset("two", "two", "b");
			VoidReply c = conn.set("three", "c");
		}
		EXPECT_TRUE((std::string)readA == "a");
	}

	EXPECT_TRUE((std::string)conn.get("one") == "a");
	EXPECT_TRUE((std::string)conn.hget("two", "two") == "b");
	EXPECT_TRUE((std::string)conn.get("three") == "c");

	{
		conn.del("hello");
		IntReply c = conn.lpush("hello", "c");
		IntReply d = conn.lpush("hello", "d");
		IntReply b = conn.rpush("hello", "b");
		IntReply a = conn.rpush("hello", "a");
		{
			MultiBulkEnumerator result = conn.lrange("hello", 1, 3);
			IntReply c = conn.lpush("hello", "c");
			IntReply d = conn.lpush("hello", "d");
			IntReply b = conn.rpush("hello", "b");
			IntReply a = conn.rpush("hello", "a");
			EXPECT_TRUE((int)a == 8);
			EXPECT_TRUE((int)b == 7);
			EXPECT_TRUE((int)d == 6);
			EXPECT_TRUE((int)c == 5);
			std::string str;
			EXPECT_TRUE(result.next(&str));
			EXPECT_TRUE(str == "c");
			EXPECT_TRUE(result.next(&str));
			EXPECT_TRUE(str == "b");
			EXPECT_TRUE(result.next(&str));
			EXPECT_TRUE(str == "a");
		}
		IntReply len = conn.llen("hello");
		EXPECT_TRUE((int)a == 4);
		EXPECT_TRUE((int)b == 3);
		EXPECT_TRUE((int)d == 2);
		EXPECT_TRUE((int)c == 1);
	}
}

TEST_F(TestConnection, ZSet)
{
	conn.del("Myzset");

	EXPECT_TRUE((bool)conn.zadd("Myzset", 1, "one"));
	EXPECT_TRUE((bool)conn.zadd("Myzset", 2, "two"));
	EXPECT_TRUE((bool)conn.zadd("Myzset", 3, "three"));
	
	MultiBulkEnumerator result = conn.zrange("Myzset", 0, -1);
	std::string str1;
	std::string str2;
	std::string str3;
	EXPECT_TRUE(result.next(&str1));
	EXPECT_TRUE(str1 == "one");
	EXPECT_TRUE(result.next(&str2));
	EXPECT_TRUE(str2 == "two");
	EXPECT_TRUE(result.next(&str3));
	EXPECT_TRUE(str3 == "three");

	result = conn.zrange("Myzset", 0, -1, true);
	EXPECT_TRUE(result.next(&str1));
	EXPECT_TRUE(str1 == "one");
	EXPECT_TRUE(result.next(&str2));
	EXPECT_TRUE(str2 == "1");
	EXPECT_TRUE(result.next(&str3));
	EXPECT_TRUE(str3 == "two");

	EXPECT_TRUE((int)conn.zcard("Myzset") == 3);
	EXPECT_TRUE((int)conn.zcount("Myzset", "(1", "3") == 2);
	EXPECT_TRUE((int)conn.zcount("Myzset", "-inf", "+inf") == 3);
	
	result = conn.zrangebyscore("Myzset", "(1", "2");
	EXPECT_TRUE(result.next(&str1));
	EXPECT_TRUE(str1 == "two");

	EXPECT_TRUE((int)conn.zrank("Myzset", "three") == 2);
	EXPECT_TRUE((int)conn.zrank("Myzset", "two") == 1);

	EXPECT_TRUE((std::string)conn.zscore("Myzset", "three") == "3");
	EXPECT_TRUE((std::string)conn.zscore("Myzset", "two") == "2");
}


int _tmain(int argc, _TCHAR* argv[])
{
	GlobalInit();
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}