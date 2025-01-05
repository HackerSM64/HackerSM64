#ifndef _ULTRA64_H_
#define _ULTRA64_H_

/* ultratypes.h */
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

/* rcp.h */
#define K0BASE                  0x80000000
#define K1BASE                  0xA0000000
#define PI_BASE_REG             0x04600000
#define PI_DRAM_ADDR_REG        (PI_BASE_REG+0x00)
#define PI_CART_ADDR_REG        (PI_BASE_REG+0x04)
#define PI_RD_LEN_REG           (PI_BASE_REG+0x08)
#define PI_WR_LEN_REG           (PI_BASE_REG+0x0C)
#define PI_STATUS_REG           (PI_BASE_REG+0x10)
#define PI_BSD_DOM1_LAT_REG     (PI_BASE_REG+0x14)
#define PI_BSD_DOM1_PWD_REG     (PI_BASE_REG+0x18)
#define PI_BSD_DOM1_PGS_REG     (PI_BASE_REG+0x1C)
#define PI_BSD_DOM1_RLS_REG     (PI_BASE_REG+0x20)
#define PI_BSD_DOM2_LAT_REG     (PI_BASE_REG+0x24)
#define PI_BSD_DOM2_PWD_REG     (PI_BASE_REG+0x28)
#define PI_BSD_DOM2_PGS_REG     (PI_BASE_REG+0x2C)
#define PI_BSD_DOM2_RLS_REG     (PI_BASE_REG+0x30)
#define PI_STATUS_ERROR         0x04
#define PI_STATUS_IO_BUSY       0x02
#define PI_STATUS_DMA_BUSY      0x01
#define PI_DOM1_ADDR1           0x06000000
#define PI_DOM1_ADDR2           0x10000000
#define PI_DOM1_ADDR3           0x1FD00000
#define PI_DOM2_ADDR1           0x05000000
#define PI_DOM2_ADDR2           0x08000000

/* math.h */
#define M_PI                    3.14159265358979323846

/* os.h */
#define OS_PIM_STACKSIZE        4096
#define OS_VIM_STACKSIZE        4096
#define OS_SIM_STACKSIZE        4096
#define OS_MIN_STACKSIZE        72

/* os_thread.h */
#define OS_STATE_STOPPED        1
#define OS_STATE_RUNNABLE       2
#define OS_STATE_RUNNING        4
#define OS_STATE_WAITING        8
#define OS_PRIORITY_MAX         255
#define OS_PRIORITY_VIMGR       254
#define OS_PRIORITY_RMON        250
#define OS_PRIORITY_RMONSPIN    200
#define OS_PRIORITY_PIMGR       150
#define OS_PRIORITY_SIMGR       140
#define OS_PRIORITY_APPMAX      127
#define OS_PRIORITY_IDLE        0

/* os_message.h */
#define OS_NUM_EVENTS           15
#define OS_EVENT_SW1            0
#define OS_EVENT_SW2            1
#define OS_EVENT_CART           2
#define OS_EVENT_COUNTER        3
#define OS_EVENT_SP             4
#define OS_EVENT_SI             5
#define OS_EVENT_AI             6
#define OS_EVENT_VI             7
#define OS_EVENT_PI             8
#define OS_EVENT_DP             9
#define OS_EVENT_CPU_BREAK      10
#define OS_EVENT_SP_BREAK       11
#define OS_EVENT_FAULT          12
#define OS_EVENT_THREADSTATUS   13
#define OS_EVENT_PRENMI         14
#define OS_MESG_NOBLOCK         0
#define OS_MESG_BLOCK           1

/* os_exception.h */
#define OS_FLAG_CPU_BREAK       1
#define OS_FLAG_FAULT           2
#define OS_IM_NONE              0x00000001
#define OS_IM_SW1               0x00000501
#define OS_IM_SW2               0x00000601
#define OS_IM_CART              0x00000c01
#define OS_IM_PRENMI            0x00001401
#define OS_IM_RDBWRITE          0x00002401
#define OS_IM_RDBREAD           0x00004401
#define OS_IM_COUNTER           0x00008401
#define OS_IM_CPU               0x0000ff01
#define OS_IM_SP                0x00010401
#define OS_IM_SI                0x00020401
#define OS_IM_AI                0x00040401
#define OS_IM_VI                0x00080401
#define OS_IM_PI                0x00100401
#define OS_IM_DP                0x00200401
#define OS_IM_ALL               0x003fff01
#define RCP_IMASK               0x003f0000
#define RCP_IMASKSHIFT          16

