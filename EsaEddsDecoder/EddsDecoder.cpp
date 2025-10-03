

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

	int size = sizeof(EDDS_PACKET_TYPE);

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
	int charCounter, idx;

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

// fájl létezésének ellenőrzése, fájl konvertálása bináris formátumba, csomagonként 
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

	if (FileExists(m_pfileNameRaw, &fileLen, NULL) == 0) {					// fájl létezésének ellenőrzése
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
	size = fread(buffer, 1, 2, fpIn);										//	a fájl elejéről 2 karakter olvasása (kezdeti bájt ellenőrzéshez) 00
	if ((buffer[0]!='0') || (buffer[1] != '0')) {
		printf("\nKezdeti bájt bájt nem 0!");
		ch0 = _getch();
		fclose(fpIn);
		return -2;
	}
	olvasott = 2;
	fopen_s(&fpOut, m_pfileNameBin, "wb");									// bináris fájl megnyitása írásra
	m_imageNumber = 0;
	m_metaNumber = 0;
	do {
		if (packetCount % 1000 == 0) {										// státusz kiírása minden 1000. csomagnál
			SetTerminalCursorPos(5, 10);
			printf("Olvasott bájtok: %d, %lld / %lld", packetCount, olvasott, fileLen);
		}
		result = fgetpos(fpIn, &posRaw);
		if ((result == 0) && (posRaw >= fileLen)) {							// fájl vége
			printf("\nFájl vége!");
			ch0 = _getch();
			ujra = -1;
			break;
		}
		result = LoadRawData(fpIn, &eddsPacket);							// egy EDDS csomag olvasása
		olvasott += result;
		packetCount++;
		if (result > 0) {
			result = DecodeScos2000(&eddsPacket, &scosPacket);				// az EDDS csomagból SCOS 2000 csomag dekódolása
			if (result == 0) {
				SaveBinaryPacket(fpOut, &eddsPacket);						// az EDDS csomag mentése bináris fájlba
				posBin = 5 + eddsPacket.sizePacket / 2;
			}
			else {
				printf("\nDekódolási hiba a %d. csomagnál!", packetCount);
				ch0 = _getch();
				ujra = 0;
				break;
			}
		}
		if (result < 0) {
			printf("\n%s", m_messageTxt);									// hiba üzenet kiírása
			ch0 = _getch();
			ujra = 0;
			break;
		}
	} while (ujra>0);

	fclose(fpOut);					// bináris fájl lezárása
	fclose(fpIn);					// bemenő fájl lezárása
	return 0;
}

// EDDS RAW fájl olvasása, egy csomag dekódolása
int CEddsDecoder::LoadRawData(FILE* fpRawFile, EDDS_PACKET_TYPE* pEddsPacket)
{
	int sizeIn;
	BYTE pRaw8[8], pBin4[4];
	BYTE pData[5000];

	// read counter
	sizeIn = fread(pRaw8, 1, 2, fpRawFile);								// 2 hex karakter olvasása a számlálóhoz
	if (sizeIn < 2) {
		strncpy_s(m_messageTxt, "Fájl túl rövid 0!", 255);
		return -1;
	}		
	if (GetHexByte(pRaw8, pBin4, 2) != 0) {								// hex karakterek konvertálása bináris bájttá
		strncpy_s(m_messageTxt, "Hibás hex adat a fájlban!", 255);
		return -2;
	}
	pEddsPacket->counter = pBin4[0];									// a számláló tárolása a struktúrában
	// read sizePacket
	sizeIn = fread(pRaw8, 1, 8, fpRawFile);								// 8 hex karakter olvasása a csomag mérethez
	if (sizeIn < 8) {
		strncpy_s(m_messageTxt, "Fájl túl rövid 1!", 255);
		return -2;
	}
	if (GetHexByte(pRaw8, pBin4, 8) != 0) {								// hex karakterek konvertálása bináris bájtokkánt
		strncpy_s(m_messageTxt, "Hibás hex adat a fájlban!", 255);
		return -2;
	}
	SwapBytes4(pBin4);													// a méret mező átrendezése little-endian formátumra
	memcpy(&pEddsPacket->sizePacket, pBin4, 4);

	sizeIn = fread(pData, 1, pEddsPacket->sizePacket, fpRawFile);		// a csomag adat tartalmának olvasása (hex formátumban)
	if (sizeIn < pEddsPacket->sizePacket) {
		strncpy_s(m_messageTxt, "Fájl túl rövid 3!", 255);
		return -2;
	}
	if (GetHexByte(pData, pEddsPacket->pData, pEddsPacket->sizePacket) != 0) {		// hex karakterek konvertálása bináris bájtokkánt
		strncpy_s(m_messageTxt, "Hibás hex adat a fájlban!", 255);
		return -2;
	}

	//// debug ellenörrzés
	//SetTerminalCursorPos(5, 11);
	//printf("Sz�ml�l�: %d ", pEddsPacket->counter);

	return pEddsPacket->sizePacket;
}

