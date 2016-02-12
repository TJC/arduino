
# make show_boards gives a list of board tags
BOARD_TAG = nano328
ARDUINO_PORT = $(wildcard /dev/ttyUSB*)
# Libs are in $(ARDUINO_DIR)/hardware/libraries
ARDUINO_LIBS = 

# RESET_CMD = python -c "from serial import Serial;from time import sleep;s=Serial('$(ARDUINO_PORT)', 57600);sleep(0.25);s.close();s.open();sleep(0.25);s.close();s.setBaudrate(1200);s.open();sleep(0.25);s.close();sleep(1);"
RESET_CMD="/bin/true"

include /usr/share/arduino/Arduino.mk

#   make             - no upload
#   make upload
#   make clean       - remove all our dependencies
#   make depends     - update dependencies
#   make reset       - reset the Arduino by tickling DTR on the serial port
#   make raw_upload  - upload without first resetting
#   make show_boards - list all the boards defined in boards.txt
#   make monitor     - connect to the Arduino's serial port