/* os_tlb.h */
#define OS_PM_4K        0x0000000
#define OS_PM_16K       0x0006000
#define OS_PM_64K       0x001e000
#define OS_PM_256K      0x007e000
#define OS_PM_1M        0x01fe000
#define OS_PM_4M        0x07fe000
#define OS_PM_16M       0x1ffe000

/* os_pi.h */
#define OS_READ                 0
#define OS_WRITE                1
#define OS_MESG_TYPE_BASE       (10)
#define OS_MESG_TYPE_LOOPBACK   (OS_MESG_TYPE_BASE+0)
#define OS_MESG_TYPE_DMAREAD    (OS_MESG_TYPE_BASE+1)
#define OS_MESG_TYPE_DMAWRITE   (OS_MESG_TYPE_BASE+2)
#define OS_MESG_TYPE_VRETRACE   (OS_MESG_TYPE_BASE+3)
#define OS_MESG_TYPE_COUNTER    (OS_MESG_TYPE_BASE+4)
#define OS_MESG_TYPE_EDMAREAD   (OS_MESG_TYPE_BASE+5)
#define OS_MESG_TYPE_EDMAWRITE  (OS_MESG_TYPE_BASE+6)
#define OS_MESG_PRI_NORMAL      0
#define OS_MESG_PRI_HIGH        1

/* os_vi.h */
#define OS_VI_NTSC_LPN1         0
#define OS_VI_NTSC_LPF1         1
#define OS_VI_NTSC_LAN1         2
#define OS_VI_NTSC_LAF1         3
#define OS_VI_NTSC_LPN2         4
#define OS_VI_NTSC_LPF2         5
#define OS_VI_NTSC_LAN2         6
#define OS_VI_NTSC_LAF2         7
#define OS_VI_NTSC_HPN1         8
#define OS_VI_NTSC_HPF1         9
#define OS_VI_NTSC_HAN1         10
#define OS_VI_NTSC_HAF1         11
#define OS_VI_NTSC_HPN2         12
#define OS_VI_NTSC_HPF2         13
#define OS_VI_PAL_LPN1          14
#define OS_VI_PAL_LPF1          15
#define OS_VI_PAL_LAN1          16
#define OS_VI_PAL_LAF1          17
#define OS_VI_PAL_LPN2          18
#define OS_VI_PAL_LPF2          19
#define OS_VI_PAL_LAN2          20
#define OS_VI_PAL_LAF2          21
#define OS_VI_PAL_HPN1          22
#define OS_VI_PAL_HPF1          23
#define OS_VI_PAL_HAN1          24
#define OS_VI_PAL_HAF1          25
#define OS_VI_PAL_HPN2          26
#define OS_VI_PAL_HPF2          27
#define OS_VI_GAMMA_ON                  0x0001
#define OS_VI_GAMMA_OFF                 0x0002
#define OS_VI_GAMMA_DITHER_ON           0x0004
#define OS_VI_GAMMA_DITHER_OFF          0x0008
#define OS_VI_DIVOT_ON                  0x0010
#define OS_VI_DIVOT_OFF                 0x0020
#define OS_VI_DITHER_FILTER_ON          0x0040
#define OS_VI_DITHER_FILTER_OFF         0x0080
#define OS_VI_BIT_NONINTERLACE          0x0001
#define OS_VI_BIT_INTERLACE             0x0002
#define OS_VI_BIT_NORMALINTERLACE       0x0004
#define OS_VI_BIT_DEFLICKINTERLACE      0x0008
#define OS_VI_BIT_ANTIALIAS             0x0010
#define OS_VI_BIT_POINTSAMPLE           0x0020
#define OS_VI_BIT_16PIXEL               0x0040
#define OS_VI_BIT_32PIXEL               0x0080
#define OS_VI_BIT_LORES                 0x0100
#define OS_VI_BIT_HIRES                 0x0200
#define OS_VI_BIT_NTSC                  0x0400
#define OS_VI_BIT_PAL                   0x0800

