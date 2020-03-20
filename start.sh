#!/bin/bash

for (( i = 0; i < 10; i++ ));do
./trinity_sdk_demo  -c 1 -n 25 &
done