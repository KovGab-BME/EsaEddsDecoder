// EsaEddsDecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "..\..\MenuTest\MenuTest\ProgramMenu.h"
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <locale.h>


void GetFileName(char *pFileName, int sizeFileName)
{
	char pFN[256];

	system("cls");
	printf("\n\nTelemetria f�jl megad�sa\n\n");

	printf("�rja be a f�jl nev�t: ");
	InputStr(pFN, 256);

	printf("Kov�cs G�bor\n");



	_getch();
}





int main()
{
	CProgramMenu mainMenu;

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
