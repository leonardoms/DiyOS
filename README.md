[![Build Status](https://travis-ci.org/leonardoms/DiyOS.svg?branch=master)](https://travis-ci.org/leonardoms/DiyOS)
# DiyOS - do it yourself
Small study Operating System

This is a very very very experimental code, no warranties, it's for own study.
---
Latest built ISO @ [build/](build/)

Latest boot screenshot
![Boot Screenshot](build/screenshot.png)
---

What is done:
* Multitasking  
* Global Descriptor Table (GDT)
  * Flat memory
* Interrupt Descriptor Table (IDT)
  * Basic ISR (only print a message and halt)
  * IRQs
* Paging enabled
  * Low Memory (0 ~ 1MB) identity paging
  * High-Half Kernel (kernel above 0xC0000000)
  * 4MB at 0xFFC00000 fixed for Page Tables (physical memory after kernel location)
  * kmalloc() and kfree() (bitmap manager with flags on 3-bit unused on each Page Table entry)
* Drivers
  * keyboard, Timer, Basic Video, PIC
  * Bochs VBE video
  * Basic VGA
  * Network Realtek RTL81xx
  * Network NE2000 (n2k) [working on]

To  compile:
```
make
```
To create a disk iso file:
```
make iso
```
To run with Bochs (starts paused, just type 'cont' on debugger):
```
make run
```
