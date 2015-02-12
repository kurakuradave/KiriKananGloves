# KiriKananGloves
Accelerometerized Gloves For Controlling Mouse and Keyboard Shortcuts In A Zoomed-Desktop

Left Glove:
- moves mouse pointer Up, Down, Left and Right via hand-flipping
- uses LSM303 Tilt-compensated breakout board from Sparkfun

Right Glove:
- moves mouse wheel (scroll) up or down
- in combination with left glove movement, zooms in (when back of the two hands face each other), or zooms out ( when palms face each other )
- uses ADXL335 (3.3V version) from Adafruit.

Both gloves connected to an Arduino Micro.
There's also an On-Off switch for turning tracking on/off.
Via software, can set keyboard shortcuts for Ubuntu Gnome Desktop or Knoppix LXDE Desktop.
This is 100% Arduino code, HID functionalities are from hardware (ATMega 32U4 chip that can emulate mouse/keyboard).
Will play with Python version in future, so can use any type of Arduino ( other than ATMega 32U4 ), and XBee wireless.
Also will add more stuff in future, such as real-time clock, text-to-speech, capacitive touch pads, etc.
