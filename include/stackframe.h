/*
 * 16-bit mode stack frame layout
 *
 * Copyright 1995 Alexandre Julliard
 */

#ifndef __WINE_STACKFRAME_H
#define __WINE_STACKFRAME_H

#include <string.h>
#include "windows.h"
#include "ldt.h"

#pragma pack(1)

  /* 32-bit stack layout after CallTo16() */
typedef struct _STACK32FRAME
{
    SEGPTR  frame16;        /* 00 16-bit frame from last CallFrom16() */
    DWORD   edi;            /* 04 saved registers */
    DWORD   esi;            /* 08 */
    DWORD   edx;            /* 0c */
    DWORD   ecx;            /* 10 */
    DWORD   ebx;            /* 14 */
    DWORD   restore_addr;   /* 18 return address for restoring code selector */
    DWORD   codeselector;   /* 1c code selector to restore */
    DWORD   ebp;            /* 20 saved 32-bit frame pointer */
    DWORD   retaddr;        /* 24 actual return address */
    DWORD   args[1];        /* 28 arguments to 16-bit function */
} STACK32FRAME;

  /* 16-bit stack layout after CallFrom16() */
typedef struct
{
    STACK32FRAME *frame32;        /* 00 32-bit frame from last CallTo16() */
    DWORD         ebp;            /* 04 full 32-bit content of ebp */
    WORD          entry_ip;       /* 08 ip of entry point */
    WORD          ds;             /* 0a ds */
    WORD          entry_cs;       /* 0c cs of entry point */
    WORD          es;             /* 0e es */
    DWORD         entry_point;    /* 10 32-bit entry point to call */
    WORD          bp;             /* 14 16-bit bp */
    WORD          ip;             /* 16 return address */
    WORD          cs;             /* 18 */
} STACK16FRAME;

#pragma pack(4)

  /* Saved 16-bit stack for current process (Win16 only) */
extern DWORD IF1632_Saved16_ss_sp;

#define CURRENT_STACK16 ((STACK16FRAME *)PTR_SEG_TO_LIN(IF1632_Saved16_ss_sp))
#define CURRENT_DS      (CURRENT_STACK16->ds)

/* varargs lists on the 16-bit stack */

typedef void *VA_LIST16;

#define __VA_ROUNDED16(type) \
    ((sizeof(type) + sizeof(WORD) - 1) / sizeof(WORD) * sizeof(WORD))
#define VA_START16(list) ((list) = (VA_LIST16)(CURRENT_STACK16 + 1))
#define VA_ARG16(list,type) \
    (((list) = (VA_LIST16)((char *)(list) + __VA_ROUNDED16(type))), \
     *((type *)(void *)((char *)(list) - __VA_ROUNDED16(type))))
#define VA_END16(list) ((void)0)

/* Push bytes on the 16-bit stack; return a segptr to the first pushed byte */
#define STACK16_PUSH(size) \
 (memmove((char*)CURRENT_STACK16-(size),CURRENT_STACK16,sizeof(STACK16FRAME)),\
  IF1632_Saved16_ss_sp -= (size), \
  (SEGPTR)(IF1632_Saved16_ss_sp + sizeof(STACK16FRAME)))

/* Pop bytes from the 16-bit stack */
#define STACK16_POP(size) \
 (memmove((char*)CURRENT_STACK16+(size),CURRENT_STACK16,sizeof(STACK16FRAME)),\
  IF1632_Saved16_ss_sp += (size))

#endif /* __WINE_STACKFRAME_H */
