#include <gtest/gtest.h>

#include "./MockSerialMonitorIO.h"
#include "../src/monitor.h"

namespace
{
  using ::testing::AtLeast;
  using ::testing::Return;
  using ::testing::Test;

  class SerialMonitorTest : public Test {
    protected:
      MockSerialMonitorIO mock;
      SerialMonitor serialMonitor = SerialMonitor(&mock);
  };

  TEST_F(SerialMonitorTest, callback)
  {
    EXPECT_CALL(mock, rsv()).Times(AtLeast(1)).WillOnce(Return("test")); // 水位 9
    EXPECT_EQ("test", serialMonitor.command()); // 期待値：false
  }

  int main(int argc, char **argv)
  {
    // 以下の行は，テスト開始前に Google Mock （と Google Test）
    // を初期化するために必ず実行する必要があります．
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
  }

} // namespace