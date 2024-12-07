#ifndef FULLOPS_H_
#define FULLOPS_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<errno.h>

ssize_t FullRead(int, void *, size_t);
ssize_t FullWrite(int, void *, size_t);  

#endif 
