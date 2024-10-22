//
// Created by robot1 on 10/11/24.
//
#include <gtest/gtest.h>
#include "ClassToTest.h"

TEST(package_name, a_first_test)
{
    auto class_to_test = new ClassToTest();
    ASSERT_EQ(4, 2 + 2);
}

TEST(package_name,a_second_test) {
    ASSERT_EQ(2, 2);
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}