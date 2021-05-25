#!/bin/bash
gcc -shared -fpic -o libvec.so addvec.c multvec.c
gcc -rdynamic -o prog2 main.c -ldl
