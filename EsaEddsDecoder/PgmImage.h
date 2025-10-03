#pragma once

// MOGI objektum orient�lt programoz�s mintap�lda

// PGM image class
// Portable Gray Map (PGM) form�tum� k�p f�jlok l�trehoz�s�ra szolg�l� oszt�ly
// a jelenlegi verzi� csak a P5 (bin�ris) form�tumot kezeli, 256 sz�rke�rnyalattal

class CPgmImage
{
public:
	// konstruktor, destruktor
	CPgmImage();
	~CPgmImage();

	// k�p tulajdon�gai
	int m_width;
	int m_height;
	int m_nPixels;
	// pixel adatok
	unsigned char	*m_pPixels;

	// k�p inicializ�l�sa, pixel mem�ria foglal�s
	int	Init(int width, int height);
	// k�p ment�se PGM form�tumban
	int Save(const char* pFileName);

};

