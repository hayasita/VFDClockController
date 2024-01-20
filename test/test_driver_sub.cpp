#include <gtest/gtest.h>
#include "../src/driver_sub.h"

TEST(SensorENVIIIData, SensorENVIIIData){
  SensorENVIIIData testEnviii;
  EXPECT_FALSE(testEnviii.sensorActive);
  EXPECT_EQ(testEnviii.env3Temperature, 0);
  EXPECT_EQ(testEnviii.env3Humidity, 0);
  EXPECT_EQ(testEnviii.env3Pressure, 0);
}

TEST(SensorBME680Data, SensorBME680Data){
  SensorBME680Data testBme680;
  EXPECT_FALSE(testBme680.sensorActive);
  EXPECT_EQ(testBme680.temperature, 0);
  EXPECT_EQ(testBme680.humidity, 0);
  EXPECT_EQ(testBme680.pressure, 0);
  EXPECT_EQ(testBme680.gasResistance, 0);
  EXPECT_EQ(testBme680.altitude, 0);
}
