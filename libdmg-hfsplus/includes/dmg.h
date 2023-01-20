#ifndef DMG_H
#define DMG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <abstractfile.h>

#define CHECKSUM_CRC32 0x00000002
#define CHECKSUM_MKBLOCK 0x0002
#define CHECKSUM_NONE 0x0000

#define BLOCK_ZLIB 0x80000005
#define BLOCK_RAW 0x00000001
#define BLOCK_TERMINATOR 0xFFFFFFFF

#define UDIF_BLOCK_SIGNATURE 0x6D697368 // mish
#define KOLY_SIGNATURE 0x6B6F6C79       // koly
#define KOLY_CHECKSUM_SIZE 0x20         // 32
#define KOLY_HEADER_SIZE 0x200          // 512
#define KOLY_RESERVED 0x78              // 120

#define ATTRIBUTE_HDIUTIL 0x0050

enum { kUDIFFlagsFlattened = 1 };

enum { kUDIFDeviceImageType = 1, kUDIFPartitionImageType = 2 };

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t data[KOLY_CHECKSUM_SIZE];
} __attribute__((__packed__)) UDIFChecksum;

typedef struct {
  uint32_t data1; /* smallest */
  uint32_t data2;
  uint32_t data3;
  uint32_t data4; /* largest */
} __attribute__((__packed__)) UDIFID;

typedef struct {
  uint32_t fUDIFSignature;
  uint32_t fUDIFVersion;
  uint32_t fUDIFHeaderSize;
  uint32_t fUDIFFlags;

  uint64_t fUDIFRunningDataForkOffset;
  uint64_t fUDIFDataForkOffset;
  uint64_t fUDIFDataForkLength;
  uint64_t fUDIFRsrcForkOffset;
  uint64_t fUDIFRsrcForkLength;

  uint32_t fUDIFSegmentNumber;
  uint32_t fUDIFSegmentCount;
  UDIFID fUDIFSegmentID; /* a 128-bit number like a GUID, but does not seem to
                            be a OSF GUID, since it doesn't have the proper
                            versioning byte */

  UDIFChecksum fUDIFDataForkChecksum;

  uint64_t fUDIFXMLOffset;
  uint64_t fUDIFXMLLength;

  uint8_t reserved1[KOLY_RESERVED]; /* this is actually the perfect amount of
                              space to store every thing in this struct until
                              the checksum */

  UDIFChecksum fUDIFMasterChecksum;

  uint32_t fUDIFImageVariant;
  uint64_t fUDIFSectorCount;

  uint32_t reserved2;
  uint32_t reserved3;
  uint32_t reserved4;

} __attribute__((__packed__)) UDIFResourceFile;

typedef struct {
  uint32_t type;
  uint32_t reserved;
  uint64_t sectorStart;
  uint64_t sectorCount;
  uint64_t compOffset;
  uint64_t compLength;
} __attribute__((__packed__)) BLKXRun;

typedef struct {
  uint16_t version; /* set to 5 */
  uint32_t isHFS; /* first dword of v53(ImageInfoRec): Set to 1 if it's a HFS or
                     HFS+ partition -- duh. */
  uint32_t unknown1; /* second dword of v53: seems to be garbage if it's HFS+,
                        stuff related to HFS embedded if it's that*/
  uint8_t dataLen; /* length of data that proceeds, comes right before the data
                      in ImageInfoRec. Always set to 0 for HFS, HFS+ */
  uint8_t data[255]; /* other data from v53, dataLen + 1 bytes, the rest NULL
                        filled... a string? Not set for HFS, HFS+ */
  uint32_t unknown2; /* 8 bytes before volumeModified in v53, seems to be always
                        set to 0 for HFS, HFS+  */
  uint32_t unknown3; /* 4 bytes before volumeModified in v53, seems to be always
                        set to 0 for HFS, HFS+ */
  uint32_t volumeModified;  /* offset 272 in v53 */
  uint32_t unknown4;        /* always seems to be 0 for UDIF */
  uint16_t volumeSignature; /* HX in our case */
  uint16_t sizePresent;     /* always set to 1 */
} __attribute__((__packed__)) SizeResource;

typedef struct {
  uint16_t version; /* set to 1 */
  uint32_t type;    /* set to 0x2 for MKBlockChecksum */
  uint32_t checksum;
} __attribute__((__packed__)) CSumResource;

typedef struct NSizResource {
  char isVolume;
  unsigned char *sha1Digest;
  uint32_t blockChecksum2;
  uint32_t bytes;
  uint32_t modifyDate;
  uint32_t partitionNumber;
  uint32_t version;
  uint32_t volumeSignature;
  struct NSizResource *next;
} NSizResource;

