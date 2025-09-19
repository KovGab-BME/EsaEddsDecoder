// EsaEddsDecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "..\..\MenuTest\MenuTest\ProgramMenu.h"
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <locale.h>
#include "EddsDecoder.h"

CProgramMenu mainMenu;
CEddsDecoder eddsDecoder;



void GetFileName(char *pFileName, int sizeFileName)
{
	char pFN[256];

	system("cls");
	printf("\n\nTelemetria f�jl megad�sa\n\n");

	printf("�rja be a f�jl nev�t: ");
	InputStr(pFN, 256);

	if (eddsDecoder.SetFilename(pFN)==0) {
		printf("\nA f�jl megadva: %s\n", pFN);
		strncpy_s(pFileName, sizeFileName, pFN, sizeFileName);
	}
	else {
		printf("\nHiba a f�jl megad�sakor!\n");
	}

	_getch();
}





int main()
{
	char pFileName[256] = "";
	double dbl;
	int menuSelected = -1;

	setlocale(LC_ALL, "hu-HU");
	system("cls");

	mainMenu.AddTitle("Program men�");

	mainMenu.AddMenuItem("F�jl�v megad�sa");
	mainMenu.AddMenuItem("Konvert EDDS f�jl");
	mainMenu.AddMenuItem("Kil�p�s", 0);

	do {
		menuSelected = mainMenu.ShowMenu();
		switch (menuSelected) {
		case 1: GetFileName(pFileName, 256);
			break;
		case 2: 
			;
			break;
		}

	} while (menuSelected != 0);

	system("cls");


}
