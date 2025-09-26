#!/bin/bash

make clean
if make; then
	clear
	./main
fi
