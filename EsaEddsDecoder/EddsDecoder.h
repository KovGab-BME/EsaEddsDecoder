#pragma once


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#include "PgmImage.h"

//#pragma pack(push,1)					// tároljuk az adatokat bájtonként szorosan!
typedef struct tagEDDS_PACKET
{
	BYTE counter;
	int sizePacket;
	BYTE pData[2500];
} EDDS_PACKET_TYPE;
//#pragma pack(pop)						// tároljuk az adatokat optimalizáltan!

typedef struct tagSCOS2000_PACKET_TYPE
{
	DWORD	eddsDataIdx;				// index in EDDS data array

	WORD	PacketID;					//vNumber(3b) +Type(1b)+DataFieldFlag(1b)+APID11b
	WORD	PktSeqCtrl;
	DWORD	PktDataFieldLen;			// data length-1
		WORD	pktApid;

	BYTE	pusVer;
	BYTE	serviceType;
	BYTE	serviceSubType;
	WORD	pktSubCounter;
	WORD	destId;
	BYTE	scTime[7];

	BYTE	logAddr;				// logical address 
	BYTE	protId;					// protocolID
	BYTE	PacketMark;				// packet mark
	BYTE	PacketCount;			// packet count
	WORD	addrY;					// line number

	BYTE    lineData[1020];		    // image line data
} SCOS2000_PACKET_TYPE;


class CEddsDecoder
{
public:
	CEddsDecoder();
	~CEddsDecoder();

	char m_messageTxt[256];

	BYTE	*m_pImagebytes;
	int		m_imageStarted;
	int		m_imageNumber;
	int		m_metaNumber;

	CPgmImage* m_pImageOut;

private:

	char m_pfileNameRaw[256];
	char m_pfileNameBin[256];
	char m_pfileNameSdb[256];
	char m_pImageDir[256];

public:
	int SetFilename(char* pFileName);

	int LoadRawData(FILE* fpRawFile, EDDS_PACKET_TYPE *pEddsPacket);
	int SaveBinaryPacket(FILE* fpBinFile, EDDS_PACKET_TYPE* pEddsPacket);

	int GetHexByte(BYTE* pBytesIn, BYTE* pByteOut, int nBytesIn);
	int ConvertToBinary();

	int DecodeScos2000(EDDS_PACKET_TYPE* pEddsPacket, SCOS2000_PACKET_TYPE* pScosPacket);

	int NewImage();
	int NewImageLine(SCOS2000_PACKET_TYPE* pScosPacket);
};

