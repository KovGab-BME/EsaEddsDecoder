

#include <conio.h>
#include <stdlib.h>
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


// CEddsDecoder class methods

CEddsDecoder::CEddsDecoder()
{
	memset(m_pfileNameRaw, 0, 256);
	memset(m_pfileNameBin, 0, 256);
	memset(m_pfileNameSdb, 0, 256);
	memset(m_messageTxt, 0, 256);
	memset(m_pImageDir, 0, 256);

	m_pImagebytes = new BYTE[1020*1020];
	m_imageStarted = 0;

	m_pImageOut = new CPgmImage;
	m_pImageOut->Init(1020, 1020);
	m_imageNumber = 0;
}


CEddsDecoder::~CEddsDecoder()
{
	delete[] m_pImagebytes;
	delete m_pImageOut;
}


int CEddsDecoder::SetFilename(char* pFileName)
{
	int len;
	char dir[256];

	memset(m_pfileNameRaw, 0, 256);
	memset(m_pfileNameBin, 0, 256);
	if (FileExists(pFileName, NULL, NULL) == 0) {
		printf("\nA megadott fájl nem létezik: %s\n", pFileName);
		return -1;
	}
	strncpy_s(m_pfileNameRaw, pFileName, 256);
	strncpy_s(m_pfileNameBin, pFileName, 256);
	strncpy_s(m_pfileNameSdb,  pFileName, 256);
	len = (int)strlen(m_pfileNameBin);
	m_pfileNameBin[len - 3] = 'b';
	m_pfileNameBin[len - 2] = 'i';
	m_pfileNameBin[len - 1] = 'n';
	m_pfileNameSdb[len - 3] = 'S';
	m_pfileNameSdb[len - 2] = 'd';
	m_pfileNameSdb[len - 1] = 'b';

	_splitpath_s(pFileName, m_pImageDir, 256, dir, 256, NULL,0, NULL, 0);
	strcat_s(m_pImageDir, 256, dir);

	return 0;
}


