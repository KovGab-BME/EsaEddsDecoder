// MOGI objektum orientált programozás mintapélda

// ESA EDDS dekóder
// AFC navigációs képek dekódolása EDDS RAW csomagokból
// A képek mentése PGM 8bites formátumban
// rövid leírás a programról: EDDS_Data_Decoding_Procedure.pptx
// bemennő adatok : EDDS fájlok (hex formátumú RAW csomagok) a .zip fájlokban


// EsaEddsDecoder.cpp: main program file

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <locale.h>
// 'CProgramMenu' menü osztály fejléc include elérése. a cpp és h fájlok a MenuTest projektből származnak
#include "..\..\MenuTest\MenuTest\ProgramMenu.h"	// ügyeljen a pontos elérési útra! (a projekt listában is!)
#include "EddsDecoder.h"							// EDDS dekóder osztály fejléc include elérése (az aktuális program könyvtárból)

// globális változók
CProgramMenu mainMenu;			// főmenü
CEddsDecoder eddsDecoder;		// EDDS dekóder osztály 

// fájlnév bekérése konzolról
// 'pFileName' - a fájlnév tárolására szolgáló tömb, 'sizeFileName' - a tömb mérete
int GetFileName(char *pFileName, int sizeFileName)
{
	char pFN[256];
	int result, chw;

	// a konzol törlése, menü kiírása
	system("cls");									
	printf("\n\nTelemetria fájl megadása\n\n");
	printf("Írja be a fájl nevét: ");
	// fájlnév beolvasása konzolról (ProgramMenu.cpp, és .h' szükséges) 
	result = InputStr(pFN, 256);								// max 256 karakteres fájlnév beolvasása
	if (result == 0) strcpy_s(pFN, 256, "c:\\t\\1.dat");		// ha nincs bevitel, akkor legyen egy alapértelmezett fájlnév	
	if (eddsDecoder.SetFilename(pFN) == 0) {					// fájlnév beállítása az EDDS dekóder osztályban
		printf("\nA fájl megadva: %s\n", pFN);
		strncpy_s(pFileName, sizeFileName, pFN, sizeFileName);	// fájlnév másolása a visszatérési paraméterbe
		result = 0;
	}
	else {
		printf("\nHiba a fájl megadásakor!\n");					// hiba a fájlnév megadásakor
		result = -1;
	}
	chw = _getch();												// várakozás egy billentyű lenyomására
	return result;
}

// EDDS fájl konvertálása bináris formátumba, és a képek másik fájlba mentése
void WriteBinary()
{
	int result;

	system("cls");		
	result = eddsDecoder.ConvertToBinary();			// EDDS fájl konvertálása
	result = _getch();								// várakozás egy billentyű lenyomására
}

// program belépési pont
int main()
{
	char pFileName[256] = "";						// fájlnév tároló
	int menuSelected = -1;							// menü választás változó

	setlocale(LC_ALL, "hu-HU");						// magyar helyi beállítások
	system("cls");									// konzol törlése

	// főmenü létrehozása, és a menüpontok hozzáadása 
	mainMenu.AddTitle("Program menü");
	mainMenu.AddMenuItem("Fájlév megadása");
	mainMenu.AddMenuItem("Konvert EDDS fájl");
	mainMenu.AddMenuItem("Kilépés", 0);

	// a menü megjelenítése, és a menüpontok kezelése
	do {
		menuSelected = mainMenu.ShowMenu();
		switch (menuSelected) {
		case 1: 
			GetFileName(pFileName, 256);
			break;
		case 2: 
			WriteBinary();
			break;
		}

	} while (menuSelected != 0);
	system("cls");						// konzol törlése kilépéskor
}