/* os_cont.h */
#define MAXCONTROLLERS          4
#define CONT_NO_RESPONSE_ERROR  0x8
#define CONT_OVERRUN_ERROR      0x4
#define CONT_ABSOLUTE           0x0001
#define CONT_RELATIVE           0x0002
#define CONT_JOYPORT            0x0004
#define CONT_EEPROM             0x8000
#define CONT_EEP16K             0x4000
#define CONT_TYPE_MASK          0x1f07
#define CONT_TYPE_NORMAL        0x0005
#define CONT_TYPE_MOUSE         0x0002
#define CONT_CARD_ON            0x01
#define CONT_CARD_PULL          0x02
#define CONT_ADDR_CRC_ER        0x04
#define CONT_EEPROM_BUSY        0x80
#define CONT_A                  0x8000
#define CONT_B                  0x4000
#define CONT_G                  0x2000
#define CONT_START              0x1000
#define CONT_UP                 0x0800
#define CONT_DOWN               0x0400
#define CONT_LEFT               0x0200
#define CONT_RIGHT              0x0100
#define CONT_L                  0x0020
#define CONT_R                  0x0010
#define CONT_E                  0x0008
#define CONT_D                  0x0004
#define CONT_C                  0x0002
#define CONT_F                  0x0001
#define A_BUTTON                CONT_A
#define B_BUTTON                CONT_B
#define L_TRIG                  CONT_L
#define R_TRIG                  CONT_R
#define Z_TRIG                  CONT_G
#define START_BUTTON            CONT_START
#define U_JPAD                  CONT_UP
#define L_JPAD                  CONT_LEFT
#define R_JPAD                  CONT_RIGHT
#define D_JPAD                  CONT_DOWN
#define U_CBUTTONS              CONT_E
#define L_CBUTTONS              CONT_C
#define R_CBUTTONS              CONT_F
#define D_CBUTTONS              CONT_D
#define CONT_ERR_NO_CONTROLLER  1
#define CONT_ERR_CONTRFAIL      4
#define CONT_ERR_INVALID        5
#define CONT_ERR_DEVICE         11
#define CONT_ERR_NOT_READY      12

/* os_system.h */
#define OS_TV_PAL               0
#define OS_TV_NTSC              1
#define OS_TV_MPAL              2
#define OS_APP_NMI_BUFSIZE      64

/* sptask.h */
#if (defined(F3DEX_GBI)||defined(F3DLP_GBI)||defined(F3DEX_GBI_2))
#define OS_YIELD_DATA_SIZE      0xc00
#else
#define OS_YIELD_DATA_SIZE      0x900
#endif
#define OS_YIELD_AUDIO_SIZE     0x400

/* mbi.h */
#define G_ON                    (1)
#define G_OFF                   (0)
#define M_GFXTASK               1
#define M_AUDTASK               2
#define NUM_SEGMENTS            (16)
#define SEGMENT_ADDR(num, off)  (((num) << 24) + (off))

/* ucode.h */
#define SP_DRAM_STACK_SIZE8     (1024)
#define SP_DRAM_STACK_SIZE64    (SP_DRAM_STACK_SIZE8 >> 3)
#define SP_UCODE_SIZE           4096
#define SP_UCODE_DATA_SIZE      2048

#ifndef __ASSEMBLER__

/* ultratypes.h */
#ifdef sgi
#ifndef NULL
#define NULL    ((void *)0)
#endif
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       u32;
typedef unsigned long long  u64;
typedef signed char s8;
typedef short       s16;
typedef long        s32;
typedef long long   s64;
typedef long intptr_t;
typedef unsigned long uintptr_t;
typedef unsigned long size_t;
#else
#include <stddef.h>
#include <stdint.h>
typedef  int8_t  s8;
typedef uint8_t  u8;
typedef  int16_t s16;
typedef uint16_t u16;
typedef  int32_t s32;
typedef uint32_t u32;
typedef  int64_t s64;
typedef uint64_t u64;
#endif

