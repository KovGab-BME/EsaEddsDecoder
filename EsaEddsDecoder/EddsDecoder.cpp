

#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "EddsDecoder.h"
#include <memory.h>
#include "..\..\MenuTest\MenuTest\ProgramMenu.h"


CEddsDecoder::CEddsDecoder()
{
	memset(m_pfileName, 0, 256);
}


CEddsDecoder::~CEddsDecoder()
{
}


int CEddsDecoder::SetFilename(char* pFileName)
{
	if (FileExists(pFileName, NULL, NULL) == 0) {
		printf("\nA megadott fájl nem létezik: %s\n", m_pfileName);
		return -1;
	}
	strncpy_s(m_pfileName, pFileName, 256);



	return 0;
}




int CEddsDecoder::LoadRawData(FILE* fpRawFile,  BYTE* pData, int sizeData)
{
	return 0;
}





//
//int CEddsDecoder::LoadFile(const char* pFileName)
//{
//	strncpy_s(m_pfileName, pFileName, 255);
//	return 0;
// }