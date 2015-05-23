master status: [![Build Status](https://travis-ci.org/MauroMombelli/testChibiOsStm32F3.svg?branch=master)](https://travis-ci.org/MauroMombelli/testChibiOsStm32F3)

Audronomer stand for Autonomous drone, and unless many flight controller out there, it focus on making your drone as autonomous as possible.
It will make heavy use of math and physic to get this goal, so it won't be easy to set up (or even to write).

It will also try to use compile time safe code tring to follow some coding rule (non complete list);
 - no dynamic memory allocation
 - no structure initialization at runtime
 - all include into header file
 - no typedef

This code is based on 
* ChibiOS/RT by Giovanni Di Sirio @ chibios.org *

builded up starting from ChibiOS/RT stm32f3discovery demo,
and freely taking inspiration (and a bit of code) from

* freecopter-imu by formica-multiuso @ https://github.com/formica-multiuso/freecopter-imu *
* chibios-stm32f3discovery by kersny @ https://github.com/kersny/chibios-stm32f3discovery *
* KFly_ChibiOS by korken @ https://github.com/korken89/KFly_ChibiOS *
* cleanflight @ https://github.com/cleanflight/cleanflight *
