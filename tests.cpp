//
// Created by nbdy on 14.11.21.
//

#include "timerpp.h"
#include <cmath>
#include <gtest/gtest.h>

TEST(timerpp, Count) {
  uint64_t o = TIMESTAMP;
  uint64_t i = 0;

  timerpp::Timer t0([&i]{
    i += 1;
  });

  std::cout << "Created timer" << std::endl;

  EXPECT_EQ(t0.start(1000), timerpp::Timer::START_OK);

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  EXPECT_EQ(t0.stop(), timerpp::Timer::STOP_OK);

  EXPECT_TRUE(t0.isRunning());
  EXPECT_FALSE(t0.isArmed());

  t0.kill();
  EXPECT_FALSE(t0.isRunning());
  EXPECT_FALSE(t0.isArmed());

  EXPECT_EQ(i, 5);
}

TEST(timerpp, Precision) {
  std::vector<uint64_t> timestamps;

  timerpp::Timer t0([&timestamps]{
    timestamps.push_back(TIMESTAMP_MS);
  });

  t0.start(500);

  std::this_thread::sleep_for(Milliseconds(4000));

  t0.stop();

  EXPECT_EQ(timestamps.size(), 8);

  for(uint32_t i = 1; i < timestamps.size(); i++) {
    EXPECT_EQ(timestamps[i - 1], timestamps[i] - 500);
  }
}

void simulatedLoad(int n) {
  static float x = 4.2069666;
  for(uint32_t i = 0; i < n; i++) {
    x = std::atan(x);
  }
}

TEST(timerpp, PrecisionWithLoad) {
  uint32_t CYCLE_TIME = 500;
  uint32_t CYCLES = 20;

  std::vector<uint64_t> timestamps;

  timerpp::Timer t0([&timestamps]{
    TIMEIT_START("something")
    timestamps.push_back(TIMESTAMP_MS);
    simulatedLoad(30000000);
    TIMEIT_END
    TIMEIT_RESULT
  });

  t0.start(CYCLE_TIME);

  std::this_thread::sleep_for(Milliseconds(CYCLE_TIME * CYCLES));

  t0.stop();

  EXPECT_EQ(timestamps.size(), CYCLES);

  for(uint32_t i = 1; i < timestamps.size(); i++) {
    std::cout << "Jitter: " << timestamps[i] - timestamps[i - 1] << std::endl;
    // EXPECT_EQ(timestamps[i - 1], timestamps[i] - 500);
  }
}

TEST(timerpp, Restart) {
  uint32_t CYCLE_TIME = 500;
  uint32_t CYCLES = 4;

  timerpp::Timer t0([]{
    TIMEIT_START("something")
    simulatedLoad(300000);
    TIMEIT_END
    TIMEIT_RESULT
  });

  t0.start(CYCLE_TIME);

  std::this_thread::sleep_for(Milliseconds(CYCLE_TIME * CYCLES));

  t0.stop();

  EXPECT_FALSE(t0.isArmed());

  CYCLE_TIME = 300;
  CYCLES = 5;

  EXPECT_EQ(t0.start(CYCLE_TIME), timerpp::Timer::START_OK);

  std::this_thread::sleep_for(Milliseconds(CYCLE_TIME * CYCLES));

  EXPECT_EQ(t0.stop(), timerpp::Timer::STOP_OK);

  EXPECT_FALSE(t0.isArmed());
}