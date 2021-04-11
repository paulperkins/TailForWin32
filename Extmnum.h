/******************************************************************************
* (c)1998 GB Information Management. All rights reserved.
*
* Magic number module.
*
* File :    extmnum.h
* Date :    2 Apr 1998.
* Author :  Paul Perkins.
* Version : 0.0 (Development)
*
*
* Revision History.
*/

#ifndef EXTMNUM_H
#define EXTMNUM_H

#ifdef __cplusplus
extern "C"
{
#endif /* #ifdef __cplusplus */

extern BOOL UnpackLED (
  char*	pszLED,
  int*	pnDay,
  int*	pnMonth,
  int*	pnYear);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef EXTMNUM_H */

/* End of extmnum.h */
