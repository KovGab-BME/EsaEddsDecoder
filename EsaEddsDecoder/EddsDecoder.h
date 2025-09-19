#pragma once


typedef unsigned char BYTE;


class CEddsDecoder
{
public:
	CEddsDecoder();
	~CEddsDecoder();




private:

	char m_pfileName[256];


public:
	int SetFilename(char* pFileName);
	int LoadRawData(FILE* fpRawFile, BYTE* pData, int sizeData);

};

