#include <gtest/gtest.h>
#include <string>
#include <VelocityCommand.h>

#include "host_command_handler/ReceiveCommand.h"

// ── Test fixture ─────────────────────────────────────────────────────────────
class HostCommandInterfaceTest : public ::testing::Test {
   protected:
};


// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_F(HostCommandInterfaceTest, CanHandleMissingEndChar) {
   const auto cmd = ReceiveCommand("VEL 3.12 4.56");
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
   const auto cmd = ReceiveCommand("VEL 3.12 4.56;");
   // Check the command is valid
   EXPECT_TRUE(cmd.valid());
   // Compare the command name
   EXPECT_EQ(cmd.command(), HostCommandName::Vel);
}


TEST_F(HostCommandInterfaceTest, CanHandleCommandArgs) {
   const auto cmd = ReceiveCommand("VEL 3.12 4.56;");
   // Compare individual args
   const auto args = cmd.args();
   ASSERT_EQ(args.size(), 2u);          // stop test if wrong size
   EXPECT_EQ(args[0], "3.12");
   EXPECT_EQ(args[1], "4.56");
}


TEST_F(HostCommandInterfaceTest, CanBuildVelocityCommand) {
   const auto cmd = ReceiveCommand("VEL 3.12 4.56;");
   ASSERT_EQ(cmd.command(), HostCommandName::Vel);
   const auto velocityCommand  = VelocityCommand(cmd.args());
   EXPECT_TRUE(velocityCommand.valid());
   EXPECT_FLOAT_EQ(velocityCommand.left(), 3.12f);
   EXPECT_FLOAT_EQ(velocityCommand.right(), 4.56f);
}

TEST_F(HostCommandInterfaceTest, CanHandleVelocityCommandMissingArgs) {
   const auto cmd = ReceiveCommand("VEL 3.12;");
   ASSERT_EQ(cmd.command(), HostCommandName::Vel);
   const auto velocityCommand  = VelocityCommand(cmd.args());
   EXPECT_FALSE(velocityCommand.valid());
}