typedef float    f32;
typedef double   f64;

/* rcp.h */
#define IO_READ(addr)       (*(volatile u32 *)PHYS_TO_K1(addr))
#define IO_WRITE(addr,data) (*(volatile u32 *)PHYS_TO_K1(addr) = (u32)(data))

/* R4300.h */
#define K0_TO_K1(x)     ((u32)(x)|0xA0000000)
#define K1_TO_K0(x)     ((u32)(x)&0x9FFFFFFF)
#define K0_TO_PHYS(x)   ((u32)(x)&0x1FFFFFFF)
#define K1_TO_PHYS(x)   ((u32)(x)&0x1FFFFFFF)
#define KDM_TO_PHYS(x)  ((u32)(x)&0x1FFFFFFF)
#define PHYS_TO_K0(x)   ((u32)(x)|0x80000000)
#define PHYS_TO_K1(x)   ((u32)(x)|0xA0000000)

/* os_thread.h */
typedef s32 OSPri;
typedef s32 OSId;
typedef union {struct {f32 f_odd; f32 f_even;} f; f64 d;} __OSfp;
typedef struct
{
    u64     at, v0, v1, a0, a1, a2, a3;
    u64 t0, t1, t2, t3, t4, t5, t6, t7;
    u64 s0, s1, s2, s3, s4, s5, s6, s7;
    u64 t8, t9,         gp, sp, s8, ra;
    u64 lo, hi;
    u32 sr, pc, cause, badvaddr, rcp;
    u32 fpcsr;
    __OSfp  fp0,  fp2,  fp4,  fp6,  fp8, fp10, fp12, fp14;
    __OSfp fp16, fp18, fp20, fp22, fp24, fp26, fp28, fp30;
}
__OSThreadContext;
typedef struct OSThread_s
{
    struct OSThread_s  *next;
    OSPri               priority;
    struct OSThread_s **queue;
    struct OSThread_s  *tlnext;
    u16                 state;
    u16                 flags;
    OSId                id;
    s32                 fp;
    __OSThreadContext   context;
}
OSThread;

/* os_message.h */
#define MQ_GET_COUNT(mq)        ((mq)->validCount)
#define MQ_IS_EMPTY(mq)         (MQ_GET_COUNT(mq) == 0)
#define MQ_IS_FULL(mq)          (MQ_GET_COUNT(mq) >= (mq)->msgCount)
typedef u32 OSEvent;
typedef void *OSMesg;
typedef struct OSMesgQueue_s
{
    OSThread           *mtqueue;
    OSThread           *fullqueue;
    s32                 validCount;
    s32                 first;
    s32                 msgCount;
    OSMesg             *msg;
}
OSMesgQueue;

/* os_exception.h */
typedef u32 OSIntMask;
typedef u32 OSHWIntr;

/* os_tlb.h */
typedef u32 OSPageMask;

/* os_pi.h */
typedef struct
{
    u16                 type;
    u8                  pri;
    u8                  status;
    OSMesgQueue        *retQueue;
}
OSIoMesgHdr;
typedef struct
{
    OSIoMesgHdr         hdr;
    void               *dramAddr;
    u32                 devAddr;
    u32                 size;
}
OSIoMesg;
typedef struct
{
    s32                 active;
    OSThread           *thread;
    OSMesgQueue        *cmdQueue;
    OSMesgQueue        *evtQueue;
    OSMesgQueue        *acsQueue;
    s32               (*dma)(s32, u32, void *, u32);
}
OSDevMgr;

/* os_vi.h */
typedef struct
{
    u32                 ctrl;
    u32                 width;
    u32                 burst;
    u32                 vSync;
    u32                 hSync;
    u32                 leap;
    u32                 hStart;
    u32                 xScale;
    u32                 vCurrent;
}
OSViCommonRegs;
typedef struct
{
    u32                 origin;
    u32                 yScale;
    u32                 vStart;
    u32                 vBurst;
    u32                 vIntr;
}
OSViFieldRegs;
typedef struct
{
    u8                  type;
    OSViCommonRegs      comRegs;
    OSViFieldRegs       fldRegs[2];
}
OSViMode;

