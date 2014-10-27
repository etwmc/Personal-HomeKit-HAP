/*
** nsswitch.h              Name Service Switch support definitions
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
** Author: Peter Eriksson <pen@signum.se>
*/

#ifndef __NSSWITCH_H__
#define __NSSWITCH_H__

#define PATH_NSWCONF "/etc/nsswitch.conf"
#define NSW_MAXORDER 32

enum nsw_type
{
    NSWO_RETURN,
    NSWO_FILES,
    NSWO_NIS,
    NSWO_NISPLUS,
    NSWO_DNS,
    NSWO_DBM
};

struct nsw
{
  char name[33];
  int orderc;
  enum nsw_type orderl[NSW_MAXORDER];
};


extern int setnswent(void);
extern void endnswent(void);
extern struct nsw *getnswent(void);
extern struct nsw *getnswbyname(char *name);

extern char *_nsw_opts;


#ifdef FINALOUT
#  define _ENSWF(fname) fname
#else
#  define _ENSWF(fname) nsw ## fname
#endif

#endif
