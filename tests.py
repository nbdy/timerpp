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
