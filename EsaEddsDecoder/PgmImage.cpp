
// MOGI objektum orientált programozás mintapélda

// PGM image class
// Portable Gray Map (PGM) formátumú kép fájlok létrehozására szolgáló osztály
// a jelenlegi verzió csak a P5 (bináris) formátumot kezeli, 256 szürkeárnyalattal





#include "PgmImage.h"
#include <memory.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

CPgmImage::CPgmImage()
{
	m_width = 0;
	m_height = 0;
	m_nPixels = 0;
	m_pPixels = NULL;
}

CPgmImage::~CPgmImage()
{
	if (m_pPixels != NULL) {
		delete[] m_pPixels;
		m_pPixels = NULL;
	}

}

int CPgmImage::Init(int width, int height)
{
	if (m_pPixels != NULL) {
		delete[] m_pPixels;
		m_pPixels = NULL;
	}
	m_width = width;
	m_height = height;
	m_nPixels = width * height;
	if (m_nPixels <= 0) return -1;
	m_pPixels = new unsigned char[m_nPixels];
	memset(m_pPixels, 0, m_nPixels);
	return 0;
}


int CPgmImage::Save(const char* pFileName)
{
	FILE* fpOut;
	char hdr1[4] = { 'P', '5', 13, 10 };
	char hdrMax[5] = { '2', '5', '5',  13, 10 };
	char hdrPixels[64];

	if ((m_nPixels == 0) || (m_pPixels == NULL)) {
		printf("\nNincs kép menthető állapotban!");
		return -1;
	}
	sprintf_s(hdrPixels, 64, "%d %d%c%c", m_width, m_height, 13, 10);
	fopen_s(&fpOut, pFileName, "wb");
	if (fpOut == NULL) {
		printf("\nPGM fájl megnyitasi hiba: %s", pFileName);
		return -2;
	}
	fwrite(hdr1, 1, 4, fpOut);
	fwrite(hdrPixels, 1, strnlen(hdrPixels, 64), fpOut);
	fwrite(hdrMax, 1, 5, fpOut);
	for (int i = 0; i < m_nPixels; i++) {
		fwrite(&m_pPixels[i], 1, 1, fpOut);
	}
	fclose(fpOut);		// fájl lezárása
	return 0;
}
