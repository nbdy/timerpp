# timerpp

# features

- [X] header only
- [X] [tested](tests.cpp)
- [X] no dependencies
- [X] python bindings

# example

```c++
#include "timerpp.h"
#include <iostream>

int main() {
  uint64_t lastTimestamp = TIMESTAMP_MS;
  uint64_t newTimestamp = 0;
  uint32_t x = 0;
  timerpp::Timer t([&x, &newTimestamp, &lastTimestamp]{
    newTimestamp = TIMESTAMP_MS;
    x += newTimestamp - lastTimestamp;
    lastTimestamp = newTimestamp;
  });

  t.start(500);
  std::this_thread::sleep_for(Milliseconds(1501));
  t.stop();

  std::cout << "1500 == " <<  x << std::endl;

  return 0;
}
```