#!/bin/bash

ip link set can0 type can bitrate 125000
ip link set can0 txqueuelen 1000
ifconfig can0 up
