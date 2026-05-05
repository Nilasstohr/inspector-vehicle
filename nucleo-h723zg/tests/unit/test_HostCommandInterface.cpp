#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include <VelocityCommand.h>

#include "DistanceReadRequestCommand.h"
#include "host_command_handler/ReceiveCommand.h"

// ── Test fixture ─────────────────────────────────────────────────────────────
class HostCommandInterfaceTest : public ::testing::Test {
   protected:
};


// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_F(HostCommandInterfaceTest, CanHandleMissingEndChar) {
   const auto cmd = ReceiveCommand("v 3.12 4.56");
   // Check the command is valid
   EXPECT_TRUE(!cmd.valid());
}

TEST_F(HostCommandInterfaceTest, CanHandleUnknownCommand) {
   const auto cmd = ReceiveCommand("UNKNOWN 3.12 4.56;");
   // Check the command is valid
   EXPECT_TRUE(!cmd.valid());
   EXPECT_TRUE(cmd.command() == HostCommandName::Unknown);
}

TEST_F(HostCommandInterfaceTest, CanHandleEmptyCommand) {
   const auto cmd = ReceiveCommand("");
   EXPECT_TRUE(!cmd.valid());
}


TEST_F(HostCommandInterfaceTest, CanHandleknownCommand) {
   const auto cmd = ReceiveCommand("v 3.12 4.56;");
   // Check the command is valid
   EXPECT_TRUE(cmd.valid());
   // Compare the command name
   EXPECT_EQ(cmd.command(), HostCommandName::Vel);
}


TEST_F(HostCommandInterfaceTest, CanHandleCommandArgs) {
   const auto cmd = ReceiveCommand("v 3.12 4.56;");
   // Compare individual args
   const auto args = cmd.args();
   ASSERT_EQ(args.size(), 2u);          // stop test if wrong size
   EXPECT_EQ(args[0], "3.12");
   EXPECT_EQ(args[1], "4.56");
}


TEST_F(HostCommandInterfaceTest, CanBuildVelocityCommand) {
   const auto cmd = ReceiveCommand("v 3.12 4.56;");
   ASSERT_EQ(cmd.command(), HostCommandName::Vel);
   const auto velocityCommand  = VelocityCommand(cmd.args());
   EXPECT_TRUE(velocityCommand.valid());
   EXPECT_FLOAT_EQ(velocityCommand.left(), 3.12f);
   EXPECT_FLOAT_EQ(velocityCommand.right(), 4.56f);
}

TEST_F(HostCommandInterfaceTest, CanHandleVelocityCommandMissingArgs) {
   const auto cmd = ReceiveCommand("v 3.12;");
   ASSERT_EQ(cmd.command(), HostCommandName::Vel);
   const auto velocityCommand  = VelocityCommand(cmd.args());
   EXPECT_FALSE(velocityCommand.valid());
}


TEST_F(HostCommandInterfaceTest, CanAcceptWheelDistanceCommand) {
   const auto cmd = ReceiveCommand("p;");
   ASSERT_EQ(cmd.command(), HostCommandName::Dis);
   EXPECT_TRUE(cmd.valid());
}

TEST_F(HostCommandInterfaceTest, CanHandleWheelDistanceCommand) {
   constexpr float left = 4.65f;
   constexpr float right = -206.6f;
   auto response = DistanceReadRequestCommand::handle(left, right);
   EXPECT_STREQ(response.c_str(), "4.650 -206.600");
}

TEST_F(HostCommandInterfaceTest, PositionRequest_BothZero) {
   const auto response = DistanceReadRequestCommand::handle(0.0f, 0.0f);
   EXPECT_STREQ(response.c_str(), "0.000 0.000");
}

TEST_F(HostCommandInterfaceTest, PositionRequest_BothPositiveIntegers) {
   const auto response = DistanceReadRequestCommand::handle(100.0f, 200.0f);
   EXPECT_STREQ(response.c_str(), "100.000 200.000");
}

TEST_F(HostCommandInterfaceTest, PositionRequest_BothNegative) {
   // 0.5 and 2.5 are exactly representable in IEEE-754 single precision
   const auto response = DistanceReadRequestCommand::handle(-1.5f, -2.5f);
   EXPECT_STREQ(response.c_str(), "-1.500 -2.500");
}

TEST_F(HostCommandInterfaceTest, PositionRequest_NegativeLeftPositiveRight) {
   const auto response = DistanceReadRequestCommand::handle(-99.5f, 99.5f);
   EXPECT_STREQ(response.c_str(), "-99.500 99.500");
}

TEST_F(HostCommandInterfaceTest, PositionRequest_ZeroLeftNegativeRight) {
   const auto response = DistanceReadRequestCommand::handle(0.0f, -5.0f);
   EXPECT_STREQ(response.c_str(), "0.000 -5.000");
}

TEST_F(HostCommandInterfaceTest, PositionRequest_ExactThreeDecimalPlaces) {
   // 0.125 = 1/8, exactly representable; precision(3) should render it exactly
   const auto response = DistanceReadRequestCommand::handle(0.125f, 0.25f);
   EXPECT_STREQ(response.c_str(), "0.125 0.250");
}

TEST_F(HostCommandInterfaceTest, PositionRequest_AlwaysContainsSingleSpace) {
   const auto response = DistanceReadRequestCommand::handle(1.0f, 2.0f);
   const std::string s = response.c_str();
   const auto spaceCount = std::count(s.begin(), s.end(), ' ');
   EXPECT_EQ(spaceCount, 1);
}

TEST_F(HostCommandInterfaceTest, PositionRequest_NegativeOneAndOne) {
   const auto response = DistanceReadRequestCommand::handle(-1.0f, 1.0f);
   EXPECT_STREQ(response.c_str(), "-1.000 1.000");
}

TEST_F(HostCommandInterfaceTest, CanAcceptRestCommand) {
   const auto cmd = ReceiveCommand("r;");
   ASSERT_EQ(cmd.command(), HostCommandName::Reset);
   EXPECT_TRUE(cmd.valid());
}
