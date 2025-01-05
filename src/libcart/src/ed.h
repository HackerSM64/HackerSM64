#ifndef __ED_H__
#define __ED_H__

#define ED_BASE_REG             0x08040000

#define ED_CFG_REG              (ED_BASE_REG+0x00)
#define ED_STATUS_REG           (ED_BASE_REG+0x04)
#define ED_DMA_LEN_REG          (ED_BASE_REG+0x08)
#define ED_DMA_ADDR_REG         (ED_BASE_REG+0x0C)
#define ED_MSG_REG              (ED_BASE_REG+0x10)
#define ED_DMA_CFG_REG          (ED_BASE_REG+0x14)
#define ED_SPI_REG              (ED_BASE_REG+0x18)
#define ED_SPI_CFG_REG          (ED_BASE_REG+0x1C)
#define ED_KEY_REG              (ED_BASE_REG+0x20)
#define ED_SAV_CFG_REG          (ED_BASE_REG+0x24)
#define ED_SEC_REG              (ED_BASE_REG+0x28)
#define ED_VER_REG              (ED_BASE_REG+0x2C)

#define ED_CFG_CNT_REG          (ED_BASE_REG+0x40)
#define ED_CFG_DAT_REG          (ED_BASE_REG+0x44)
#define ED_MAX_MSG_REG          (ED_BASE_REG+0x48)
#define ED_CRC_REG              (ED_BASE_REG+0x4C)

#define ED_DMA_SD_TO_RAM        1
#define ED_DMA_RAM_TO_SD        2
#define ED_DMA_FIFO_TO_RAM      3
#define ED_DMA_RAM_TO_FIFO      4

#define ED_CFG_SDRAM_OFF        (0 << 0)
#define ED_CFG_SDRAM_ON         (1 << 0)
#define ED_CFG_BYTESWAP         (1 << 1)

#define ED_STATE_DMA_BUSY       (1 << 0)
#define ED_STATE_DMA_TOUT       (1 << 1)
#define ED_STATE_TXE            (1 << 2)
#define ED_STATE_RXF            (1 << 3)
#define ED_STATE_SPI            (1 << 4)

#define ED_SPI_SPD_50           (0 << 0)
#define ED_SPI_SPD_25           (1 << 0)
#define ED_SPI_SPD_LO           (2 << 0)
#define ED_SPI_SS               (1 << 2)
#define ED_SPI_WR               (0 << 3)
#define ED_SPI_RD               (1 << 3)
#define ED_SPI_CMD              (0 << 4)
#define ED_SPI_DAT              (1 << 4)
#define ED_SPI_8BIT             (0 << 5)
#define ED_SPI_1BIT             (1 << 5)

#define ED_SAV_EEP_ON           (1 << 0)
#define ED_SAV_SRM_ON           (1 << 1)
#define ED_SAV_EEP_SIZE         (1 << 2)
#define ED_SAV_SRM_SIZE         (1 << 3)

#define ED_KEY                  0x1234

#define ED_SD_CMD_RD            (ED_SPI_CMD|ED_SPI_RD)
#define ED_SD_CMD_WR            (ED_SPI_CMD|ED_SPI_WR)
#define ED_SD_DAT_RD            (ED_SPI_DAT|ED_SPI_RD)
#define ED_SD_DAT_WR            (ED_SPI_DAT|ED_SPI_WR)

#define ED_SD_CMD_8b            ED_SPI_8BIT
#define ED_SD_CMD_1b            ED_SPI_1BIT
#define ED_SD_DAT_8b            ED_SPI_8BIT
#define ED_SD_DAT_1b            ED_SPI_1BIT

#define __ed_sd_mode(reg, val)  __cart_wr(ED_SPI_CFG_REG, __sd_cfg|(reg)|(val))
#define __ed_sd_cmd_rd(val)     __ed_spi((val) & 0xFF)
#define __ed_sd_cmd_wr(val)     __ed_spi((val) & 0xFF)
#define __ed_sd_dat_rd()        __ed_spi(0xFF)
#define __ed_sd_dat_wr(val)     __ed_spi((val) & 0xFF)

extern int __ed_spi(int val);
extern int __ed_sd_cmd(int cmd, u32 arg);
extern int __ed_sd_close(int flag);

#endif /* __ED_H__ */
