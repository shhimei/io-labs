#ifndef LAB2_DEV_UTILS_H
#define LAB2_DEV_UTILS_H

#include <linux/bio.h>
#include <linux/blk-mq.h>
#include <linux/blkdev.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

#define MEMSIZE 0x19000 // Size of Ram disk in sectors


#define SECTOR_SIZE 512
#define MBR_SIZE SECTOR_SIZE
#define MBR_DISK_SIGNATURE_OFFSET 440
#define MBR_DISK_SIGNATURE_SIZE 4
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16
#define PARTITION_TABLE_SIZE 64
#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE_SIZE 2
#define MBR_SIGNATURE 0xAA55
#define BR_SIZE SECTOR_SIZE
#define BR_SIGNATURE_OFFSET 510
#define BR_SIGNATURE_SIZE 2
#define BR_SIGNATURE MBR_SIGNATURE

typedef struct
{
    unsigned char boot_type; // 0x00 -- inactive, 0x80 -- active
    unsigned char start_head;
    unsigned char start_sec:6;
    unsigned char start_cyl_hi:2;
    unsigned char start_cyl;
    unsigned char part_type;
    unsigned char end_head;
    unsigned char end_sec:6;
    unsigned char end_cyl_hi:2;
    unsigned char end_cyl;
    unsigned long abs_start_sec;
    unsigned long sec_in_part;
} PartEntry;

typedef PartEntry PartTable[4];

#define SEC_PER_HEAD 63
#define HEAD_PER_CYL 255
#define HEAD_SIZE (SEC_PER_HEAD * MDISK_SECTOR_SIZE)
#define CYL_SIZE (SEC_PER_HEAD * HEAD_PER_CYL * MDISK_SECTOR_SIZE)

#define sec4size(s) ((((s) % CYL_SIZE) % HEAD_SIZE) / MDISK_SECTOR_SIZE)
#define head4size(s) (((s) % CYL_SIZE) / HEAD_SIZE)
#define cyl4size(s) ((s) / CYL_SIZE)

//#define sec_num_size(s) ((((s)*1048576*8/64)/8)/8)

#define PRIMARY_PART_SEC_NUM 0x4FFF
#define LOG_PART_SEC_NUM 0x9FFF
#define EXT_PART_SEC_NUM 0x13FFF
#define ALL_SEC_NUM 0x18FFF

static PartTable def_part_table =
        {
                {
                    boot_type: 0x00,
                    start_sec: 0x02,
                    start_head: 0x0,
                    start_cyl: 0x0,
                    part_type: 0x83,
                    end_head: 0x03,
                    end_sec: 0x20,
                    end_cyl: 0x9F,
                    abs_start_sec: 0x01,
                    sec_in_part: PRIMARY_PART_SEC_NUM
                },
                {
                    boot_type: 0x00,
                    start_head: 0x04,
                    start_sec: 0x1,
                    start_cyl: 0x0,
                    part_type: 0x05, // extended partition type
                    end_head: 0x11,
                    end_sec: 0x20,
                    end_cyl: 0x13F,
                    abs_start_sec: 0x5000,
                    sec_in_part: 0x14000
                }
        };

static unsigned int def_log_part_br_abs_start_sector[] = {0x5000, 0x14000};
static const PartTable def_log_part_table[] =
        {
                {
                        {
                                boot_type: 0x00,
                                start_head: 0x4,
                                start_sec: 0x2,
                                start_cyl: 0x0,
                                part_type: 0x83,
                                end_head: 0xA,
                                end_sec: 0x20,
                                end_cyl: 0x13F,
                                abs_start_sec: 0x1,
                                sec_in_part: 0x9FFF
                        },
                        {
                                boot_type: 0x00,
                                start_head: 0xB,
                                start_sec: 0x01,
                                start_cyl: 0x00,
                                part_type: 0x05,
                                end_head: 0x11,
                                end_sec: 0x20,
                                end_cyl: 0x13F,
                                abs_start_sec: 0xA000,
                                sec_in_part: 0xA000
                        }
                },
                {
                        {
                                boot_type: 0x00,
                                start_head: 0xB,
                                start_sec: 0x02,
                                start_cyl: 0x00,
                                part_type: 0x83,
                                end_head: 0x11,
                                end_sec: 0x20,
                                end_cyl: 0x13F,
                                abs_start_sec: 0x1,
                                sec_in_part: 0x9FFF
                        }
                }
        };

static void copy_mbr(u8 *disk) {
    memset(disk, 0x0, MBR_SIZE);
    *(unsigned long *)(disk + MBR_DISK_SIGNATURE_OFFSET) = 0x36E5756D;
    memcpy(disk + PARTITION_TABLE_OFFSET, &def_part_table, PARTITION_TABLE_SIZE);
    *(unsigned short *)(disk + MBR_SIGNATURE_OFFSET) = MBR_SIGNATURE;
}

static void copy_br(u8 *disk, int abs_start_sector,
                    const PartTable *part_table) {
    disk += (abs_start_sector * MDISK_SECTOR_SIZE);
    memset(disk, 0x0, BR_SIZE);
    memcpy(disk + PARTITION_TABLE_OFFSET, part_table, PARTITION_TABLE_SIZE);
    *(unsigned short *)(disk + BR_SIGNATURE_OFFSET) = BR_SIGNATURE;
}

void copy_mbr_n_br(u8 *disk) {
    int i;
    copy_mbr(disk);
    for (i = 0; i < ARRAY_SIZE(def_log_part_table); ++i)
        copy_br(disk, def_log_part_br_abs_start_sector[i], &def_log_part_table[i]);
}

#endif //LAB2_DEV_UTILS_H
