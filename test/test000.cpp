#include <gtest/gtest.h>
#include "../src/func.h"

TEST(func1, BasicTest) {
    EXPECT_EQ( func1(3, 4), 12); //GOOD
    EXPECT_EQ( func1(1, 1), 1);  //GOOD
    EXPECT_EQ( func1(1, 2), 1);  //NG
}