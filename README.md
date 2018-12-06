# pi-pin
Small utility for manipulating with GPIO pins of Raspberry PI
# Build
g++ -o pi-pin pi-pin.cpp
# Usage
pi-pin pin command [PWM value (0-255)]
- command options:
  - s0  set pin LOW
  - s1  - set pin HIGH
  - t   - toggle pin
  - p   - PWM
  - g   - get pin value
  - r   - release pin
# License
https://www.gnu.org/licenses/gpl-3.0.en.html
