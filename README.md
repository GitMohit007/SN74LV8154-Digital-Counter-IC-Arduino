# SN74LV8154-Digital-Counter-IC-Arduino
Arduino code for polling digital counter IC

TI SN74LV8154 is a dual 16-bit counter IC that allows counting of two separate signals up to 65,535 or by chaining the counters together 
a single 32-bit counter can be created allowing a signal to be counted up to 4,294,967,295.  The code in counter.ino provides an interface 
between this counter and Arduino.  Since the output from the counter is 8-bit parallel, it can be connected to a parallel-to-serial shift 
register (TI SN54HC165) to provide serial data to the Arduino.  The code also includes this feature.

Note that SN74LV8154 requires a register clock signal (RCLK).  In my circuit, I have used a CMOS 555 timer for this purpose but a PWM output
from Arduino can just as easily serve this purpose.
