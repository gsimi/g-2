#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "DeviceManager.h"
#include "Event.h"
#include "InfoAcq.h"

using namespace std;

int main(int argc, char** argv) {

	cout << "PS5244B program\n";
	cout << "Made by Carlo Poggi\n";

	cout << "c++ version found:\t";
	if( __cplusplus == 201103L ) cout << "C++11\n";
	else if( __cplusplus == 199711L ) {
		cout << "C++98\n";
		cout << "c++11 would be better\n";
	}
	else {
		cout << "pre-standard C++\n" ;
		cout << "c++11 would be better\n";
	}

	/*GESTIONE DELL'OSCILLOSCOPIO*/
//	cout << "Connect the device and press any key\n";
//	getchar();
	char* config_file=(char*)"config.txt";
	if (argc>1) config_file=argv[1];
	DeviceManager* manager = new DeviceManager(config_file); // nel costruttore sono incluse le funzioni per aprire il device e inizializzare i canali

	manager->FillStructsInfoAcq();
	manager->PrintInfo();

	/*TODO
	 * interfacciare l'acquisizione e creare i tree
	 */

	manager->Run();

	manager->~DeviceManager();

	cout << "Fine\n";

	return 0;
}