typedef struct {
  uint32_t fUDIFBlocksSignature;
  uint32_t infoVersion;
  uint64_t firstSectorNumber;
  uint64_t sectorCount;

  uint64_t dataStart;
  uint32_t decompressBufferRequested;
  uint32_t blocksDescriptor;

  uint32_t reserved1;
  uint32_t reserved2;
  uint32_t reserved3;
  uint32_t reserved4;
  uint32_t reserved5;
  uint32_t reserved6;

  UDIFChecksum checksum;

  uint32_t blocksRunCount;
  BLKXRun runs[0];
} __attribute__((__packed__)) BLKXTable;

typedef struct ResourceData {
  uint32_t attributes;
  unsigned char *data;
  size_t dataLength;
  int id;
  char *name;
  struct ResourceData *next;
} ResourceData;

typedef void (*FlipDataFunc)(unsigned char *data, char out);
typedef void (*ChecksumFunc)(void *ckSum, const unsigned char *data,
                             size_t len);

typedef struct ResourceKey {
  unsigned char *key;
  ResourceData *data;
  struct ResourceKey *next;
  FlipDataFunc flipData;
} ResourceKey;

typedef struct {
  unsigned long state[5];
  unsigned long count[2];
  unsigned char buffer[64];
} SHA1_CTX;

typedef struct {
  uint32_t block;
  uint32_t crc;
  SHA1_CTX sha1;
} ChecksumToken;

static inline uint32_t readUInt32(AbstractFile *file) {
  uint32_t data;

  ASSERT(file->read(file, &data, sizeof(data)) == sizeof(data), "fread");
  FLIPENDIAN(data);

  return data;
}

static inline void writeUInt32(AbstractFile *file, uint32_t data) {
  FLIPENDIAN(data);
  ASSERT(file->write(file, &data, sizeof(data)) == sizeof(data), "fwrite");
}

static inline uint32_t readUInt64(AbstractFile *file) {
  uint64_t data;

  ASSERT(file->read(file, &data, sizeof(data)) == sizeof(data), "fread");
  FLIPENDIAN(data);

  return data;
}

static inline void writeUInt64(AbstractFile *file, uint64_t data) {
  FLIPENDIAN(data);
  ASSERT(file->write(file, &data, sizeof(data)) == sizeof(data), "fwrite");
}

unsigned char *decodeBase64(char *toDecode, size_t *dataLength);
void writeBase64(AbstractFile *file, unsigned char *data, size_t dataLength,
                 int tabLength, int width);
char *convertBase64(unsigned char *data, size_t dataLength, int tabLength,
                    int width);

uint32_t CRC32Checksum(uint32_t *crc, const unsigned char *buf, size_t len);
uint32_t MKBlockChecksum(uint32_t *ckSum, const unsigned char *data,
                         size_t len);

void BlockCRC(void *token, const unsigned char *data, size_t len);
uint32_t calculateMasterChecksum(ResourceKey *resources);
void CRCProxy(void *token, const unsigned char *data, size_t len);

void flipUDIFChecksum(UDIFChecksum *o, char out);
void readUDIFChecksum(AbstractFile *file, UDIFChecksum *o);
void writeUDIFChecksum(AbstractFile *file, UDIFChecksum *o);
void readUDIFID(AbstractFile *file, UDIFID *o);
void writeUDIFID(AbstractFile *file, UDIFID *o);
void readUDIFResourceFile(AbstractFile *file, UDIFResourceFile *o);
void writeUDIFResourceFile(AbstractFile *file, UDIFResourceFile *o);

ResourceKey *readResources(AbstractFile *file, UDIFResourceFile *resourceFile);
void writeResources(AbstractFile *file, ResourceKey *resources);
void releaseResources(ResourceKey *resources);

NSizResource *readNSiz(ResourceKey *resources);
ResourceKey *writeNSiz(NSizResource *nSiz);
void releaseNSiz(NSizResource *nSiz);

ResourceKey *getResourceByKey(ResourceKey *resources, const char *key);
ResourceData *getDataByID(ResourceKey *resource, int id);
ResourceKey *insertData(ResourceKey *resources, const char *key, int id,
                        const char *name, const char *data, size_t dataLength,
                        uint32_t attributes);
ResourceKey *makePlst();

BLKXTable *insertBLKX(AbstractFile *out, AbstractFile *in,
                      ChecksumFunc uncompressedChk, void *uncompressedChkToken,
                      ChecksumFunc compressedChk, void *compressedChkToken);

int convertToDMG(AbstractFile *iso, AbstractFile *dmg);

#endif
