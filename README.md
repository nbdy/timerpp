# timerpp

# features

- [X] header only
- [X] [tested](tests.cpp)
- [X] no dependencies
- [X] python bindings
  - `pip3 install git+https://github.com/nbdy/timerpp`
  - `pip3 install timerpp`

# example

## cpp

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

## python

```python
from timerpp import Timer
from time import sleep


class Example(Timer):
  elapsed_ms = 0

  def __init__(self):
    Timer.__init__(self, self.callback, 500)

  def callback(self):
    self.elapsed_ms += 500


e = Example()
e.start()
sleep(1.49)
e.stop()
print(f"elapsed_ms: {e.elapsed_ms}")
```