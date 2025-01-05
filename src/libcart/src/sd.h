#ifndef __SD_H__
#define __SD_H__

#define CMD0    (0x40| 0)
#define CMD1    (0x40| 1)
#define CMD2    (0x40| 2)
#define CMD3    (0x40| 3)
#define CMD7    (0x40| 7)
#define CMD8    (0x40| 8)
#define CMD9    (0x40| 9)
#define CMD12   (0x40|12)
#define CMD18   (0x40|18)
#define CMD25   (0x40|25)
#define CMD55   (0x40|55)
#define CMD58   (0x40|58)
#define ACMD6   (0x40| 6)
#define ACMD41  (0x40|41)

extern unsigned char __sd_resp[17];
extern unsigned char __sd_cfg;
extern unsigned char __sd_type;
extern unsigned char __sd_flag;

extern int __sd_crc7(const char *src);
extern void __sd_crc16(u64 *dst, const u64 *src);

#endif /* __SD_H__ */
