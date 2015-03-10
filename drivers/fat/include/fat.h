#ifndef _fat_h_
#define _fat_h_
#include <stdint.h>

typedef uint8_t checksum_t;

typedef enum
{
  free_cluster = 0,
  invalid_cluster_id = 1,
  normal_cluster = 2,
  bad_cluster = 3,
  last_cluster = 4
}fat_cluster_t __attribute__ ((__packed__));

typedef enum
{
  fat12 = 12,
  fat16 = 16,
  fat32 = 32
}fat_t;

typedef enum
{
  write_protected_file 	= 1,
  hidden_file 		= 2,
  system_file 		= 4,
  volume_lable		= 8,
  vfat_entry		= 15,
  special_entry		= 15,
  directory		= 16,
  archive		= 32
}entry_attrib_t __attribute__ ((__packed__));

typedef struct
{
  uint8_t jmp[3]; 			//instruction to jump to the bootloader
  uint8_t oem[8]; 			//namestring of the software which created the filesystem
  uint16_t sector_size; 		//size of one secor. Valid values: 512,1024,2048,4096
  uint8_t sectors_per_cluster; 		//number of sectors per cluster. Valid values: 1-64
  uint16_t reserved_sectors; 		//number of reserved sectors including boot secotr
  uint8_t num_of_fats; 			//number of redundant file Allocation tables. Normal value: 2
  uint16_t max_root_dir_entries; 	//maximum number of root directory entries. Only valid for FAT12/16
  uint16_t num_of_sectors; 		// number of sectors of the partition. If 0 then the value is in num_of_sectors32 field
  uint8_t media_type; 			//if the block device is a floppy drive, this valie indicates die floppy disk type. This value is outdated.
  uint16_t fat_size; 			//size of a fat in sectors; If value = 0, the filesystem is FAT32 an this value is given in the fat32 fat_size
  uint16_t sectors_per_track; 		//numbers of sector per sectors per track
  uint16_t num_of_heads; 		//number of reading and wirting heads
  uint32_t sectors_before_bpb; 		//sectos from begin of the drive to the bootsector of this partition
  uint32_t num_of_sectors32; 		//second num of sectors field. Value = when the first one is used.
}bpb __attribute__ ((__packed__));

typedef struct
{
  uint8_t bios_drive; 			//bios drive number
  uint8_t reserved;
  uint8_t ext_boot_sig; 		//extendet boot signature. Indicates the following fields. Value: 0x29
  uint32_t fs_id;      			//filesystem id
  uint8_t fs_name[11]; 			//filesystem name padded with spaces
  uint8_t fat_ver[8];  			//FAT version padded with spaces valid is: "FAT12   ","FAT16   ","FAT32   " (field is not reliable)
  uint8_t boot_code[448];		//bootloader code
  uint16_t boot_sig;			//boot signature for the bios. Value: 0xAA55
}ebpb12 __attribute__ ((__packed__));

typedef ebpb12 ebpb16;

struct
{
  uint32_t fat_size; 			//size of a fat in sectors
  uint16_t fat_flags; 			// bit 1-3: number of the activ FAT; bit7: 0 = fat mirrioring 1 = bit 1-3 select activ fat
  uint16_t fat32_version; 		// FAT32 version. Normal Value: 0
  uint32_t first_root_dir_cluster; 	//cluster number of the first root directory cluster
  uint16_t info_sector_num; 		//sector number of the filesystem info sector
  uint16_t bootsector_backup; 		//sector with a bootsector copy. Microsoft recommends sector 6
  uint8_t reserved[12];
  uint8_t bios_drive; 			//bios drive number
  uint8_t reserved2;
  uint8_t ext_boot_sig; 		//extendet boot signature. Indicates the following fields. Value: 0x29
  uint32_t fs_id;      			//filesystem id
  uint8_t fs_name[11]; 			//filesystem name padded with spaces
  uint8_t fat_ver[8];  			//FAT version padded with spaces valid is only:"FAT32   "
  uint8_t boot_code[420];		//bootloader code
  uint16_t boot_sig; 			//boot signature for the bios. Value: 0xAA55
}ebpb32 __attribute__ ((__packed__));



typedef struct
{
  uint8_t  num; // Bit 0-5: number of the special entry Bit 6: 1 = first erntry
  uint16_t name1[5]; //first 5 unicode characters of the entry name
  entry_attrib_t file_attributes; // must be special_entry (0xF)
  uint8_t zero; //entry is zero
  checksum_t checksum; //checksum of the short filename
  uint16_t name2[6]; //next 6 unicode name characters
  uint16_t zero2; // a second zero field
  uint16_t name3[2]; //next two characters of the entry name
}spec_entry __attribute__ ((__packed__));

typedef struct
{
  char name[8]; //filename without extension, padded with spaces
  char fne[3]; //filename extension padded with spaces
  entry_attrib_t file_attributes;
  uint8_t reserved;
  uint8_t creation_date[5]; //date of the creation of the entry
  uint16_t last_aacess; // date of the last file access
  uint16_t upper_first_cluster; //upper 16 bit of the first cluster of the file
  uint32_t last_change; //date of the last change;
  uint16_t lower_first_cluster; //lower 16 bit of the first cluster of the file
  uint32_t file_size; //file size; directory filesize = 0
}dir_entry __attribute__ ((__packed__));

fat_t get_fat_type(void *bootsector);
fat_cluster_t check_fat_entry(fat_t version, uint32_t entry);
void *get_fat(fat_t version, void *bootsector);
uint32_t get_fat_entry(fat_t version, void *fat);
void *get_root_dir(fat_t version, void *bootsector);
void *get_dir_entry(fat_t version, void *dir);
int is_entry_special(fat_t version, void *entry);


#endif