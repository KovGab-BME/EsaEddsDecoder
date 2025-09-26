#pragma once


typedef unsigned char BYTE;


typedef struct tagEDDS_PACKET
{
	BYTE counter;
	int sizePacket;
	BYTE pData[2500];
} EDDS_PACKET_TYPE;


class CEddsDecoder
{
public:
	CEddsDecoder();
	~CEddsDecoder();

	char m_messageTxt[256];

private:

	char m_pfileNameRaw[256];
	char m_pfileNameBin[256];


public:
	int SetFilename(char* pFileName);
	int GetHexByte(BYTE* pBytesIn, BYTE* pByteOut, int nBytesIn);
	int ConvertToBinary();

	int LoadRawData(FILE* fpRawFile, EDDS_PACKET_TYPE *pEddsPacket);

};

