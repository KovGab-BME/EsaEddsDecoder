// MOGI objektum orientált programozás mintapélda
// ESA EDDS dekóder

#pragma once
#include "PgmImage.h"

// típusdefiníciók
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

//#pragma pack(push,1)					// tároljuk az adatokat bájtonként szorosan!

// EDDS raw csomag típus
typedef struct tagEDDS_PACKET
{
	BYTE counter;		// csomag számláló
	int sizePacket;		// csomag méret bájtban (hex formátumban kétszeres méretű a tárolás)
	BYTE pData[2500];	// dekódolt csomag adatok (bináris formátumban)
} EDDS_PACKET_TYPE;
//#pragma pack(pop)						// tároljuk az adatokat optimalizáltan!

// ESA EDDS dekóder	osztály
// AFC navigációs képek dekódolása EDDS RAW csomagokból
// A képek mentése PGM 8bites formátumban

// SCOS 2000 csomag típus (a belső EDDS csomag adattartalma)
typedef struct tagSCOS2000_PACKET_TYPE
{
	WORD	PacketID;					// 16 bit adat: vNumber(3b) +Type(1b)+DataFieldFlag(1b)+APID (11b)
	WORD	PktSeqCtrl;		
	DWORD	PktDataFieldLen;			// data length-1
		WORD	pktApid;				// APID a PacketID-ből		AFC: 282

	BYTE	pusVer;
	BYTE	serviceType;				// service type: 139 = AFC kamera	
	BYTE	serviceSubType;				// service subtype: 13 = gudance metadata, 14= guidance image line (8bites kép)
										// service subtype: 20 = science metadata, 15= navcam image line (16bites kép)
	WORD	pktSubCounter;
	WORD	destId;
	BYTE	scTime[7];					// spacecraft time számláló (4 byte sec + 3byte fraction)

	BYTE	logAddr;					// logical address  (AFC kép:65)
	BYTE	protId;						// protocolID		(AFC kép:240)
	BYTE	PacketMark;					// packet mark		( 8 bites kép: 13)
	BYTE	PacketCount;				// packet count		
	WORD	addrY;						// a sor száma (2..1021)

	BYTE    lineData[1020];				// kép sor adatai (8bites kép)
} SCOS2000_PACKET_TYPE;

// EDDS dekóder osztály
class CEddsDecoder
{
public:
	// konstruktor, destruktor
	CEddsDecoder();
	~CEddsDecoder();

	// üzenet szöveg a program állapotáról, hibákról
	char m_messageTxt[256];

	BYTE* m_pImagebytes;		// egy kép sorának bájt adatai
	int		m_imageStarted;		// kép sorok fogadása folyamatban
	int		m_imageNumber;		// a fogadott kép sorszáma
	int		m_metaNumber;		// a fogadott meta adatok sorszáma

	CPgmImage* m_pImageOut;		// a kimenő kép objektum

private:

	char m_pfileNameRaw[256];	// bemenő EDDS fájl neve
	char m_pfileNameBin[256];	// kimenő bináris fájl neve
	char m_pfileNameSdb[256];	// kimenő SDB fájl neve
	char m_pImageDir[256];		// a kimenő képek könyvtára

public:
	// fájlnév beállítása, a fájl létezésének ellenőrzése
	int SetFilename(char* pFileName);

	// EDDS RAW fájl olvasása, egy csomag dekódolása
	int LoadRawData(FILE* fpRawFile, EDDS_PACKET_TYPE *pEddsPacket);

	// EDDS csomag mentése bináris fájlba
	int SaveBinaryPacket(FILE* fpBinFile, EDDS_PACKET_TYPE* pEddsPacket);

	// hex bájt (2 karakter) konvertálása bináris bájttá
	int GetHexByte(BYTE* pBytesIn, BYTE* pByteOut, int nBytesIn);

	// fájl létezésének ellenőrzése, fájl konvertálása bináris formátumba, csomagonként 
	int ConvertToBinary();

	// SCOS 2000 csomag dekódolása az EDDS bináris struktúrából
	int DecodeScos2000(EDDS_PACKET_TYPE* pEddsPacket, SCOS2000_PACKET_TYPE* pScosPacket);

	// új kép kezdése, a kép objektum inicializálása
	int NewImage();
	// új kép sor fogadása, a kép sor adatainak tárolása, teljes kép esetén PGM mentése
	int NewImageLine(SCOS2000_PACKET_TYPE* pScosPacket);
};

