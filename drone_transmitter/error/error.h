#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <string.h>

class error {
public:
	static void init();
	static void errorMSG(char* msg);
	static void errorMSGwithcode(const char *msg, int num);
};

#endif // !ERROR_H

