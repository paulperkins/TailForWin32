/******************************************************************************
* (c)1998 GB Information Management. All rights reserved.
*
* Magic number test module.
*
* File :    EXTMNUM.C
* Date :    13 Mar 1998.
* Author :  Paul Perkins.
* Version : 0.0 (Development)
*
*
* Revision History.
*/                               

//#include "nrstdhdr.h"   /* Standard defines and includes. */
//#include "cusec.h"      /* Parameter block definition. */
#include <windows.h>

#include "mnumber.h"    /* Magic number helper functions. */
#include <stdio.h>
//#include "gbscerr.h"


/******************************************************************************
* UnpackLED
*/
BOOL UnpackLED (
  char*	pszLED,
  int*	pnDay,
  int*	pnMonth,
  int*	pnYear)
{
  BYTE  Magic[12];
  char    szMagic[48];
  BYTESWAP Jumble[12] = { {0, 3},  /* 11 -> 0 */
                          {1, 5},   /* 2 -> 1 */
                          {2, 4},   /* etc */
                          {3, 0},
                          {4, 1},
                          {5, 6},
                          {6, 2}};
  BYTE  nDay = 0;
  BYTE  nMonth = 0;
  WORD  wYear = 0;
  crc32_t crc = 0;
  crc32_t crcNumber = 0;

  memset (&szMagic[0], 0, sizeof (szMagic));

  strcpy (szMagic, pszLED);

  memset (&Magic[0], 0, sizeof (Magic));

  UnMakeDisplay (szMagic, Magic);

  ByteSwap (Magic, &Jumble[0], 7);

  crcNumber = GetChecksum (Magic, 3);

  crc = Magic[6];
  crc |= Magic[5] << 8;
  crc |= Magic[4] << 16;
  crc |= Magic[3] << 24;

  if (crc != crcNumber)
  {
    return FALSE;
  }

  wYear = Magic[2];                 /* Bottom 8 bits of year. */
  wYear |= (Magic[1] & 0xfe) << 7;  /* Top 7 bits of year. */

  nMonth = ((Magic[1] & 0x01) << 3) & 0x08;   /* Top bit of month. */
  nMonth |= ((BYTE) (Magic[0] & 0xf0) >> 5) & 0x07;  /* Bottom 3 bits of month. */

  nDay = Magic[0] & 0x1f;

  if (pnDay)
	  *pnDay = nDay;

  if (pnMonth)
	  *pnMonth = nMonth;

  if (pnYear)
	  *pnYear = wYear;

//  sprintf (pParams->szLED, "%02d/%02d/%4d", nDay, nMonth, wYear);

  return TRUE;
}


/* End of EXTMNUM.C */

