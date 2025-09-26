

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "EddsDecoder.h"
#include <memory.h>
#include "..\..\MenuTest\MenuTest\ProgramMenu.h"


// swap a words byte order
void SwapBytes2(void* p)
{
	unsigned char ch, * pch;

	pch = (unsigned char*)p;
	ch = pch[0];
	pch[0] = pch[1];
	pch[1] = ch;
}

// swap a 3 byte order
void SwapBytes3(void* p)
{
	unsigned char ch, * pch;

	pch = (unsigned char*)p;
	ch = pch[0];
	pch[0] = pch[2];
	pch[2] = ch;
}


// swap a DWORD byte order
void SwapBytes4(void* p)
{
	unsigned char ch, * pch;

	pch = (unsigned char*)p;
	ch = pch[0];
	pch[0] = pch[3];
	pch[3] = ch;
	ch = pch[1];
	pch[1] = pch[2];
	pch[2] = ch;
}

// swap a WORD64 byte order
void SwapBytes8(void* p)
{
	unsigned char ch, * pch;

	pch = (unsigned char*)p;
	ch = pch[0];
	pch[0] = pch[7];
	pch[7] = ch;
	ch = pch[1];
	pch[1] = pch[6];
	pch[6] = ch;
	ch = pch[2];
	pch[2] = pch[5];
	pch[5] = ch;
	ch = pch[3];
	pch[3] = pch[4];
	pch[4] = ch;
}



CEddsDecoder::CEddsDecoder()
{
	memset(m_pfileNameRaw, 0, 256);
	memset(m_pfileNameBin, 0, 256);
	memset(m_messageTxt, 0, 256);
	
}


CEddsDecoder::~CEddsDecoder()
{
}


int CEddsDecoder::SetFilename(char* pFileName)
{
	int len;

	memset(m_pfileNameRaw, 0, 256);
	memset(m_pfileNameBin, 0, 256);
	if (FileExists(pFileName, NULL, NULL) == 0) {
		printf("\nA megadott fájl nem létezik: %s\n", pFileName);
		return -1;
	}
	strncpy_s(m_pfileNameRaw, pFileName, 256);
	strncpy_s(m_pfileNameBin, pFileName, 256);
	len = (int)strlen(m_pfileNameBin);
	m_pfileNameBin[len - 3] = 'b';
	m_pfileNameBin[len - 2] = 'i';
	m_pfileNameBin[len - 1] = 'n';
	return 0;
}

// convert 2 Hex chars to 1 byte
int CEddsDecoder::GetHexByte(BYTE *pBytesIn, BYTE *pByteOut, int nBytesIn)
{
	return 0;
}



int CEddsDecoder::ConvertToBinary()
{
	FILE *fpIn, *fpOut;
	char ch0;
	BYTE buffer[3], b1;
	int size, ujra = 1, result, packetCount=0;
	long long fileLen, olvasott;
	EDDS_PACKET_TYPE eddsPacket;
	fpos_t pos;

	if (FileExists(m_pfileNameRaw, &fileLen, NULL) == 0) {
		printf("\nA megadott fájl nem létezik: %s\n", m_pfileNameRaw);
		return -1;
	}
	printf("\nA megadott fájl hossza: %lld bájt \n", fileLen);
	ch0 = _getch();


	fopen_s(&fpIn, m_pfileNameRaw, "rb");
	if (fpIn == NULL) {
		printf("\nInput fájl megnyitasi hiba!");
		ch0 = _getch();
		return -2;
	}
	size = fread(buffer, 1, 2, fpIn);
	if ((buffer[0]!='0') || (buffer[1] != '0')) {
		printf("\nKezdeti bájt bájt nem 0!");
		ch0 = _getch();
		fclose(fpIn);
		return -2;
	}
	olvasott = 2;
	fopen_s(&fpOut, m_pfileNameBin, "wb");

	do {
		if (packetCount % 100 == 0) {
			SetTerminalCursorPos(5, 10);
			printf("Olvasott bájtok: %d, %lld / %lld", packetCount, olvasott, fileLen);
		}
		result = fgetpos(fpIn, &pos);
		if ((result==0) && (pos >= fileLen)) {
			printf("\nFájl vége!");
			ch0 = _getch();
			ujra = -1;
			break;
		}
		result = LoadRawData(fpIn,   &eddsPacket);
		olvasott += result;
		packetCount++;
		if (result > 0) {
			//fwrite(&eddsPacket, sizeof(EDDS_PACKET_TYPE), 1, fpOut);
	
		}
		if (result < 0) {
			printf("\n%s", m_messageTxt);
			ch0 = _getch();
			ujra = 0;
			break;
		}
	} while (ujra>0);

	fclose(fpOut);
	fclose(fpIn);
	return 0;
}


int CEddsDecoder::LoadRawData(FILE* fpRawFile, EDDS_PACKET_TYPE* pEddsPacket)
{
	int sizeIn, dataPtr, result;
	BYTE pRaw8[8], pBin4[4];
	BYTE pData[5000];

	// read counter
	sizeIn = fread(pRaw8, 1, 2, fpRawFile);
	if (sizeIn < 2) {
		strncpy_s(m_messageTxt, "Fájl túl rövid 0!", 255);
		return -1;
	}
	if (GetHexByte(pRaw8, pBin4, 2) != 0) {
		strncpy_s(m_messageTxt, "Hibás hex adat a fájlban!", 255);
		return -2;
	}
	pEddsPacket->counter = pBin4[0];
	// read sizePacket
	sizeIn = fread(pRaw8, 1, 8, fpRawFile);
	if (sizeIn < 8) {
		strncpy_s(m_messageTxt, "Fájl túl rövid 1!", 255);
		return -2;
	}
	if (GetHexByte(pRaw8, pBin4, 8) != 0) {
		strncpy_s(m_messageTxt, "Hibás hex adat a fájlban!", 255);
		return -2;
	}
	SwapBytes4(pBin4);
	memcpy(&pEddsPacket->sizePacket, pBin4, 4);
	// read payload
	sizeIn = fread(pData, 1, pEddsPacket->sizePacket, fpRawFile);
	if (sizeIn < pEddsPacket->sizePacket) {
		strncpy_s(m_messageTxt, "Fájl túl rövid 3!", 255);
		return -2;
	}
	if (GetHexByte(pData, pEddsPacket->pData, pEddsPacket->sizePacket) != 0) {
		strncpy_s(m_messageTxt, "Hibás hex adat a fájlban!", 255);
		return -2;
	}
	return pEddsPacket->sizePacket;
}




//
//int CEddsDecoder::LoadFile(const char* pFileName)
//{
//	strncpy_s(m_pfileName, pFileName, 255);
//	return 0;
// }