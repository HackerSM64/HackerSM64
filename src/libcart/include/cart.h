#ifndef __CART_H__
#define __CART_H__

#ifdef _ULTRA64
#include <ultra64.h>
#else
#include <stdint.h>
typedef uint32_t u32;
#endif

/* Cartrige types */
#define CART_NULL       -1
#define CART_CI         0       /* 64Drive */
#define CART_EDX        1       /* EverDrive-64 X-series */
#define CART_ED         2       /* EverDrive-64 V1, V2, V2.5, V3 and ED64+ */
#define CART_SC         3       /* SummerCart64 */
#define CART_MAX        4

#ifdef __cplusplus
extern "C" {
#endif

/* Size of cartridge SDRAM */
extern u32 cart_size;

/* Cartridge type */
extern int cart_type;

/* Detect cartridge and initialize it */
extern int cart_init(void);
/* Close the cartridge interface */
extern int cart_exit(void);

/* Swap high and low bytes per 16-bit word when reading into SDRAM */
extern char cart_card_byteswap;

/* Initialize card */
extern int cart_card_init(void);
/* Read sectors from card to system RDRAM */
extern int cart_card_rd_dram(void *dram, u32 lba, u32 count);
/* Read sectors from card to cartridge SDRAM */
extern int cart_card_rd_cart(u32 cart, u32 lba, u32 count);
/* Write sectors from system RDRAM to card */
extern int cart_card_wr_dram(const void *dram, u32 lba, u32 count);
/* Write sectors from cartridge SDRAM to card */
extern int cart_card_wr_cart(u32 cart, u32 lba, u32 count);

/* 64Drive functions */
extern int ci_init(void);
extern int ci_exit(void);
extern int ci_card_init(void);
extern int ci_card_byteswap(int flag);
extern int ci_card_rd_dram(void *dram, u32 lba, u32 count);
extern int ci_card_rd_cart(u32 cart, u32 lba, u32 count);
extern int ci_card_wr_dram(const void *dram, u32 lba, u32 count);
extern int ci_card_wr_cart(u32 cart, u32 lba, u32 count);

/* EverDrive-64 X-series functions */
extern int edx_init(void);
extern int edx_exit(void);
extern int edx_card_init(void);
extern int edx_card_byteswap(int flag);
extern int edx_card_rd_dram(void *dram, u32 lba, u32 count);
extern int edx_card_rd_cart(u32 cart, u32 lba, u32 count);
extern int edx_card_wr_dram(const void *dram, u32 lba, u32 count);
extern int edx_card_wr_cart(u32 cart, u32 lba, u32 count);

/* EverDrive-64 functions */
extern int ed_init(void);
extern int ed_exit(void);
extern int ed_card_init(void);
extern int ed_card_byteswap(int flag);
extern int ed_card_rd_dram(void *dram, u32 lba, u32 count);
extern int ed_card_rd_cart(u32 cart, u32 lba, u32 count);
extern int ed_card_wr_dram(const void *dram, u32 lba, u32 count);
extern int ed_card_wr_cart(u32 cart, u32 lba, u32 count);

/* SummerCart64 functions */
extern int sc_init(void);
extern int sc_exit(void);
extern int sc_card_init(void);
extern int sc_card_byteswap(int flag);
extern int sc_card_rd_dram(void *dram, u32 lba, u32 count);
extern int sc_card_rd_cart(u32 cart, u32 lba, u32 count);
extern int sc_card_wr_dram(const void *dram, u32 lba, u32 count);
extern int sc_card_wr_cart(u32 cart, u32 lba, u32 count);

#ifdef __cplusplus
}
#endif

#endif /* __CART_H__ */
