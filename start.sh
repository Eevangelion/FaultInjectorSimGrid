#!/bin/bash
cmake --build .
./FaultInjectorSimGrid platform.xml > logs.txt 2>&1