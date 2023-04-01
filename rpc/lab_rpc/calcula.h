/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _CALCULA_H_RPCGEN
#define _CALCULA_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct numbers {
	int x;
	int y;
};
typedef struct numbers numbers;

#define SUB_ADD_PROG 0x23451111
#define SUB_ADD_PROG 100

#if defined(__STDC__) || defined(__cplusplus)
#define add 1
extern  int * add_100(numbers *, CLIENT *);
extern  int * add_100_svc(numbers *, struct svc_req *);
#define sub 2
extern  int * sub_100(numbers *, CLIENT *);
extern  int * sub_100_svc(numbers *, struct svc_req *);
extern int sub_add_prog_100_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define add 1
extern  int * add_100();
extern  int * add_100_svc();
#define sub 2
extern  int * sub_100();
extern  int * sub_100_svc();
extern int sub_add_prog_100_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_numbers (XDR *, numbers*);

#else /* K&R C */
extern bool_t xdr_numbers ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_CALCULA_H_RPCGEN */
