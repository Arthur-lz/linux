#!/bin/bash
gcc -shared -fpic -o libvec.so addvec.c multvec.c
gcc -o prog2 main2.c ./libvec.so
