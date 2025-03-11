#include <gtest/gtest.h>
#include "Utilities/String.h"

int argcTest;
char **argvTest;

TEST(string_test,test_1)
{
    std::vector<std::string> expected;
    expected.emplace_back("this");
    expected.emplace_back("is");
    expected.emplace_back("a");
    expected.emplace_back("test");
    std::string testString;
    for (auto& str : expected) {
        testString += str + " ";
    }
    std::vector<std::string> actual;
    String::split(actual,testString,' ');
    if(actual!=expected) {
        EXPECT_TRUE(false) << "Split string failed";
    }
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}