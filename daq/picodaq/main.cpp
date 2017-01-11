/*
 *	main.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: carlo
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <chrono>
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

#define NO_SEGMENTS 1024 // Numero di segmenti
#define NO_WAVEFORMS 100 // Numero di forme d'onda acquisite

BOOL g_ready = FALSE;  // Global flag

struct acq_info {
	int waveformPerBlock;
	unsigned long elapsedTime;
	unsigned long waveformStored;
};

struct sampling_info {
	int32_t nSamplesDIV;
	int32_t samplesStored;
	int32_t preTrig;
	float timeInterval;
};

struct event {
	int32_t samplesStored;
	int64_t time;
	short timeUnit;
	float* sample;
};


/****************************************************************************
* Callback
* used by PS5000A data block collection calls, on receipt of data.
* used to set global flags etc checked by user routines
****************************************************************************/
void PREF4 callBackBlock( int16_t handle, PICO_STATUS status, void * pParameter)
{
	if (status != PICO_CANCELLED)
		g_ready = TRUE;
}

/****************************************************************************
* MAIN
****************************************************************************/
int main() {

	if( __cplusplus == 201103L ) cout << "C++11\n" ;
	else if( __cplusplus == 199711L ) cout << "C++98\n" ;
	else cout << "pre-standard C++\n" ;

	PICO_STATUS status = PICO_OK;
	UNIT* unit = new UNIT[1];

	printf("PS5244B program\n");
	printf("Made by Carlo Poggi\n");
	printf("\nConnect the device and press any key\n");
	getchar();
	
	char ch = '.';

	// Apri il dispositivo

	do {

		printf("Opening...\n");
		status = openDevice(unit, NULL);
		if (status == PICO_OK || status == PICO_POWER_SUPPLY_NOT_CONNECTED)
		{
			printf("\nDEVICE FOUND!\n");
			unit->openStatus = (int16_t) status;
			set_info(unit);
			break;
		}
		else {
			printf("Device not found!\n");
			printf("Plug the device and press any key or press X to exit\n");
			ch = toupper(getchar());
		}

	} while (ch != 'X');

	if (ch == 'X') {
		printf("\nBye!\n");
		return 0;
	}

	if (unit->openStatus == PICO_POWER_SUPPLY_NOT_CONNECTED)
	{
		unit->openStatus = (int16_t) changePowerSource(unit->handle, PICO_POWER_SUPPLY_NOT_CONNECTED, unit);
	}

	// Imposta i canali da utilizzare
	// Chiama la funzione per settare i canali
	printf("\nSetting channels...\n");
	status = setChannels(unit);

	// Definisco numero waveform da registrare (uso la modalità rapid block mode per avere deadtime ridotto, vedi programmer guide)
	int32_t maxSamples; // Numero massimo di samples per questa timebase

	status = ps5000aMemorySegments(unit->handle,NO_SEGMENTS,&maxSamples); // Definisco il numero di segmenti di memoria
	status = ps5000aSetNoOfCaptures(unit->handle, NO_WAVEFORMS); // Definisco il numero di waveform da registrare

	printf("\nNumber of segments:	%i\n",NO_SEGMENTS);
	printf("Number of waveforms:	%i\n",NO_WAVEFORMS);
	printf("Max samples per div:	%i\n",maxSamples);

	// Imposta la timebase
	uint32_t nSamplesDIV = 100; // Numero di samples in una divisione richiesti
	uint32_t nSamplesTOT = nSamplesDIV*10; // Numero di samples totali

	int32_t preTrigger = nSamplesDIV*2;
	int32_t postTrigger = nSamplesDIV*5;
	uint32_t timebase = 127; // Dovrebbe corrispondere a (timebase-2)/1.25e8 s per divisione (se timebase=127, corrisponde a 1 microsec/div)

	printf("\nSetting sampling properties...\n");
	printf("\nDEFAULT:\n");
	printf("Timebase:					%i\n",timebase);
	printf("Time per DIV (ns):			%f\n",(timebase-2)*1./0.125);

	printf("\nIs it ok? (Y/N)\n");
	ch = YN_check();

	if (ch == 'Y') {
		// Continua senza modificare...
	}
	else if (ch == 'N') {
		printf("Time per DIV (ns) = (timebase-2)/0.125\n");
		printf("Insert timebase:\n");
		cin >> timebase;
		printf("Got %i\n",timebase);
		printf("Time per DIV (ns):			%f\n",(timebase-2)*1./0.125);
	}
	else {
		printf("Closing the device...\n");
		closeDevice(unit);
		printf("Bye!\n");
		return 0;
	}

	printf("\nDEFAULT:\n");
	printf("Samples per DIV:		%u\n",nSamplesDIV);
	printf("Number of total samples:	%i\n",nSamplesTOT);
	printf("PreTrigger samples:		%u\n",preTrigger);
	printf("PostTrigger samples:		%u\n",postTrigger);

	printf("\nIs it ok? (Y/N)\n");
	ch = YN_check();

	if (ch == 'Y') {
		// Continua senza modificare...
	}
	else if (ch == 'N') {
		printf("Insert samples per DIV:\n");
		cin >> nSamplesDIV;
		printf("Got %i\n",nSamplesDIV);
		printf("Insert preTrigger samples:\n");
		cin >> preTrigger;
		printf("Got %i\n",preTrigger);
		printf("Insert postTrigger samples:\n");
		cin >> postTrigger;
		printf("Got %i\n",postTrigger);
	}
	else {
		printf("Closing the device...\n");
		closeDevice(unit);
		printf("Bye!\n");
		return 0;
	}

	int32_t samplesRequired = preTrigger+postTrigger;
	float timeIntervalNanoseconds; // Intervallo temporale in ns tra due campionamenti

	printf("\nChecking the timebase...\n");

	for (int inSegment = 0; inSegment < NO_SEGMENTS-1; inSegment++) {
		do
		{
			 // Per ogni segmento di memoria controlla se il numero di samples scelto è adeguato per la timebase, in caso trova una timebase adeguata per tutti
			status = ps5000aGetTimebase2(unit->handle, timebase, nSamplesTOT, &timeIntervalNanoseconds, &maxSamples, inSegment);

			if(status == PICO_INVALID_NUMBER_CHANNELS_FOR_RESOLUTION)
			{
				printf("BlockDataHandler: Error - Invalid number of channels for resolution.\n");
				closeDevice(unit);
				return 0;
			}
			else if(status == PICO_OK)
			{
				// Do nothing
			}
			else
			{
				printf("Increasing the timebase...\n");
				timebase++;
			}

		}
		while(status != PICO_OK);
	}

	printf("\nTIMEBASE USED\n");
	printf("Timebase:			%i\n",timebase);
	printf("Time per DIV (ns):	%f\n",(timebase-2)*1./0.125);
	printf("Total samples:		%i\n",nSamplesTOT);
	printf("Max samples:		%i\n",maxSamples);
	printf("Time interval (ns):	%f\n",timeIntervalNanoseconds);
	printf("Samples stored per waveform (pre+post):	%i\n",samplesRequired);

	// Dove salvare i dati (gestione del file root)
	// store file name
	string file_name = "prova.root";
	// open file
	TFile* file = new TFile( file_name.c_str(), "RECREATE" );
	TTree* tree = new TTree( file_name.c_str(),"Acquisition" );

	// Acquisizione
	uint32_t nCompletedCaptures;
	uint32_t nCaptures;
	uint32_t capture;
//	int32_t timeIndisposed;
	int16_t channel;
	int16_t ***rapidBuffers;
	int16_t *overflow;

	unsigned long index = 0;
	event waveform;
	acq_info a_info;
	sampling_info s_info;

	s_info.nSamplesDIV = nSamplesDIV;
	s_info.samplesStored = samplesRequired;
	s_info.preTrig = preTrigger;
	s_info.timeInterval = timeIntervalNanoseconds; // Intervallo temporale tra due campionamenti, in ns

	a_info.waveformPerBlock = 0;
	a_info.elapsedTime = 0;
	a_info.waveformStored = 0;

	waveform.samplesStored = samplesRequired;
	waveform.sample = new float[samplesRequired];

	int64_t	*times;
	PS5000A_TIME_UNITS *timeUnits;

//	tree->Branch("Waveform",&waveform.nSamples,"nSamples/I:timeInterval/F:preTrig/I:time/I:timeUnit/I:sample[nSamples]/F");
	TBranch* brAcq = tree->Branch("Acquisition details",&a_info.waveformPerBlock,"WaveformPerBlock/I:ElapsedT_ms/i:TotalNumber/i");
	TBranch* brSamp = tree->Branch("Sampling info",&s_info.nSamplesDIV,"SamplesDIV/I:SamplesStored/I:PreTrigger/I:DeltaTsampling_ns:F");
	TBranch* branch = tree->Branch("Waveforms",&waveform.samplesStored,"SamplesStored/I:time/I:timeUnit/I:sample[SamplesStored]/F");

	brSamp->Fill();

	/* Start it collecting, then wait for completion*/

	auto begin = chrono::high_resolution_clock::now();
	chrono::time_point<std::chrono::system_clock> now = begin;
	auto dur = now - begin;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

	auto interval = chrono::seconds(5);

	unsigned long nMaxSignals = 1000000;

	while (index < nMaxSignals && dur < interval) {

		short retry = 0;
		do
		{
	//		 Inizializza il sistema

//				status = ps5000aRunBlock(unit->handle, preTrigger, postTrigger, timebase, &timeIndisposed, 0, callBackBlock, NULL);
			status = ps5000aRunBlock(unit->handle, preTrigger, postTrigger, timebase, NULL, 0, callBackBlock, NULL);

			if(status != PICO_OK)
			{
				if(status == PICO_POWER_SUPPLY_CONNECTED || status == PICO_POWER_SUPPLY_NOT_CONNECTED)
				{
					status = changePowerSource(unit->handle, status, unit);
					retry = 1;
				}
				else
				{
					printf("collectRapidBlock:ps5000aRunBlock ------ 0x%08lx \n", long(status));
				}
			}

		}
		while( retry ==  1);

		g_ready = FALSE;

		printf("Press any key to abort\n");
		while(g_ready == FALSE && !_kbhit())
		{
	//		printf("SONO NEL LOOP!\n");
			Sleep(0);
		}

		now = chrono::high_resolution_clock::now();
		dur = now - begin;
		ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
		a_info.elapsedTime = ms;
		cout << "Elapsed time (ms):	" << ms << endl;

		if(g_ready == FALSE)
		{
			printf("\nRapid capture aborted.\n");
			getchar();
			status = ps5000aGetNoOfCaptures(unit->handle, &nCompletedCaptures);
			printf("%i complete blocks were captured\n", nCompletedCaptures);
			nMaxSignals = index-1;
			printf("\nPress any key to continue...\n\n");
			getchar();

			//Only display the blocks that were captured
			nCaptures = (uint32_t)nCompletedCaptures;
		}
		else {
			status = ps5000aGetNoOfCaptures(unit->handle, &nCompletedCaptures);
			nCaptures = (uint32_t)nCompletedCaptures;

			printf("Rapid capture ended. %i complete blocks were captured\n", nCompletedCaptures);
//			printf("\nPress any key...\n\n");
//			getchar();
		}

		if (nCompletedCaptures > 0) {

			a_info.waveformPerBlock = nCompletedCaptures;

			//Allocate memory
			rapidBuffers = (int16_t ***) calloc(unit->channelCount, sizeof(int16_t*));
			overflow = (int16_t *) calloc(unit->channelCount * nCaptures, sizeof(int16_t));
			times = (int64_t *) calloc(unit->channelCount * nCaptures, sizeof(int64_t));
			timeUnits = (PS5000A_TIME_UNITS *) calloc(unit->channelCount * nCaptures, sizeof(PS5000A_TIME_UNITS));

			for (channel = 0; channel < unit->channelCount; channel++)
			{
				rapidBuffers[channel] = (int16_t **) calloc(nCaptures, sizeof(int16_t*));
			}

			for (channel = 0; channel < unit->channelCount; channel++)
			{
				if(unit->channelSettings[channel].enabled)
				{
					for (capture = 0; capture < nCaptures; capture++)
					{
						rapidBuffers[channel][capture] = (int16_t *) calloc(samplesRequired, sizeof(int16_t));
					}
				}
			}

			for (channel = 0; channel < unit->channelCount; channel++)
			{
				if(unit->channelSettings[channel].enabled)
				{
					for (capture = 0; capture < nCaptures; capture++)
					{
						status = ps5000aSetDataBuffer(unit->handle, (PS5000A_CHANNEL)channel, rapidBuffers[channel][capture], samplesRequired, capture, PS5000A_RATIO_MODE_NONE);
					}
				}
			}

			//Get data
			uint32_t samplesGot = samplesRequired;
			status = ps5000aGetValuesBulk(unit->handle, &samplesGot, 0, nCaptures - 1, 1, PS5000A_RATIO_MODE_NONE, overflow);

			if (status == PICO_POWER_SUPPLY_CONNECTED || status == PICO_POWER_SUPPLY_NOT_CONNECTED)
			{
				printf("\nPower Source Changed. Data collection aborted.\n");
				closeDevice(unit);
				return 0;
			}

			status = ps5000aGetValuesTriggerTimeOffsetBulk64(unit->handle, times, timeUnits, 0, nCaptures - 1);

			//Stop
			status = ps5000aStop(unit->handle);


			if (status == PICO_OK)
			{

				for (capture = 0; capture < nCaptures; capture++)
				{
	//				printf("\nCapture %d\n\n", capture + 1);

					for (channel = 0; channel < unit->channelCount; channel++)
					{
						if(unit->channelSettings[channel].enabled)
						{
	//						printf("Channel %c:\t", 'A' + channel);

							for(int i = 0; i < samplesRequired-1; i++)
							{
	//							if (i<10) printf("   %6f       ",adc_to_mv(rapidBuffers[channel][capture][i], unit->channelSettings[PS5000A_CHANNEL_A +channel].range, unit));
								waveform.sample[i] = adc_to_mv(rapidBuffers[channel][capture][i], unit->channelSettings[PS5000A_CHANNEL_A +channel].range, unit);
							}
							index++;
							waveform.time = times[capture+channel*nCaptures]; // Trigger instant
							waveform.timeUnit = timeUnits[capture+channel*nCaptures]; // Time unit of trigger instant
							branch->Fill();
						}
					}
	//				printf("\n");

				}
			}

			a_info.waveformStored = index;
			brAcq->Fill();

			//Free memory
			free(overflow);

			for (channel = 0; channel < unit->channelCount; channel++)
			{
				if(unit->channelSettings[channel].enabled)
				{
					for (capture = 0; capture < nCaptures; capture++)
					{
						free(rapidBuffers[channel][capture]);
					}
				}
			}

			for (channel = 0; channel < unit->channelCount; channel++)
			{
				free(rapidBuffers[channel]);
			}

			free(rapidBuffers);

		}

	}

	tree->Print();
	file->Write();
	file->Close();

	printf("Captured %lu waveforms\n",index);
	cout << "Elapsed time:	" << ms << endl << endl;

	printf("\nClosing the device...\n");
	closeDevice(unit);
	return 0;
}