// convert 2 Hex chars to 1 byte
int CEddsDecoder::GetHexByte(BYTE* pBytesIn, BYTE* pByteOut, int nBytesIn)
{
	char hexB[3] = { 0,0,0 };
	int c0 = -1, c1 = -1;
	int valByte, charCounter, idx;

	charCounter = 0;
	idx = 0;
	do {
		memcpy(hexB, pBytesIn + charCounter, 2);
		if ((isalnum(hexB[0]) == 0) || (isalnum(hexB[1]) == 0)) return -111;
		_strupr_s(hexB);
		if ((hexB[0] >= '0') && (hexB[0] <= '9')) c0 = hexB[0] - '0';
		else {
			if ((hexB[0] >= 'A') && (hexB[0] <= 'F')) c0 = hexB[0] - 'A' + 10;
			else c0 = -1;
		}
		if ((hexB[1] >= '0') && (hexB[1] <= '9')) c1 = hexB[1] - '0';
		else {
			if ((hexB[1] >= 'A') && (hexB[1] <= 'F')) c1 = hexB[1] - 'A' + 10;
			else c1 = -1;
		}
		if ((c0 < 0) || (c1 < 0)) return -112;
		pByteOut[idx] = 16 * c0 + c1;
		//if ((valB < 0) || (valB > 255)) return -111;
		charCounter += 2;
		idx++;
	} while (charCounter < nBytesIn);
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
	SCOS2000_PACKET_TYPE scosPacket;
	fpos_t posRaw, posBin;

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
	m_imageNumber = 0;
	m_metaNumber = 0;
	do {
		if (packetCount % 1000 == 0) {
			SetTerminalCursorPos(5, 10);
			printf("Olvasott bájtok: %d, %lld / %lld", packetCount, olvasott, fileLen);
		}
		result = fgetpos(fpIn, &posRaw);
		if ((result==0) && (posRaw >= fileLen)) {
			printf("\nFájl vége!");
			ch0 = _getch();
			ujra = -1;
			break;
		}
		result = LoadRawData(fpIn,   &eddsPacket);
		olvasott += result;
		packetCount++;
		if (result > 0) {
			result = DecodeScos2000(&eddsPacket, &scosPacket);
			if (result == 0) {
				SaveBinaryPacket(fpOut, &eddsPacket);
				posBin = 5 + eddsPacket.sizePacket / 2;
				result = DecodeScos2000(&eddsPacket, &scosPacket);
				if (result != 0) {
					printf("\nSCOS 2000 dekódolási hiba a %d. csomagnál!", packetCount);
					ch0 = _getch();
					ujra = 0;
					break;
				}
			}
			else {
				printf("\nDekódolási hiba a %d. csomagnál!", packetCount);
				ch0 = _getch();
				ujra = 0;
				break;
			}
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

	//// debug ellenörrzés
	//SetTerminalCursorPos(5, 11);
	//printf("Sz�ml�l�: %d ", pEddsPacket->counter);


	return pEddsPacket->sizePacket;
}

// 
int CEddsDecoder::SaveBinaryPacket(FILE* fpBinFile, EDDS_PACKET_TYPE* pEddsPacket)
{
	fwrite(&pEddsPacket->counter, 1, 1, fpBinFile);
	fwrite(&pEddsPacket->sizePacket, 4, 1, fpBinFile);
	fwrite(&pEddsPacket->pData, pEddsPacket->sizePacket / 2, 1, fpBinFile);
	return 0;
}


int CEddsDecoder::DecodeScos2000(EDDS_PACKET_TYPE* pEddsPacket, SCOS2000_PACKET_TYPE* pScosPacket)
{
	WORD dataW;

	memset(pScosPacket, 0, sizeof(SCOS2000_PACKET_TYPE));

	memcpy(&pScosPacket->PacketID, pEddsPacket->pData, 2);
	SwapBytes2(&pScosPacket->PacketID);
	memcpy(&pScosPacket->PktSeqCtrl, pEddsPacket->pData + 2, 2);
	SwapBytes2(&pScosPacket->PktSeqCtrl);
	memcpy(&pScosPacket->PktDataFieldLen, pEddsPacket->pData + 4, 2);
	SwapBytes2(&pScosPacket->PktDataFieldLen);

	memcpy(&pScosPacket->pusVer, pEddsPacket->pData + 6, 1);
	memcpy(&pScosPacket->serviceType, pEddsPacket->pData + 7, 1);
	memcpy(&pScosPacket->serviceSubType, pEddsPacket->pData + 8, 1);
	memcpy(&pScosPacket->pktSubCounter, pEddsPacket->pData + 9, 2);
	SwapBytes2(&pScosPacket->pktSubCounter);
	memcpy(&pScosPacket->destId, pEddsPacket->pData + 11, 2);
	SwapBytes2(&pScosPacket->destId);
	memcpy(&pScosPacket->scTime, pEddsPacket->pData + 13, 7);

	pScosPacket->pktApid = pScosPacket->PacketID & 0x07FF;
	if ((pScosPacket->pktApid == 292) && (pScosPacket->serviceType == 139) && (pScosPacket->serviceSubType == 13)) {	//guidance metadata
		NewImage();
		return 0;
	}
	if ((pScosPacket->pktApid == 292) && (pScosPacket->serviceType == 139) && (pScosPacket->serviceSubType == 14)) { // guidance image line
		SetTerminalCursorPos(5, 10);
		memcpy(&pScosPacket->logAddr, pEddsPacket->pData + 20, 1);
		memcpy(&pScosPacket->protId, pEddsPacket->pData + 21, 1);
		memcpy(&pScosPacket->PacketMark, pEddsPacket->pData + 22, 1);
		memcpy(&pScosPacket->PacketCount, pEddsPacket->pData + 23, 1);
		if ((pScosPacket->logAddr == 65) && (pScosPacket->protId == 240) && (pScosPacket->PacketMark == 13)) {

			memcpy(&pScosPacket->addrY, pEddsPacket->pData + 24, 2);
			SwapBytes2(&pScosPacket->addrY);
			SetTerminalCursorPos(5, 14);
			printf_s("Image line: %d / %d           ", pScosPacket->addrY, m_imageNumber);
			memcpy(&pScosPacket->lineData[0], pEddsPacket->pData + 26, 1020);
			if (m_imageStarted && (pScosPacket->addrY > 1) && (pScosPacket->addrY < 1023)) NewImageLine(pScosPacket);
		}
		return 0;
	}

	return 0;
}


int CEddsDecoder::NewImage()
{
	memset(m_pImagebytes, 0, 1020 * 1020);
	m_imageStarted = 1;

	m_metaNumber++;
	SetTerminalCursorPos(5, 12);
	printf_s("Guidance metadata: %d", m_metaNumber);
	return 0;
}


int CEddsDecoder::NewImageLine(SCOS2000_PACKET_TYPE* pScosPacket)
{
	int dataIdx;
	char pFileName[32], imagePath[256];

	if (m_imageStarted == 0) return -1;
	dataIdx = (pScosPacket->addrY-2) * 1020;
	memcpy(m_pImagebytes+ dataIdx, pScosPacket->lineData, 1020);
	if (pScosPacket->addrY == 1021) {			// image complete
		m_imageStarted = 0;
		sprintf_s(pFileName, "Image_%02d.pgm", m_imageNumber);
		strcpy_s(imagePath, m_pImageDir);
		strcat_s(imagePath, pFileName);
		memcpy(m_pImageOut->m_pPixels, m_pImagebytes, 1020 * 1020);
		m_pImageOut->Save(imagePath);
		m_imageNumber++;
		SetTerminalCursorPos(5, 16);
		printf_s("Kép mentve: %s          ", imagePath);
	}
	return 0;
}

