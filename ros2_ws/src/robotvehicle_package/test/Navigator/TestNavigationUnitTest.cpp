#include <gtest/gtest.h>
#include <chrono>
#include <cmath>
#include <numeric>
#include "Utilities/String.h"
#include "Navigation/NavigationPath.h"
#include "Navigation/NavigationExceptionNoPathPoints.h"
#include "Navigation/NavigationExceptionPathPointAlreadyCompleted.h"
#include "PathPlanning/AStar.h"
#include "PathPlanning/AStar2.h"
#include "Utilities/MatrixHelper.h"

// ── Helpers ──────────────────────────────────────────────────────────────────

// Builds an obstacle-free grid of given size
static Eigen::MatrixXd makeOpenGrid(int rows, int cols) {
    return Eigen::MatrixXd::Zero(rows, cols);
}

// Builds a maze-like grid: every even row has a wall with one gap
static Eigen::MatrixXd makeMazeGrid(int rows, int cols) {
    Eigen::MatrixXd grid = makeOpenGrid(rows, cols);
    for (int r = 2; r < rows - 1; r += 4) {
        grid.row(r).setOnes();       // full wall
        grid(r, cols / 2) = 0;      // one gap in the middle
    }
    return grid;
}

// Manhattan distance — minimum possible steps on a 4-connected grid
static int manhattanDist(std::pair<int,int> a, std::pair<int,int> b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

// Octile distance — minimum possible steps on an 8-connected grid
// diagonals cost √2, straights cost 1
static double octileDist(std::pair<int,int> a, std::pair<int,int> b) {
    double dx = std::abs(a.first  - b.first);
    double dy = std::abs(a.second - b.second);
    return (dx + dy) + (M_SQRT2 - 2.0) * std::min(dx, dy);
}

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
    const NavigationPoint & point = path.getCurrentGoToPoint();
    EXPECT_EQ(point.getX(),1);
    EXPECT_EQ(point.getY(),1);
    EXPECT_EQ(point.getTheta(),1);
}

TEST(test_navigation_path,can_get_second_point)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    path.addPathPoint(2,2,2);
    path.setNextGoToPoint();
    const NavigationPoint &point = path.getCurrentGoToPoint();
    EXPECT_EQ(point.getX(),2);
    EXPECT_EQ(point.getY(),2);
    EXPECT_EQ(point.getTheta(),2);
}

