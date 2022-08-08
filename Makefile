default: build

build:
	avr-gcc -mmcu=atmega328p src/main.c -o src/main

burn: build
	avr-objcopy -O ihex -R .eeprom src/main src/main.hex

	avrdude -F -V -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:src/main.hex
