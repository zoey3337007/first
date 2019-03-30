
#pragma once
#include <windows.h>
int openPort(const char * dev_name);
int configurePort(int fd);
bool sendXYZ(int fd, double * xyz);