/* os_time.h */
typedef u64 OSTime;
typedef struct OSTimer_s
{
    struct OSTimer_s   *next;
    struct OSTimer_s   *prev;
    OSTime              interval;
    OSTime              value;
    OSMesgQueue        *mq;
    OSMesg              msg;
}
OSTimer;

/* os_cont.h */
typedef struct
{
    u16                 type;
    u8                  status;
    u8                  errno;
}
OSContStatus;
typedef struct
{
    u16                 button;
    s8                  stick_x;
    s8                  stick_y;
    u8                  errno;
}
OSContPad;

/* os_convert.h */
#define OS_K0_TO_PHYSICAL(x)    (u32)(((char *)(x)-0x80000000))
#define OS_PHYSICAL_TO_K0(x)    (void *)(((u32)(x)+0x80000000))

/* sptask.h */
#define osSpTaskStart(tp)   \
{                           \
    osSpTaskLoad((tp));     \
    osSpTaskStartGo((tp));  \
}
typedef struct
{
    u32                 type;
    u32                 flags;
    u64                *ucode_boot;
    u32                 ucode_boot_size;
    u64                *ucode;
    u32                 ucode_size;
    u64                *ucode_data;
    u32                 ucode_data_size;
    u64                *dram_stack;
    u32                 dram_stack_size;
    u64                *output_buff;
    u64                *output_buff_size;
    u64                *data_ptr;
    u32                 data_size;
    u64                *yield_data_ptr;
    u32                 yield_data_size;
}
OSTask_t;
typedef union
{
    OSTask_t            t;
    u64                 force_structure_alignment;
}
OSTask;
typedef u32 OSYieldResult;

/* mbi.h */
#define _SHIFTL(v, s, w)        \
    ((unsigned int) (((unsigned int)(v) & ((0x01 << (w)) - 1)) << (s)))
#define _SHIFTR(v, s, w)        \
    ((unsigned int)(((unsigned int)(v) >> (s)) & ((0x01 << (w)) - 1)))
#include <gbi.h>
#include <abi.h>
#define SEGMENT_OFFSET(a)       ((unsigned int)(a) & 0x00ffffff)
#define SEGMENT_NUMBER(a)       (((unsigned int)(a) << 4) >> 28)

/* 0x80000300 */ extern s32 osTvType;
/* 0x80000304 */ extern s32 osRomType;
/* 0x80000308 */ extern void *osRomBase;
/* 0x8000030C */ extern s32 osResetType;
/* 0x80000310 */ extern s32 osCicId;
/* 0x80000314 */ extern s32 osVersion;
/* 0x80000318 */ extern u32 osMemSize;
/* 0x8000031C */ extern s32 osAppNMIBuffer[];

