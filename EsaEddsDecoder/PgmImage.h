#pragma once

// MOGI objektum orientált programozás mintapélda

// PGM image class
// Portable Gray Map (PGM) formátumú kép fájlok létrehozására szolgáló osztály
// a jelenlegi verzió csak a P5 (bináris) formátumot kezeli, 256 szürkeárnyalattal

class CPgmImage
{
public:
	// konstruktor, destruktor
	CPgmImage();
	~CPgmImage();

	// kép tulajdonágai
	int m_width;
	int m_height;
	int m_nPixels;
	// pixel adatok
	unsigned char	*m_pPixels;

	// kép inicializálása, pixel memória foglalás
	int	Init(int width, int height);
	// kép mentése PGM formátumban
	int Save(const char* pFileName);

};

