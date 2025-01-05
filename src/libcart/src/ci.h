#ifndef __CI_H__
#define __CI_H__

#define CI_BASE_REG             0x18000000

#define CI_BUFFER_REG           (CI_BASE_REG+0x0000)
#define CI_SDRAM_ADDR_REG       (CI_BASE_REG+0x0004)

#define CI_STATUS_REG           (CI_BASE_REG+0x0200)
#define CI_COMMAND_REG          (CI_BASE_REG+0x0208)
#define CI_LBA_REG              (CI_BASE_REG+0x0210)
#define CI_LENGTH_REG           (CI_BASE_REG+0x0218)
#define CI_RESULT_REG           (CI_BASE_REG+0x0220)

#define CI_MAGIC_REG            (CI_BASE_REG+0x02EC)
#define CI_VARIANT_REG          (CI_BASE_REG+0x02F0)
#define CI_REVISION_REG         (CI_BASE_REG+0x02FC)

#define CI_STATUS_MASK          0xF000
#define CI_IDLE                 0x0000
#define CI_BUSY                 0x1000

#define CI_RD_BUFFER            0x01
#define CI_RD_SDRAM             0x03
#define CI_WR_BUFFER            0x10
#define CI_WR_SDRAM             0x13
#define CI_SD_RESET             0x1F
#define CI_BYTESWAP_OFF         0xE0
#define CI_BYTESWAP_ON          0xE1
#define CI_CARTROM_WR_ON        0xF0
#define CI_CARTROM_WR_OFF       0xF1
#define CI_EXT_ADDR_ON          0xF8
#define CI_EXT_ADDR_OFF         0xF9
#define CI_ABORT                0xFF

#define CI_MAGIC                0x55444556  /* UDEV */

#define CI_VARIANT_HW1          0x4100  /* A */
#define CI_VARIANT_HW2          0x4200  /* B */

extern int __ci_sync(void);

#endif /* __CI_H__ */
