#!/bin/bash

ip link set can0 type can bitrate 125000
ifconfig can0 up
