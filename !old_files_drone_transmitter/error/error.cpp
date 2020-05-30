#include "error.h"

void error::init() {
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
}

void error::errorMSG(char * msg){
	printf("%s", msg);
}

void error::errorMSGwithcode(const char * msg, int num){
	char numstr[10];
	char fmsg[150];
	memset(fmsg, 0, sizeof(fmsg));
	sprintf(numstr, "%d", num);
	strcat(fmsg, msg);
	strcat(fmsg, " failed with the code:");
	strcat(fmsg, numstr);
	printf("%s", fmsg);
}
