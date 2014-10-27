/*
** endXXent.h        Function definition file for endXXent() type calls
**
** Copyright (c) 1993 Signum Support AB, Sweden
**
** This file is part of the NYS Library.
**
** The NYS Library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version.
**
** The NYS Library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with the NYS Library; see the file COPYING.LIB.  If
** not, write to the Free Software Foundation, Inc., 675 Mass Ave,
** Cambridge, MA 02139, USA.
**
** Macros that should be defined before including this file:
**    RETOBJTYPE
**    FUNCOBJENT
**    NSWENTRY
**
**    NSWPREFIX
**    STDPREFIX
**
**    CONCAT(a,b)
*/

#define ENDPREFIX CONCAT(STDPREFIX,end)

static void CONCAT(_nsw_end,FUNCOBJENT)(int ptr)
{
    switch (nswp->orderl[ptr])
    {
      case NSWO_RETURN:
	break;
	
      case NSWO_FILES:
	CONCAT(ENDPREFIX,FUNCOBJENT)();
	break;
	    
#ifdef ENABLE_NIS
      case NSWO_NISPLUS:
	CONCAT(_nis_end,FUNCOBJENT)();
	break;
#endif
#ifdef ENABLE_YP
      case NSWO_NIS:
	CONCAT(_yp_end,FUNCOBJENT)();
	break;
#endif
#ifdef ENABLE_DNS
      case NSWO_DNS:
	CONCAT(_dns_end,FUNCOBJENT)();
	break;
#endif
#ifdef ENABLE_DBM
      case NSWO_DBM:
	CONCAT(_dbm_end,FUNCOBJENT)();
	break;
#endif
    }
}


void CONCAT(CONCAT(NSWPREFIX,end),FUNCOBJENT)(void)
{
    static int CONCAT(recur_,FUNCOBJENT) = 0;
    setflag = 0;
    
    
    if (nswp == NULL || CONCAT(recur_,FUNCOBJENT) == 1)
	CONCAT(ENDPREFIX,FUNCOBJENT)();
    else
    {
	CONCAT(recur_,FUNCOBJENT) = 1;
	if (setptr == nswp->orderc)
	    setptr--;
	
	CONCAT(_nsw_end,FUNCOBJENT)(setptr);
	
	CONCAT(recur_,FUNCOBJENT) = 0;
    }

    setptr = 0;
}


