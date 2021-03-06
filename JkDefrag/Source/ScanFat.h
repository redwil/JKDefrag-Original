#ifndef __SCANFAT_H__
#define __SCANFAT_H__

#pragma pack(push,1)                  /* Align to bytes. */

struct FatBootSectorStruct
{
	UCHAR  BS_jmpBoot[3];          // 0
	UCHAR  BS_OEMName[8];          // 3
	USHORT BPB_BytsPerSec;         // 11
	UCHAR  BPB_SecPerClus;         // 13
	USHORT BPB_RsvdSecCnt;         // 14
	UCHAR  BPB_NumFATs;            // 16
	USHORT BPB_RootEntCnt;         // 17
	USHORT BPB_TotSec16;           // 19
	UCHAR  BPB_Media;              // 21
	USHORT BPB_FATSz16;            // 22
	USHORT BPB_SecPerTrk;          // 24
	USHORT BPB_NumHeads;           // 26
	ULONG  BPB_HiddSec;            // 28
	ULONG  BPB_TotSec32;           // 32

	union
	{
		struct
		{
			UCHAR  BS_DrvNum;          // 36
			UCHAR  BS_Reserved1;       // 37
			UCHAR  BS_BootSig;         // 38
			ULONG  BS_VolID;           // 39
			UCHAR  BS_VolLab[11];      // 43
			UCHAR  BS_FilSysType[8];   // 54
			UCHAR  BS_Reserved2[448];  // 62
		} Fat16;

		struct
		{
			ULONG  BPB_FATSz32;        // 36
			USHORT BPB_ExtFlags;       // 40
			USHORT BPB_FSVer;          // 42
			ULONG  BPB_RootClus;       // 44
			USHORT BPB_FSInfo;         // 48
			USHORT BPB_BkBootSec;      // 50
			UCHAR  BPB_Reserved[12];   // 52
			UCHAR  BS_DrvNum;          // 64
			UCHAR  BS_Reserved1;       // 65
			UCHAR  BS_BootSig;         // 66
			ULONG  BS_VolID;           // 67
			UCHAR  BS_VolLab[11];      // 71
			UCHAR  BS_FilSysType[8];   // 82
			UCHAR  BPB_Reserved2[420]; // 90
		} Fat32;
	};

	USHORT Signature;              // 510
};

struct FatDirStruct
{
	UCHAR  DIR_Name[11];           // 0   File name, 8 + 3.
	UCHAR  DIR_Attr;               // 11  File attributes.
	UCHAR  DIR_NTRes;              // 12  Reserved.
	UCHAR  DIR_CrtTimeTenth;       // 13  Creation time, tenths of a second, 0...199.
	USHORT DIR_CrtTime;            // 14  Creation time.
	USHORT DIR_CrtDate;            // 16  Creation date.
	USHORT DIR_LstAccDate;         // 18  Last access date.
	USHORT DIR_FstClusHI;          // 20  First cluster number, high word.
	USHORT DIR_WrtTime;            // 22  Last write time.
	USHORT DIR_WrtDate;            // 24  Last write date.
	USHORT DIR_FstClusLO;          // 26  First cluster number, low word.
	ULONG  DIR_FileSize;           // 28  File size in bytes.
};

struct FatLongNameDirStruct
{
	UCHAR LDIR_Ord;                // 0   Sequence number
	WCHAR LDIR_Name1[5];           // 1   Characters 1-5 in name
	UCHAR LDIR_Attr;               // 11  Attribute, must be ATTR_LONG_NAME
	UCHAR LDIR_Type;               // 12  Always zero
	UCHAR LDIR_Chksum;             // 13  Checksum
	WCHAR LDIR_Name2[6];           // 14  Characters 6-11
	UCHAR LDIR_FstClusLO[2];       // 26  Always zero
	WCHAR LDIR_Name3[2];           // 28  Characters 12-13
};

#pragma pack(pop)                     /* Reset byte alignment. */

/* The attribute flags. */
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

#define ATTR_LONG_NAME  (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)
#define ATTR_LONG_NAME_MASK (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE)

/* Struct used by the scanner to store disk information from the bootblock. */
struct FatDiskInfoStruct
{
	ULONG64 BytesPerSector;
	ULONG64 SectorsPerCluster;
	ULONG64 TotalSectors;
	ULONG64 RootDirSectors;
	ULONG64 FirstDataSector;
	ULONG64 FATSz;
	ULONG64 DataSec;
	ULONG64 CountofClusters;

	union
	{
		BYTE *FAT12;
		USHORT *FAT16;
		ULONG *FAT32;
	} FatData;
};

class JKScanFat
{
public:
	JKScanFat();
	~JKScanFat();

	// Get instance of the class
	static JKScanFat *getInstance();

	BOOL AnalyzeFatVolume(struct DefragDataStruct *Data);

private:

	UCHAR CalculateShortNameCheckSum(UCHAR *Name);
	ULONG64 ConvertTime(USHORT Date, USHORT Time, USHORT Time10);
	void MakeFragmentList(struct DefragDataStruct *Data, struct FatDiskInfoStruct *DiskInfo, struct ItemStruct *Item, ULONG64 Cluster);
	BYTE *LoadDirectory(struct DefragDataStruct *Data, struct FatDiskInfoStruct *DiskInfo, ULONG64 StartCluster, ULONG64 *OutLength);
	void AnalyzeFatDirectory(struct DefragDataStruct *Data, struct FatDiskInfoStruct *DiskInfo, BYTE *Buffer, ULONG64 Length, struct ItemStruct *ParentDirectory);

	// static member that is an instance of itself
	static JKScanFat *m_jkScanFat;

	JKDefragLib *m_jkLib;
};

#endif