// EDDS RAW csomag struktúra mentése bináris fájlba
int CEddsDecoder::SaveBinaryPacket(FILE* fpBinFile, EDDS_PACKET_TYPE* pEddsPacket)
{
	fwrite(&pEddsPacket->counter, 1, 1, fpBinFile);
	fwrite(&pEddsPacket->sizePacket, 4, 1, fpBinFile);
	fwrite(&pEddsPacket->pData, pEddsPacket->sizePacket / 2, 1, fpBinFile);
	return 0;
}

// SCOS 2000 csomag dekódolása az EDDS bináris struktúrából
int CEddsDecoder::DecodeScos2000(EDDS_PACKET_TYPE* pEddsPacket, SCOS2000_PACKET_TYPE* pScosPacket)
{
	// SCOS 2000 csomag adatok másolása az EDDS csomagból. 
	// Több bájt esetén átrendezés szükséges (big-endian little-endian formátum)
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
	// Az APID a PacketID mező 11 bitje
	pScosPacket->pktApid = pScosPacket->PacketID & 0x07FF;
	// guidance (navigációs) kép adatainak ellenőrzése, feldolgozása
	if ((pScosPacket->pktApid == 292) && (pScosPacket->serviceType == 139) && (pScosPacket->serviceSubType == 13)) {	//guidance metadata
		NewImage();
		return 0;
	}
	if ((pScosPacket->pktApid == 292) && (pScosPacket->serviceType == 139) && (pScosPacket->serviceSubType == 14)) { // guidance kép sor
		memcpy(&pScosPacket->logAddr, pEddsPacket->pData + 20, 1);
		memcpy(&pScosPacket->protId, pEddsPacket->pData + 21, 1);
		memcpy(&pScosPacket->PacketMark, pEddsPacket->pData + 22, 1);
		memcpy(&pScosPacket->PacketCount, pEddsPacket->pData + 23, 1);
		if ((pScosPacket->logAddr == 65) && (pScosPacket->protId == 240) && (pScosPacket->PacketMark == 13)) {
			memcpy(&pScosPacket->addrY, pEddsPacket->pData + 24, 2);
			SwapBytes2(&pScosPacket->addrY);
			SetTerminalCursorPos(5, 14);
			printf_s("Image line: %d / %d           ", pScosPacket->addrY, m_imageNumber);
			memcpy(&pScosPacket->lineData[0], pEddsPacket->pData + 26, 1020);											// 8 bites kép sor mentése
			if (m_imageStarted && (pScosPacket->addrY > 1) && (pScosPacket->addrY < 1023)) NewImageLine(pScosPacket);	// kép sor fogadása
		}
		return 0;
	}
	return 0;
}

// új kép kezdése, a kép objektum inicializálása
int CEddsDecoder::NewImage()
{
	memset(m_pImagebytes, 0, 1020 * 1020);	// kép bájtok törlése, új fekete kép kezdése
	m_imageStarted = 1;						// kép sorok fogadása folyamatban
	m_metaNumber++;							// új meta adat csomag
	SetTerminalCursorPos(5, 12);
	printf_s("Guidance metadata: %d", m_metaNumber);
	return 0;
}

// új kép sor fogadása, a kép sor adatainak tárolása, teljes kép esetén PGM mentése
int CEddsDecoder::NewImageLine(SCOS2000_PACKET_TYPE* pScosPacket)
{
	int dataIdx;
	char pFileName[32], imagePath[256];

	if (m_imageStarted == 0) return -1;								// nincs folyamatban kép fogadás
	dataIdx = (pScosPacket->addrY - 2) * 1020;						// adat mutató a kép sorának megfelelő helyére
	memcpy(m_pImagebytes + dataIdx, pScosPacket->lineData, 1020);	// kép sor adatainak másolása a kép bájt tömbbe
	if (pScosPacket->addrY == 1021) {								// a kép teljes, mentsük ki!
		m_imageStarted = 0;											// kép fogadás befejezve
		sprintf_s(pFileName, "Image_%02d.pgm", m_imageNumber);		// kép fájlnév
		strcpy_s(imagePath, m_pImageDir);							// kép teljes elérési útjának összeállítása
		strcat_s(imagePath, pFileName);
		memcpy(m_pImageOut->m_pPixels, m_pImagebytes, 1020 * 1020); // kép bájtok másolása a PGM kép objektumba
		m_pImageOut->Save(imagePath);								// kép mentése PGM formátumban
		m_imageNumber++;											// következő kép sorszáma
		SetTerminalCursorPos(5, 16);
		printf_s("Kép mentve: %s          ", imagePath);
	}
	return 0;
}

