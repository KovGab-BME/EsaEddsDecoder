#pragma once


// PGM image class
class CPgmImage
{
public:
	CPgmImage();
	~CPgmImage();

	int m_width;
	int m_height;
	int m_nPixels;
	unsigned char	*m_pPixels;

	int	Init(int width, int height);
	int Save(const char* pFileName);

};