/* 0x803223B0 */ extern void osSetTime(OSTime);
/* 0x803223E0 */ extern void osMapTLB(s32, OSPageMask, void *, u32, u32, s32);
/* 0x803224A0 */ extern void osUnmapTLBAll(void);
/* 0x803224F0 */ extern s32 sprintf(char *, const char *, ...);
/* 0x803225A0 */ extern void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
/* 0x803225D0 */ extern void osSetEventMesg(OSEvent, OSMesgQueue *, OSMesg);
/* 0x80322640 */ extern void osViSetEvent(OSMesgQueue *, OSMesg, u32);
/* 0x803226B0 */ extern void osCreateThread(
    OSThread *, OSId, void (*)(void *), void *, void *, OSPri
);
/* 0x80322800 */ extern s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
/* 0x80322A5C */ extern void osSpTaskLoad(OSTask *);
/* 0x80322BBC */ extern s32 osSpTaskStartGo(OSTask *);
/* 0x80322C00 */ extern s32 osSpTaskYield(void);
/* 0x80322C20 */ extern void osSendMesg(OSMesgQueue *, OSMesg, s32);
/* 0x80322D70 */ extern OSYieldResult osSpTaskYielded(OSTask *);
/* 0x80322DF0 */ extern void osStartThread(OSThread *);
/* 0x80322F40 */ extern void osWritebackDCacheAll(void);
/* 0x80322F70 */ extern void osCreateViManager(OSPri);
/* 0x803232D0 */ extern void osViSetMode(OSViMode *);
/* 0x80323340 */ extern void osViBlack(u8);
/* 0x803233B0 */ extern void osViSetSpecialFeatures(u32);
/* 0x80323570 */ extern void osCreatePiManager(
    OSPri, OSMesgQueue *, OSMesg *, s32
);
/* 0x803236F0 */ extern void osSetThreadPri(OSThread *, OSPri);
/* 0x803237D0 */ extern void osInitialize(void);
/* 0x80323A00 */ extern void osViSwapBuffer(void *);
/* 0x80323A50 */ extern f32 sqrtf(f32);
/* 0x80323A60 */ extern s32 osContStartReadData(OSMesgQueue *);
/* 0x80323B24 */ extern void osContGetReadData(OSContPad *);
/* 0x80323CC0 */ extern s32 osContInit(OSMesgQueue *, u8 *, OSContStatus *);
/* 0x80324080 */ extern s32 osEepromProbe(OSMesgQueue *);
/* 0x803243B0 */ extern void osInvalDCache(void *, s32);
/* 0x80324460 */ extern s32 osPiStartDma(
    OSIoMesg *, s32, s32, u32, void *, u32, OSMesgQueue *
);
/* 0x80324570 */ extern void bzero(void *, u32);
/* 0x80324610 */ extern void osInvalICache(void *, s32);
/* 0x80324690 */ extern s32 osEepromLongRead(OSMesgQueue *, u8, u8 *, s32);
/* 0x803247D0 */ extern s32 osEepromLongWrite(OSMesgQueue *, u8, u8 *, s32);
/* 0x80324910 */ extern void bcopy(const void *, void *, size_t);
/* 0x80324C20 */ extern void guOrthoF(
    f32[4][4], f32, f32, f32, f32, f32, f32, f32
);
/* 0x80324D74 */ extern void guOrtho(Mtx *, f32, f32, f32, f32, f32, f32, f32);
/* 0x80324DE0 */ extern void guPerspectiveF(
    f32[4][4], u16 *, f32, f32, f32, f32, f32
);
/* 0x80325010 */ extern void guPerspective(
    Mtx *, u16 *, f32, f32, f32, f32, f32
);
/* 0x80325070 */ extern OSTime osGetTime(void);
/* 0x80325310 */ extern f32 cosf(f32);
/* 0x80325480 */ extern f32 sinf(f32);
/* 0x80325640 */ extern void guTranslateF(f32[4][4], f32, f32, f32);
/* 0x80325688 */ extern void guTranslate(Mtx *, f32, f32, f32);
/* 0x803256E0 */ extern void guRotateF(f32[4][4], f32, f32, f32, f32);
/* 0x80325874 */ extern void guRotate(Mtx *, f32, f32, f32, f32);
/* 0x803258D0 */ extern void guScaleF(f32[4][4], f32, f32, f32);
/* 0x80325924 */ extern void guScale(Mtx *, f32, f32, f32);
/* 0x80325970 */ extern s32 osAiSetFrequency(u32);
/* 0x80325CD8 */ extern void alSeqFileNew(void *, u8 *);
/* 0x80325D20 */ extern void osWritebackDCache(void *, s32);
/* 0x80325DA0 */ extern u32 osAiGetLength(void);
/* 0x80325DB0 */ extern s32 osAiSetNextBuffer(void *, u32);
/* 0x803273F0 */ extern void *memcpy(void *, const void *, size_t);
/* 0x8032741C */ extern size_t strlen(const char *);
/* 0x80327444 */ extern char *strchr(const char *, int);
/* 0x80327EB0 */ extern u32 osVirtualToPhysical(void *);
/* 0x80328050 */ extern OSPri osGetThreadPri(OSThread *);
/* 0x803283E0 */ extern u32 osGetCount(void);
/* 0x80328710 */ extern void __osSetSR(u32);
/* 0x80328720 */ extern u32 __osGetSR(void);
/* 0x80328730 */ extern u32 __osSetFpcCsr(u32);
/* 0x803287E0 */ extern void osMapTLBRdb(void);
/* 0x80328840 */ extern s32 osPiRawReadIo(u32, u32 *);
/* 0x80328A10 */ extern s32 osSetTimer(
    OSTimer *, OSTime, OSTime, OSMesgQueue *, OSMesg
);
/* 0x80328AF0 */ extern s32 osEepromWrite(OSMesgQueue *mq, u8, u8 *);
/* 0x80328FD0 */ extern void osJamMesg(OSMesgQueue *, OSMesg, s32);
/* 0x80329120 */ extern OSMesgQueue *osPiGetCmdQueue(void);
/* 0x80329150 */ extern s32 osEepromRead(OSMesgQueue *, u8, u8 *);
/* 0x80329450 */ extern void guMtxF2L(f32[4][4], Mtx *);
/* 0x80329550 */ extern void guMtxIdentF(f32[4][4]);
/* 0x803295D8 */ extern void guMtxIdent(Mtx *);
/* 0x80329608 */ extern void guMtxL2F(f32[4][4], Mtx *);
/* 0x803296C0 */ extern void guNormalize(f32 *, f32 *, f32 *);
/* 0x80329780 */ extern void __osSetCompare(u32);
/* 0x8032AE10 */ extern OSIntMask osSetIntMask(OSIntMask);
/* 0x8032AE70 */ extern void osDestroyThread(OSThread *);
/* 0x8032B1F0 */ extern u32 __osGetCause(void);