TEST(test_navigation_path,can_check_verify_path_finalized)
{
    NavigationPath path;
    path.addPathPoint(1,1,1);
    path.addPathPoint(2,2,2);
    path.setNextGoToPoint();
    const NavigationPoint & point = path.getCurrentGoToPoint();
    EXPECT_EQ(point.getX(),2);
    EXPECT_EQ(point.getY(),2);
    EXPECT_EQ(point.getTheta(),2);
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


TEST(test_astar_path,can_path_is_correct_2)
{
    Eigen::MatrixXd grid(4, 4);
    grid << 0, 0, 0, 0,
            0, 1, 1, 0,
            0, 0, 0, 0,
            0, 1, 1, 0;

    std::pair start = {0,0};
    std::pair goal = {3,3};
    std::vector<std::pair<int,int>> expected_path{
        {0, 0}, {1, 0}, {2, 1}, {2, 2}, {3, 3} };
    const std::vector<std::pair<int,int>> actual_path =  AStar2::update(grid, start, goal);
    EXPECT_EQ(actual_path, expected_path);
}

// ── Efficiency: path optimality ───────────────────────────────────────────────

// On an open grid the path cost must equal octile distance (optimal for 8-connected)
TEST(test_astar_efficiency, path_is_optimal_on_open_grid)
{
    auto grid = makeOpenGrid(50, 50);
    std::pair<int,int> start = {0, 0};
    std::pair<int,int> goal  = {49, 49};

    AStar2 astar;
    auto path = astar.update(grid, start, goal);

    ASSERT_FALSE(path.empty()) << "No path found";
    // optimal steps on 8-connected = max(dx,dy) diagonal steps
    // for a 49,49 displacement that is exactly 49 diagonal steps → path size = 50
    EXPECT_EQ((int)path.size(), std::max(49, 49) + 1)
        << "Path is not optimal for 8-connected grid";
}

// Path length must always be optimal on open grid regardless of size
TEST(test_astar_efficiency, path_is_optimal_100x100_open_grid)
{
    auto grid = makeOpenGrid(100, 100);
    std::pair<int,int> start = {0,  0};
    std::pair<int,int> goal  = {99, 99};

    AStar2 astar;
    auto path = astar.update(grid, start, goal);

    ASSERT_FALSE(path.empty());
    // 99 diagonal steps + start = 100 nodes
    EXPECT_EQ((int)path.size(), std::max(99, 99) + 1);
}

// ── Efficiency: timing ────────────────────────────────────────────────────────

// Small grid must complete well under 1 ms
TEST(test_astar_efficiency, small_grid_completes_under_1ms)
{
    auto grid = makeOpenGrid(50, 50);
    std::pair<int,int> start = {0, 0}, goal = {49, 49};
    AStar2 astar;

    auto t0 = std::chrono::high_resolution_clock::now();
    astar.update(grid, start, goal);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    std::cout << "[50x50 open] elapsed: " << ms << " µs\n";
    EXPECT_LT(ms, 1000) << "50x50 open grid took more than 1 ms";
}

// Medium grid must complete under 50 ms
TEST(test_astar_efficiency, medium_grid_completes_under_50ms)
{
    auto grid = makeOpenGrid(500, 500);
    std::pair<int,int> start = {0, 0}, goal = {499, 499};
    AStar2 astar;

    auto t0 = std::chrono::high_resolution_clock::now();
    astar.update(grid, start, goal);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[500x500 open] elapsed: " << ms << " ms\n";
    EXPECT_LT(ms, 50) << "500x500 open grid took more than 50 ms";
}

// Large grid must complete under 500 ms
TEST(test_astar_efficiency, large_grid_completes_under_500ms)
{
    auto grid = makeOpenGrid(1000, 1000);
    std::pair<int,int> start = {0, 0}, goal = {999, 999};
    AStar2 astar;

    auto t0 = std::chrono::high_resolution_clock::now();
    astar.update(grid, start, goal);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[1000x1000 open] elapsed: " << ms << " ms\n";
    EXPECT_LT(ms, 500) << "1000x1000 open grid took more than 500 ms";
}

// ── Efficiency: repeated calls (robot replanning scenario) ────────────────────

// Calling update() 100 times must average under 5 ms each on a medium grid
TEST(test_astar_efficiency, repeated_calls_average_under_5ms)
{
    auto grid = makeOpenGrid(200, 200);
    std::pair<int,int> start = {0, 0}, goal = {199, 199};
    AStar2 astar;

    const int runs = 100;
    std::vector<long> times(runs);
    for (int i = 0; i < runs; ++i) {
        auto t0 = std::chrono::high_resolution_clock::now();
        astar.update(grid, start, goal);
        auto t1 = std::chrono::high_resolution_clock::now();
        times[i] = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    }

    long avg = std::accumulate(times.begin(), times.end(), 0L) / runs;
    long worst = *std::max_element(times.begin(), times.end());
    std::cout << "[200x200 x100 calls] avg: " << avg << " µs  worst: " << worst << " µs\n";
    EXPECT_LT(avg, 5000) << "Average over 100 calls exceeded 5 ms";
}

// ── Efficiency: maze (harder search) ─────────────────────────────────────────

// A* must still find a path through a maze within a reasonable time
TEST(test_astar_efficiency, maze_grid_completes_under_200ms)
{
    auto grid = makeMazeGrid(200, 200);
    std::pair<int,int> start = {0, 0}, goal = {199, 199};
    AStar2 astar;

    auto t0 = std::chrono::high_resolution_clock::now();
    auto path = astar.update(grid, start, goal);
    auto t1 = std::chrono::high_resolution_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "[200x200 maze] elapsed: " << ms << " ms  path length: " << path.size() << "\n";
    ASSERT_FALSE(path.empty()) << "No path found in maze";
    EXPECT_LT(ms, 200) << "Maze 200x200 took more than 200 ms";
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}