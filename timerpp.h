//
// Created by nbdy on 11.11.21.
//

#ifndef TIMERPP__TIMERPP_H_
#define TIMERPP__TIMERPP_H_

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional>
#include <condition_variable>
#include <utility>

using UniqueLock = std::unique_lock<std::mutex>;
using Clock = std::chrono::high_resolution_clock;
using Milliseconds = std::chrono::milliseconds;
using Nanoseconds = std::chrono::nanoseconds;
#define TIMESTAMP_MS std::chrono::duration_cast<Milliseconds>(Clock::now().time_since_epoch()).count()
#define TIMESTAMP_NS std::chrono::duration_cast<Nanoseconds>(Clock::now().time_since_epoch()).count()

#define TIMESTAMP std::chrono::high_resolution_clock::to_time_t(std::chrono::high_resolution_clock::now())

std::string TIMEIT_NAME = "TIMEIT";
uint64_t TIMEIT_START_TIMESTAMP = 0;
uint64_t TIMEIT_END_TIMESTAMP = 0;
uint64_t TIMEIT_DIFF = 0;

#define TIMEIT_START(name) \
TIMEIT_NAME = name;        \
TIMEIT_START_TIMESTAMP = TIMESTAMP_MS;

#define TIMEIT_END \
TIMEIT_END_TIMESTAMP = TIMESTAMP_MS;

#define TIMEIT_RESULT \
TIMEIT_DIFF = TIMEIT_END_TIMESTAMP - TIMEIT_START_TIMESTAMP; \
std::cout << TIMEIT_NAME << ": " << std::to_string(TIMEIT_DIFF) << std::endl;


namespace timerpp {
using FunctionType = std::function<void()>;

class Timer {
  FunctionType m_Function = nullptr;
  bool m_bRun = true;
  bool m_bArmed = false;
  bool m_bFunctionTooExpensive = false;
  uint32_t m_u32Interval = 0;
  std::mutex m_Mutex;
  std::condition_variable m_Condition;
  uint32_t m_u32ModifiedInterval = 0;
  std::thread m_Thread;
  uint64_t m_u64FunctionStartTimestamp = 0;
  uint64_t m_u64FunctionEndTimestamp = 0;
  uint64_t m_u64FunctionTime = 0;

 public:
  enum StartResult {
    START_OK = 0, ALREADY_RUNNING, INTERVAL_NULL, FUNCTION_NULL
  };

  enum StopResult {
    STOP_OK = 0, ALREADY_STOPPED
  };

  explicit Timer(FunctionType i_Function): m_Function(std::move(i_Function)), m_Thread([this]{ loop(); }) {};
  Timer(FunctionType i_Function, uint32_t i_u32Interval): m_Function(std::move(i_Function)), m_u32Interval(i_u32Interval), m_Thread([this]{ loop(); }) {};

  ~Timer(){
    kill();
    join();
  }

  StartResult start() {
    if(m_Function == nullptr) {
      return FUNCTION_NULL;
    }
    if(m_bArmed) {
      return ALREADY_RUNNING;
    }
    if(m_u32Interval == 0) {
      return INTERVAL_NULL;
    }
    m_bRun = true;
    m_bArmed = true;
    m_Condition.notify_one();
    return START_OK;
  };

  StartResult start(uint32_t i_u32Interval) {
    m_u32Interval = i_u32Interval;
    m_u32ModifiedInterval = i_u32Interval;
    return start();
  };

  StopResult stop() {
    if(!m_bArmed) {
      return ALREADY_STOPPED;
    }
    m_bArmed = false;
    m_Condition.notify_one();
    return STOP_OK;
  };

  void kill() {
    m_bRun = false;
    m_bArmed = false;
    m_Condition.notify_one();
  }

  bool join() {
    bool r = false;
    if(m_Thread.joinable()) {
      m_Thread.join();
      r = true;
    }
    return r;
  }

  bool isRunning() const {
    return m_bRun;
  }

  bool isArmed() const {
    return m_bArmed;
  }

  bool isFunctionTooExpensive() const {
    return m_bFunctionTooExpensive;
  }

 protected:
  void _waitForArm() {
    UniqueLock lg(m_Mutex);
    m_Condition.wait(lg, [this]{ return m_bArmed || !m_bRun; });
  };

  void _executeFunction() {
    m_u64FunctionStartTimestamp = TIMESTAMP_MS;
    m_Function();
    m_u64FunctionEndTimestamp = TIMESTAMP_MS;
    m_u64FunctionTime = m_u64FunctionEndTimestamp - m_u64FunctionStartTimestamp;
    if(m_u64FunctionTime > m_u32Interval) {
      m_bFunctionTooExpensive = true;
      m_u32ModifiedInterval = 0;
    } else {
      m_u32ModifiedInterval = m_u32Interval - m_u64FunctionTime;
    }
  };

  void loop() {
    while(m_bRun) {
      _waitForArm();

      while(m_bArmed) {
        _executeFunction();
        std::this_thread::sleep_for(Milliseconds(m_u32ModifiedInterval));
      }
    }
  };
};
}

#endif//TIMERPP__TIMERPP_H_