/* 0x8032B260 */ extern u64 rspbootTextStart[];
/* 0x8032B330 */ extern u64 rspbootTextEnd[];
/* 0x8032B330 */ extern u64 gspFast3D_fifoTextStart[];
/* 0x8032C740 */ extern u64 gspFast3D_fifoTextEnd[];
/* 0x8032C740 */ extern u64 aspMainTextStart[];
/* 0x8032D560 */ extern u64 aspMainTextEnd[];

/* 0x80335010 */ extern OSViMode osViModeTable[];
/* 0x80335910 */ extern u64 osClockRate;
/* 0x80335AA0 */ extern OSViMode osViModeNtscLan1;
/* 0x80335AF0 */ extern OSViMode osViModePalLan1;

/* 0x80339AC0 */ extern u64 gspFast3D_fifoDataStart[];
/* 0x8033A2C0 */ extern u64 gspFast3D_fifoDataEnd[];
/* 0x8033A2C0 */ extern u64 aspMainDataStart[];
/* 0x8033A580 */ extern u64 aspMainDataEnd[];

#else /* __ASSEMBLER__ */

/* R4300.h */
#define K0_TO_K1(x)     ((x)|0xA0000000)
#define K1_TO_K0(x)     ((x)&0x9FFFFFFF)
#define K0_TO_PHYS(x)   ((x)&0x1FFFFFFF)
#define K1_TO_PHYS(x)   ((x)&0x1FFFFFFF)
#define KDM_TO_PHYS(x)  ((x)&0x1FFFFFFF)
#define PHYS_TO_K0(x)   ((x)|0x80000000)
#define PHYS_TO_K1(x)   ((x)|0xA0000000)
#define C0_INX          $0
#define C0_RAND         $1
#define C0_ENTRYLO0     $2
#define C0_ENTRYLO1     $3
#define C0_CONTEXT      $4
#define C0_PAGEMASK     $5
#define C0_WIRED        $6
#define C0_BADVADDR     $8
#define C0_COUNT        $9
#define C0_ENTRYHI      $10
#define C0_COMPARE      $11
#define C0_SR           $12
#define C0_CAUSE        $13
#define C0_EPC          $14
#define C0_PRID         $15
#define C0_CONFIG       $16
#define C0_LLADDR       $17
#define C0_WATCHLO      $18
#define C0_WATCHHI      $19
#define C0_ECC          $26
#define C0_CACHE_ERR    $27
#define C0_TAGLO        $28
#define C0_TAGHI        $29
#define C0_ERROR_EPC    $30

#endif /* __ASSEMBLER__ */

#endif /* _ULTRA64_H_ */
