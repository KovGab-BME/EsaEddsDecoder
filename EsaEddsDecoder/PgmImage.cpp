
// MOGI objektum orientált programozás mintapélda

// PGM image class
// Portable Gray Map (PGM) formátumú kép fájlok létrehozására szolgáló osztály
// a jelenlegi verzió csak a P5 (bináris) formátumot kezeli, 256 szürkeárnyalattal


#include "PgmImage.h"
#include <memory.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

// konstruktor, destruktor
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

