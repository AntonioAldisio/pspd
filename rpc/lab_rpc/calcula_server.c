/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "calcula.h"

int *
add_100_svc(numbers *argp, struct svc_req *rqstp)
{
	static int  result;
	printf("sou soma");
	/*
	 * insert server code here
	 */

	return &result;
}

int *
sub_100_svc(numbers *argp, struct svc_req *rqstp)
{
	static int  result;

	printf("sou sub");
	/*
	 * insert server code here
	 */

	return &result;
}
