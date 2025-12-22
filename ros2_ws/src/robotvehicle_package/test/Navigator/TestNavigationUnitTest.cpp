#include <gtest/gtest.h>

#include "Utilities/String.h"
#include "Navigation/NavigationPath.h"
#include "Navigation/NavigationExceptionNoPathPoints.h"
#include "Navigation/NavigationExceptionPathPointAlreadyCompleted.h"

int argcTest;
char **argvTest;

TEST(test_navigation_path,can_fail_if_no_path_points_set)
{
    NavigationPath path;
    EXPECT_THROW(path.getCurrentGoToPoint(), NavigationExceptionNoPathPoints);
}

TEST(test_navigation_path,can_add_points_to_path)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    path.addPathPoint(2,2,2);
}

TEST(test_navigation_path,can_get_first_point)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    const NavigationPoint *point = path.getCurrentGoToPoint();
    EXPECT_EQ(point->getX(),1);
    EXPECT_EQ(point->getY(),1);
    EXPECT_EQ(point->getTheta(),1);
}

TEST(test_navigation_path,can_get_second_point)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    path.addPathPoint(2,2,2);
    path.setNextGoToPoint();
    const NavigationPoint *point = path.getCurrentGoToPoint();
    EXPECT_EQ(point->getX(),2);
    EXPECT_EQ(point->getY(),2);
    EXPECT_EQ(point->getTheta(),2);
}

TEST(test_navigation_path,can_check_verify_path_finalized)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    path.addPathPoint(2,2,2);
    path.setNextGoToPoint();
    const NavigationPoint *point = path.getCurrentGoToPoint();
    EXPECT_EQ(point->getX(),2);
    EXPECT_EQ(point->getY(),2);
    EXPECT_EQ(point->getTheta(),2);
}

TEST(test_navigation_path,can_fail_on_next_not_possible)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    path.addPathPoint(2,2,2);
    path.setNextGoToPoint();
    EXPECT_THROW(path.setNextGoToPoint(), NavigationExceptionPathPointAlreadyCompleted);
}

TEST(test_navigation_path,can_verify_if_next_point_is_available)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    path.addPathPoint(2,2,2);
    EXPECT_TRUE(path.isNextPointAvailable());
}

TEST(test_navigation_path,can_verify_if_next_point_not_is_available)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    EXPECT_FALSE(path.isNextPointAvailable());
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}