#ifndef HAS_INCLUDED_LOCATIONS_HEADER
#define HAS_INCLUDED_LOCATIONS_HEADER

  //Buffer size for strings (so they are all treated the exact same way)
  #define MAX_BUFFER_SIZE 4096


  //Sector information
  #define SECTOR_SIZE 512 //In bytes
  #define SECTOR_BOOT 0
  #define SECTOR_FIRST 1
  #define SECTOR_SECOND 2
  #define SECTOR_ROOT_FIRST 19
  #define SECTOR_ROOT_LAST 32
  #define SECTOR_DATA_BEGIN 33
  #define SECTOR_RESERVED_SIZE 2


  //Boot sector information
  //Defined as <NAME>_POS
  //        && <NAME>_SIZE
  #define NUMBER_FAT_POS 16
  #define NUMBER_FAT_SIZE 1
  #define SECTOR_PER_FAT_POS 22
  #define SECTOR_PER_FAT_SIZE 2
  #define LABEL_POS 43
  #define LABEL_SIZE 11


  //File Allocation Table
  #define FAT_UNUSED 0x00
  #define FAT_RESERVEDFIRST 0xFF0
  #define FAT_RESERVEDLAST 0xFF6
  #define FAT_BAD_CLUSTER 0xFF7
  #define FAT_LAST_CLUSTER_START 0xFF8
  #define FAT_LAST_CLUSTER_END 0xFFF
  #define FAT_FIRST_POS 512
  #define FAT_RESERVED_CNT 2
  //Note: Anything else is the number of the next cluster in the file


  //Directory Attributes
  #define DIR_READ_ONLY 0x01
  #define DIR_HIDDEN 0x02
  #define DIR_SYSTEM 0x04
  #define DIR_VOLUME_LABEL 0x08
  #define DIR_SUBDIRECTORY 0x10
  #define DIR_ARCHIVE 0x20
  #define DIR_LONG_FILENAME 0x0F
  #define DIR_EMPTY 0xE5
  #define DIR_FREE 0x00
  #define DIR_OFFSET_SIZE 32

  //Directory properties
  #define DIR_FILENAME_POS 0
  #define DIR_FILENAME_SIZE 8
  #define DIR_EXTENSION_POS 8
  #define DIR_EXTENSION_SIZE 3
  #define DIR_ATTRIBUTES_POS 11
  #define DIR_ATTRIBUTES_SIZE 1
  #define DIR_CREATION_TIME_POS 14
  #define DIR_CREATION_TIME_SIZE 2
  #define DIR_CREATION_DATE_POS 16
  #define DIR_CREATION_DATE_SIZE 2
  #define DIR_FIRST_LOG_CLUS_POS 26
  #define DIR_FIRST_LOG_CLUS_SIZE 2
  #define DIR_FILE_SIZE_POS 28
  #define DIR_FILE_SIZE_SIZE 4

  //# of sectors inside a cluster (always 1 in FAT-12)
  #define SECTORS_PER_CLUSTER 1


//End include guard
#endif

//EOF