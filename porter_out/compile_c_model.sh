#!/bin/bash
gcc svc_linear.c -std=c99 -lm -o svc_linear.o
gcc svc_poly.c -std=c99 -lm -o svc_poly.o
gcc svc_rbf.c -std=c99 -lm -o svc_rbf.o
gcc svc_sigmoid.c -std=c99 -lm -o svc_sigmoid.o
