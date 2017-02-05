/* file g1arith */
#include "grs.h"
#include "grglobal.h"
#include "conio.h"
#include "stdlib.h"

/* *****************************************************************  */
int pInverse(int arg)
{
  long temp;
  int i;

  temp = 1;

  for (i=1;i<=prime-2;i++) temp = (temp*(long) arg)%prime;
  return( (temp >= 0)? (int) temp:(int) (temp + (long) prime));
}
/* ***************************************************************** */

int pMult(int arg1,int arg2)

{
  long temp1,temp2;
  temp1 = (long) arg1;
  temp2 = (temp1 * (long) arg2)%prime;
  return( (temp2 < 0)?(int) (temp2+(long) prime):(int) temp2);
}
/* ***************************************************************** */

int pAdd(int arg1,int arg2)

{
  long temp;

  temp = (long) arg1;
  temp +=(long) arg2;
  temp = temp%(long) prime;
  return((temp>=0)?(int) temp:(int) (temp+prime));
}
/* ***************************************************************** */

int pAdjust(int arg)
{
  arg = arg% prime;
  return((arg>=0)?arg:arg+prime);
}
/* ******************************************************************  */


