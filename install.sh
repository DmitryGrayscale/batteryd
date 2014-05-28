#!/bin/sh

ISACPI=$(dpkg -l acpi  2> /dev/null | grep -o ii)
ISGST=$(dpkg -l libgstreamer1.0-dev 2> /dev/null | grep -o ii)

if [ "$ISACPI" != 'ii' -o "$ISGST" != 'ii' ]
	then
		sudo apt-get install acpi libgstreamer1.0-dev
fi

sudo gcc -o /usr/bin/batteryd batteryd.c  `pkg-config --cflags --libs gstreamer-1.0`
cp battery.mp3 ~/.local/battery.mp3
