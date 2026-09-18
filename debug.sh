#!/usr/bin/env bash

PID=$(pidof -s tf_linux64)
SO="$PWD/build/libjaci.so"

if [ ! -f "$SO" ]; then
	echo "No shared object found inside the build folder!"
	exit 1
fi

sudo gdb -q -n -p "$PID" \
	-ex "set pagination off" \
	-ex "set print pretty on" \
	-ex "call (void*)dlopen(\"$SO\", 2)" \
	-ex "call (char*)dlerror()" \
	-ex "call (void)InitLoucura()"
