#ifndef __SC_H__
#define __SC_H__

#define SC_BASE_REG             0x1FFF0000
#define SC_BUFFER_REG           0x1FFE0000

#define SC_STATUS_REG           (SC_BASE_REG+0x00)
#define SC_COMMAND_REG          (SC_BASE_REG+0x00)
#define SC_DATA0_REG            (SC_BASE_REG+0x04)
#define SC_DATA1_REG            (SC_BASE_REG+0x08)
#define SC_IDENTIFIER_REG       (SC_BASE_REG+0x0C)
#define SC_KEY_REG              (SC_BASE_REG+0x10)

#define SC_CMD_BUSY             0x80000000
#define SC_CMD_ERROR            0x40000000
#define SC_IRQ_PENDING          0x20000000

#define SC_CONFIG_GET           'c'
#define SC_CONFIG_SET           'C'
#define SC_SD_OP                'i'
#define SC_SD_SECTOR_SET        'I'
#define SC_SD_READ              's'
#define SC_SD_WRITE             'S'

#define SC_CFG_ROM_WRITE        1
#define SC_CFG_DD_MODE          3
#define SC_CFG_SAVE_TYPE        6

#define SC_SD_DEINIT            0
#define SC_SD_INIT              1
#define SC_SD_GET_STATUS        2
#define SC_SD_GET_INFO          3
#define SC_SD_BYTESWAP_ON       4
#define SC_SD_BYTESWAP_OFF      5

#define SC_DD_MODE_REGS         1
#define SC_DD_MODE_IPL          2

#define SC_IDENTIFIER           0x53437632  /* SCv2 */

#define SC_KEY_RESET            0x00000000
#define SC_KEY_LOCK             0xFFFFFFFF
#define SC_KEY_UNL              0x5F554E4C  /* _UNL */
#define SC_KEY_OCK              0x4F434B5F  /* OCK_ */

extern int __sc_sync(void);

#endif /* __SC_H__ */
