


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

