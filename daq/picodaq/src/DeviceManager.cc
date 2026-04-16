/*
 * DeviceManager.cpp
 *
 *  Created on: Dec 22, 2016
 *      Author: carlo
 */

#include "DeviceManager.h"

#include <string>
#include <chrono>
#include <fstream>

bool g_ready = false;

/****************************************************************************
* Callback
* used by PS5000A data block collection calls, on receipt of data.
* used to set global flags etc checked by user routines
****************************************************************************/
void PREF4 callBackBlock( int16_t handle, PICO_STATUS status, void * pParameter)
{
	if (status != PICO_CANCELLED)
		g_ready= true;
}

int32_t _kbhit()
{
        struct termios oldt, newt;
        int32_t bytesWaiting;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        setbuf(stdin, NULL);
        ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return bytesWaiting;
}


DeviceManager::DeviceManager(const char* config_file) {
	// TODO Auto-generated constructor stub
	g_ready = false;

	cout << "Configuro oggetto DeviceManager\n";
	unit = new UNIT[1];
	s_info = new SETTINGS[1];
	for (int32_t i=0; i<2; i++) {
		s_info->triggerSettings[i].triggerSet = 0;
		s_info->triggerSettings[i].triggerDirection = PS5000A_RISING;
		s_info->triggerSettings[i].triggerThreshold = 0.0;
	}

	cout << "Configuro file root\n";
//	string file_name = "prova.root";
	string file_name;
	cout << "Insert .root file name: ";
	cin >> file_name;
	cin.ignore();
	file_name = file_name + ".root";
	cout << "File name:\t" << file_name << endl;
	file = new TFile( file_name.c_str(), "RECREATE" );

	cout << "Creo i branches per le info\n";
	// inizializzo tree per le sampling settings
	treeCh = new TTree("Channels","Channels");
	// inizializzo tree per le sampling settings
	treeSamp = new TTree("SampSets","SampSets");
	// inizializzo tree per gli eventi
	treeEvt = new TTree( "Event","Event" );
	// inizializzo tree per le Running Time Info
	treeRTI = new TTree("RTI","RTI");

	cout << "Creo oggetto InfoAcq\n";
	info = new InfoAcq();

	cout << "Apro il device e lo configuro\n";
	openDevice();
	setChannelsFromFile(config_file);

	cout << "Creo oggetto Event\n";
	evt = new Event(s_info->samplesStored);
	FillStructsInfoAcq();

	cout << "Salvo sul file le informazioni dei canali e del sampling\n";
	info->CreateChannelBranches(treeCh);
	info->CreateSamplingBranches(treeSamp);
	treeCh->Fill();
	treeSamp->Fill();
	treeCh->AutoSave();
	treeSamp->AutoSave();

	cout << "Creo i branches per gli eventi e le RTI\n";
	evt->CreateEventBranches(treeEvt);
	evt->CreateRTIBranches(treeRTI);

}

DeviceManager::~DeviceManager() {
	// TODO Auto-generated destructor stub
	file->Write();
	file->Close();
	closeDevice();
}

void DeviceManager::openDevice() {

	PICO_STATUS status;

	int8_t ch = '.';

	do {

		printf("Opening...\n");
		status = ps5000aOpenUnit(&unit->handle, NULL, PS5000A_DR_12BIT);
		unit->openStatus = (int16_t) status;
		unit->complete = 1;

		if (status == PICO_OK || status == PICO_POWER_SUPPLY_NOT_CONNECTED)
		{
			printf("\nDEVICE FOUND!\n");
			unit->openStatus = (int16_t) status;
			set_info();
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
		exit(0);
	}

	if (unit->openStatus == PICO_POWER_SUPPLY_NOT_CONNECTED)
	{
		unit->openStatus = (int16_t) changePowerSource(PICO_POWER_SUPPLY_NOT_CONNECTED);
	}

}

/****************************************************************************
* closeDevice
****************************************************************************/
void DeviceManager::closeDevice()
{
	ps5000aCloseUnit(unit->handle);
}

/****************************************************************************
* Initialize unit' structure with Variant specific defaults
****************************************************************************/
void DeviceManager::set_info()
{
	int8_t description [11][25]= { "Driver Version",
		"USB Version",
		"Hardware Version",
		"Variant Info",
		"Serial",
		"Cal Date",
		"Kernel Version",
		"Digital HW Version",
		"Analogue HW Version",
		"Firmware 1",
		"Firmware 2"};

	int16_t i = 0;
	int16_t requiredSize = 0;
	int8_t line [10];

	unit->model				= MODEL_PS5244B;
	unit->sigGen			= SIGGEN_AWG;
	unit->firstRange		= PS5000A_10MV;
	unit->lastRange			= PS5000A_20V;
	unit->channelCount		= DUAL_SCOPE;
	unit->hasHardwareETS	= TRUE;
	unit->awgBufferSize		= PS5X44A_MAX_SIG_GEN_BUFFER_SIZE;

	if (unit->handle)
	{
		for (i = 0; i < 11; i++)
		{
			ps5000aGetUnitInfo(unit->handle, line, sizeof (line), &requiredSize, i);
			printf("%s: %s\n", description[i], line);
		}
		printf("\n");
	}
};

/****************************************************************************************
* ChangePowerSource - function to handle switches between +5V supply, and USB only power
* Only applies to PS544xA/B units
******************************************************************************************/
PICO_STATUS DeviceManager::changePowerSource(PICO_STATUS status)
{
	int8_t ch = 0;

	switch (status)
	{
		case PICO_POWER_SUPPLY_NOT_CONNECTED:			// User must acknowledge they want to power via USB

					printf("\nPowering the unit via USB\n");
					status = ps5000aChangePowerSource(unit->handle, PICO_POWER_SUPPLY_NOT_CONNECTED);		// Tell the driver that's ok

					if(status == PICO_OK && unit->channelCount == QUAD_SCOPE)
					{
						unit->channelSettings[PS5000A_CHANNEL_C].enabled = FALSE;
						unit->channelSettings[PS5000A_CHANNEL_D].enabled = FALSE;
					}
					else if (status == PICO_POWER_SUPPLY_UNDERVOLTAGE)
					{
						status = this->changePowerSource(status);
					}
					else
					{
						// Do nothing
					}


		case PICO_POWER_SUPPLY_CONNECTED:
			printf("\nUsing +5V power supply voltage\n");
			status = ps5000aChangePowerSource(unit->handle, PICO_POWER_SUPPLY_CONNECTED);					// Tell the driver we are powered from +5V supply
			break;

		case PICO_POWER_SUPPLY_UNDERVOLTAGE:
			do
			{
				printf("\nUSB not supplying required voltage");
				printf("\nPlease plug in the +5V power supply\n");
				printf("\nHit any key to continue, or Esc to exit...\n");
				ch = getchar();
				if (ch == 0x1B)	// ESC key
					exit(0);
				else
					status = ps5000aChangePowerSource(unit->handle, PICO_POWER_SUPPLY_CONNECTED);		// Tell the driver that's ok
			}
			while (status == PICO_POWER_SUPPLY_REQUEST_INVALID);
			break;
	}
	return status;
}


/****************************************************************************
* setChannels A and B
****************************************************************************/
PICO_STATUS DeviceManager::setChannels()
{
	PICO_STATUS status;
	int16_t * trigger = new int16_t[2];
	trigger[0] = 0;
	trigger[1] = 0;
	PS5000A_THRESHOLD_DIRECTION triggerDirections[2];
	float triggerThresholdsMV[2] = {0.0,0.0};
	float analogueOffsetMV[2] = {0.0,0.0};
	int8_t ch = '.';

	printf("\nSetting channels...\n");

	do
	{
		for (int32_t inCh = 0; inCh < 2; inCh++) {

			printf("\nDo you want to enable channel %c? Y/N\n",'A'+inCh);
			int8_t chEnable;

			chEnable = YN_check();

			if (chEnable == 'Y') {

				printf("\nEnabling channel %c...\n",'A'+inCh);
				unit->channelSettings[inCh].enabled = 1;

				// Accoppiamento AC-DC
				printf("\nSelect coupling:\n");
				printf("A - AC coupling\n");
				printf("D - DC coupling\n");
				ch = toupper(getchar());

				while ((ch != 'A') && (ch != 'D')) {
					printf("Insert correct option\n");
					ch = getchar();
					ch = toupper(ch);
				}

				if (ch == 'A') {
					printf("AC coupling selected\n");
					unit->channelSettings[inCh].DCcoupled = PS5000A_AC;
				}
				else {
					printf("DC coupling selected\n");
					unit->channelSettings[inCh].DCcoupled = PS5000A_DC;
				}

				// Range del canale
				printf("\nSelect range:\n");
				printf("1: +/- 10 mV					7: +/- 1 V\n");
				printf("2: +/- 20 mV					8: +/- 2 V\n");
				printf("3: +/- 50 mV					9: +/- 5 V\n");
				printf("4: +/- 100 mV					10: +/- 10 V\n");
				printf("5: +/- 200 mV					11: +/- 20 V\n");
				printf("6: +/- 500 mV\n");

				int32_t range;
				while (scanf ("%i",&range) != 1)
				{
					printf("INSERT 1 VALUE\n");
				}

				while ((range < 1) || (range > 11)) {
					printf("\nInsert correct value\n");
					while (scanf ("%i",&range) != 1)
					{
						printf("INSERT 1 VALUE\n");
					}
				}

				printf("Selected range %i\n",range);
				unit->channelSettings[inCh].range = int16_t(range)-1;

				printf("\nDo you want to set an analogue offset?\n");
				ch = YN_check();

				if (ch == 'N') unit->channelSettings[inCh].analogueOffset = 0.0;

				else if (ch == 'Y') {

					float maximumVoltage = 0.0;
					float minimumVoltage = 0.0;

					status = ps5000aGetAnalogueOffset(unit->handle,psRanges[unit->channelSettings[inCh].range],unit->channelSettings[inCh].DCcoupled,&maximumVoltage,&minimumVoltage);

					printf("\nSet analogue offset for channel %c (mV):\n",'A'+inCh);
					printf("Offset must be between %f mV ",minimumVoltage);
					printf("and %f mV\n",maximumVoltage);
					float tmp = 0.0;
					cin >> tmp;
					printf("Offset chosen: %f\n",tmp);

					while (tmp >= maximumVoltage*1. || tmp <= minimumVoltage*1.) {
						printf("Offset must be between %f mV",minimumVoltage);
						printf(" and %f mV\n",maximumVoltage);
						cin >> tmp;
						printf("Offset chosen: %f\n",tmp);
					}

					analogueOffsetMV[inCh] = tmp;
					s_info->channelSettings[inCh].analogueOffset = analogueOffsetMV[inCh];
					unit->channelSettings[inCh].analogueOffset = analogueOffsetMV[inCh];
					cout << "ANALOGUE OFFSET\t" << s_info->channelSettings[inCh].analogueOffset << endl;

				}

				// Direzione trigger del canale
				printf("\nSet trigger for channel %c?\n",'A'+inCh);
				ch = YN_check();

				if (ch == 'Y') {

					s_info->triggerSettings[inCh].triggerSet = 1;
					trigger[inCh] = 1;
					printf("\nSet trigger direction for channel %c:\n",'A'+inCh);
					printf("R - Rising\n");
					printf("F - Falling\n");

					ch = getchar();
					ch = toupper(ch);

					while ((ch != 'R') && (ch != 'F')) {
						printf("Insert correct option\n");
						ch = getchar();
						ch = toupper(ch);
					}

					cout <<"Selected " << ch << " case\n";

					if (ch == 'R') triggerDirections[inCh] = PS5000A_RISING;
					if (ch == 'F') triggerDirections[inCh] = PS5000A_FALLING;

					s_info->triggerSettings[inCh].triggerDirection = triggerDirections[inCh];

					if ((ch == 'R') || (ch == 'F')) {

						printf("\nSet trigger threshold for channel %c (mV):\n",'A'+inCh);
						printf("Threshold must be in range +/- %i mV\n",psRangesMV[unit->channelSettings[inCh].range]);
						float tmp;
						while (scanf ("%f",&tmp) != 1)
						{
							printf("INSERT 1 VALUE\n");
						}

						printf("Threshold chosen: %f\n",tmp);

						while (tmp >= psRangesMV[unit->channelSettings[inCh].range]) {
							printf("Threshold must be in range +/- %i mV\n",psRangesMV[unit->channelSettings[inCh].range]);
							while (scanf ("%f",&tmp) != 1)
							{
								printf("INSERT 1 VALUE\n");
							}
						}

						triggerThresholdsMV[inCh] = tmp;
						s_info->triggerSettings[inCh].triggerThreshold = triggerThresholdsMV[inCh];
						printf("Threshold set: %f\n",s_info->triggerSettings[inCh].triggerThreshold);

					}

				}
				else if (ch == 'N') trigger[inCh] = 0;
				else {
					printf("Closing the device...\n");
					closeDevice();
					printf("Bye!\n");
					if (file->IsOpen()) file->Close();
					exit(0);
				}
			}

			else if (chEnable == 'N') {
				unit->channelSettings[inCh].enabled = 0;
				unit->channelSettings[inCh].DCcoupled = PS5000A_DC;
				unit->channelSettings[inCh].range = 1;
				unit->channelSettings[inCh].analogueOffset = 0.0;			}

			else {
				printf("Closing the device...\n");
				closeDevice();
				printf("Bye!\n");
				if (file->IsOpen()) file->Close();
				exit(0);
			}

		}
	}
	while(unit->channelSettings[0].enabled == 0 && unit->channelSettings[1].enabled == 0);

	if (unit->channelSettings[0].enabled == 1) printf("Channel A enabled\n");
	else printf("Channel A not enabled\n");

	if (unit->channelSettings[1].enabled == 1) printf("Channel B enabled\n");
	else printf("Channel B not enabled\n");

	status = ps5000aSetChannel(	unit->handle,PS5000A_CHANNEL_A,unit->channelSettings[0].enabled,
								unit->channelSettings[0].DCcoupled,psRanges[unit->channelSettings[0].range],
								unit->channelSettings[0].analogueOffset);
	if (status != PICO_OK) {
		printf("PROBLEM SETTING CHANNEL %c\n",'A');
	}

	status = ps5000aSetChannel(	unit->handle,PS5000A_CHANNEL_B,unit->channelSettings[1].enabled,
								unit->channelSettings[1].DCcoupled,psRanges[unit->channelSettings[1].range],
								unit->channelSettings[1].analogueOffset);
	if (status != PICO_OK) {
		printf("PROBLEM SETTING CHANNEL %c\n",'B');
		return PICO_INVALID_CHANNEL;
	}

	s_info->channelSettings[0] = unit->channelSettings[0];
	s_info->channelSettings[1] = unit->channelSettings[1];


	// Chiama la funzione per settare il trigger
	// Imposta la direzione
	status = setSimpleTrigger(trigger,triggerDirections,triggerThresholdsMV,2);
	if (status == PICO_OK) printf("\nTriggering set correctly!\n");
	else {
		printf("PROBLEM SETTING TRIGGER\n");
		return PICO_INVALID_TRIGGER_STATES;
	}

	printf("Press any key\n");
	getchar();

	// Definisco numero waveform da registrare (uso la modalità rapid block mode per avere deadtime ridotto, vedi programmer guide)
	int32_t maxSamples; // Numero massimo di samples per questa timebase
	int32_t no_segments = 1024;
	int32_t no_waveforms = 100;

	printf("Setting acquisition properties...");
	printf("\nDEFAULT:\n");
	printf("\nNumber of segments:	%i\n",no_segments);
	printf("Number of waveforms:	%i\n",no_waveforms);
	printf("\nIs it ok?\n");
	ch = YN_check();

	while (ch == 'N') {
		printf("Insert number of segments:\n");
		cin >> no_segments;
		printf("Insert number of waveforms (must be less than %i):\n",no_segments);
		cin >> no_waveforms;
		printf("\nNumber of segments:	%i\n",no_segments);
		printf("Number of waveforms:	%i\n",no_waveforms);

		printf("\nIs it ok?\n");
		ch = YN_check();
	}

	status = ps5000aMemorySegments(unit->handle,no_segments,&maxSamples); // Imposto il numero di segmenti di memoria
	status = ps5000aSetNoOfCaptures(unit->handle, no_waveforms); // Imposto il numero di waveform da registrare

	printf("\nMax samples per div:	%i\n",maxSamples);

	// Imposta la timebase
	uint32_t nSamplesDIV = 100; // Numero di samples in una divisione richiesti
	uint32_t nSamplesTOT = nSamplesDIV*10; // Numero di samples totali

	int32_t preTrigger = nSamplesDIV*2;
	int32_t postTrigger = nSamplesDIV*5;
	uint32_t timebase = 4; // Dovrebbe corrispondere a (timebase-2)/1.25e8 s per sample (se timebase=127, corrisponde a 1 microsec/sample)

	printf("\nSetting sampling properties...\n");
	printf("\nDEFAULT:\n");
	printf("Timebase:					%i\n",timebase);
	printf("Time per SAMPLE (ns):			%f\n",(timebase-2)*1./0.125);

	printf("\nIs it ok?\n");
	ch = YN_check();

	while (ch == 'N') {
		printf("Time per DIV (ns) = (timebase-2)/0.125\n");
		printf("Insert timebase:\n");
		cin >> timebase;

		printf("Got %i\n",timebase);
		printf("Time per SAMPLE (ns):			%f\n",(timebase-2)*1./0.125);

		printf("\nIs it ok?\n");
		ch = YN_check();
	}

	printf("\nTimebase:					%i\n",timebase);
	printf("Sampling interval (ns):			%f\n",(timebase-2)*1./0.125);

	printf("\nDEFAULT:\n");
	printf("Samples per DIV:		%u\n",nSamplesDIV);
	printf("Number of total samples:	%i\n",nSamplesTOT);
	printf("PreTrigger samples:		%u\n",preTrigger);
	printf("PostTrigger samples:		%u\n",postTrigger);

	printf("\nIs it ok?\n");
	ch = YN_check();

	while (ch == 'N') {
		printf("Insert samples per DIV:\n");
		cin >> nSamplesDIV;
		nSamplesTOT = 10*nSamplesDIV;
		printf("Insert preTrigger samples:\n");
		cin >> preTrigger;
		printf("Insert postTrigger samples:\n");
		cin >> postTrigger;

		printf("Samples per DIV:		%u\n",nSamplesDIV);
		printf("Number of total samples:	%i\n",nSamplesTOT);
		printf("PreTrigger samples:		%u\n",preTrigger);
		printf("PostTrigger samples:		%u\n",postTrigger);

		printf("\nIs it ok?\n");
		ch = YN_check();
	}

	int32_t samplesRequired = preTrigger+postTrigger;
	float timeIntervalNanoseconds; // Intervallo temporale in ns tra due campionamenti

	printf("\nChecking the timebase...\n");

	for (int32_t inSegment = 0; inSegment < no_segments-1; inSegment++) {
		do
		{
			 // Per ogni segmento di memoria controlla se il numero di samples scelto è adeguato per la timebase, in caso trova una timebase adeguata per tutti
			status = ps5000aGetTimebase2(unit->handle, timebase, nSamplesTOT, &timeIntervalNanoseconds, &maxSamples, inSegment);

			if(status == PICO_INVALID_NUMBER_CHANNELS_FOR_RESOLUTION)
			{
				printf("BlockDataHandler: Error - Invalid number of channels for resolution.\n");
				closeDevice();
				if (file->IsOpen()) file->Close();
				exit(0);
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
	printf("Sampling interval (ns):	%f\n",(timebase-2)*1./0.125);
	printf("Total samples:		%i\n",nSamplesTOT);
	printf("Max samples:		%i\n",maxSamples);
	printf("Time interval (ns):	%f\n",timeIntervalNanoseconds);
	printf("Samples stored per waveform (pre+post):	%i\n",samplesRequired);

	s_info->no_segments = no_segments;
	s_info->no_waveforms = no_waveforms;
	s_info->nSamplesDIV = nSamplesDIV;
	s_info->preTrig = preTrigger;
	s_info->samplesStored = samplesRequired;
	s_info->timebase = timebase;
	s_info->timeIntervalNanoseconds = timeIntervalNanoseconds;

	uint64_t durata = 0;
	cout << "\nInsert acquisition time (s): ";
	while (durata <= 0.0) {
		cin >> durata;
	}

	uint64_t nMaxSignals = 0;

	printf("Insert number of waveforms to acquire: ");
	while (nMaxSignals <= 0) {
		cin >> nMaxSignals;
		cin.ignore();
	}

	s_info->durata_s = durata;
	s_info->nMaxSignals = nMaxSignals;

	cout << endl << "Configuration completed\n";
	cout << "Press enter to continue\n";
	getchar();

	return PICO_OK;
}


#include <iostream>
#include <sstream>
#include <fstream>
std::istream& get_line_strip_comments( std::istream& stm, std::string& str )
{
    if( std::getline( stm, str ) )
    {
        auto pos = str.find( "#" ) ;
        if( pos == 0 ) return get_line_strip_comments( stm, str ) ;
        else if( pos != std::string::npos ) str.erase(pos) ;
    }
    return stm ;
}

/****************************************************************************
* setChannels A and B from file config
****************************************************************************/
PICO_STATUS DeviceManager::setChannelsFromFile(const char* config_file)
{
	PICO_STATUS status;
	int16_t * trigger = new int16_t[2];
	trigger[0] = 0;
	trigger[1] = 0;
	PS5000A_THRESHOLD_DIRECTION triggerDirections[2];
	float triggerThresholdsMV[2] = {0.0,0.0};
	float analogueOffsetMV[2] = {0.0,0.0};
	int8_t ch = '.';
	/*
	ifstream config;
	config.open(config_file);
	*/
	std::ifstream stm;
	stm.open(config_file);
	std::string str ;
	std::stringstream config;
	while( get_line_strip_comments( stm, str ) ) config << str << '\n' ;
	

	printf("\nSetting channels...\n");

	do
	{
		for (int32_t inCh = 0; inCh < 2; inCh++) {
		  
			printf("\nDo you want to enable channel %c? Y/N\n",'A'+inCh);
			int8_t chEnable;

			config >> chEnable;
			chEnable = toupper(chEnable);
			
			if (chEnable == 'Y') {

				printf("\nEnabling channel %c...\n",'A'+inCh);
				unit->channelSettings[inCh].enabled = 1;

				// Accoppiamento AC-DC
				printf("\nSelect coupling:\n");
				printf("A - AC coupling\n");
				printf("D - DC coupling\n");
				config >> ch;
				ch = toupper(ch);

				if (ch == 'A') {
					printf("AC coupling selected\n");
					unit->channelSettings[inCh].DCcoupled = PS5000A_AC;
				}
				else {
					printf("DC coupling selected\n");
					unit->channelSettings[inCh].DCcoupled = PS5000A_DC;
				}

				// Range del canale
				printf("\nSelect range:\n");
				printf("1: +/- 10 mV					7: +/- 1 V\n");
				printf("2: +/- 20 mV					8: +/- 2 V\n");
				printf("3: +/- 50 mV					9: +/- 5 V\n");
				printf("4: +/- 100 mV					10: +/- 10 V\n");
				printf("5: +/- 200 mV					11: +/- 20 V\n");
				printf("6: +/- 500 mV\n");

				int32_t range;

				config >> range;

				printf("Selected range %i\n",range);
				unit->channelSettings[inCh].range = int16_t(range)-1;

				printf("\nDo you want to set an analogue offset?\n");
				config >> ch;
				ch = toupper(ch);

				if (ch == 'N') unit->channelSettings[inCh].analogueOffset = 0.0;

				else if (ch == 'Y') {

					float maximumVoltage = 0.0;
					float minimumVoltage = 0.0;

					status = ps5000aGetAnalogueOffset(unit->handle,psRanges[unit->channelSettings[inCh].range],unit->channelSettings[inCh].DCcoupled,&maximumVoltage,&minimumVoltage);

					printf("\nSet analogue offset for channel %c (mV):\n",'A'+inCh);
					printf("Offset must be between %f mV ",minimumVoltage);
					printf("and %f mV\n",maximumVoltage);
					float tmp = 0.0;
					config >> tmp;
					printf("Offset chosen: %f\n",tmp);

					analogueOffsetMV[inCh] = tmp;
					s_info->channelSettings[inCh].analogueOffset = analogueOffsetMV[inCh];
					unit->channelSettings[inCh].analogueOffset = analogueOffsetMV[inCh];
					cout << "ANALOGUE OFFSET\t" << s_info->channelSettings[inCh].analogueOffset << endl;

				}

				// Direzione trigger del canale
				printf("\nSet trigger for channel %c?\n",'A'+inCh);
				config >> ch;
				ch = toupper(ch);

				if (ch == 'Y') {

					s_info->triggerSettings[inCh].triggerSet = 1;
					trigger[inCh] = 1;
					printf("\nSet trigger direction for channel %c:\n",'A'+inCh);
					printf("R - Rising\n");
					printf("F - Falling\n");

					config >> ch;
					ch = toupper(ch);

					while ((ch != 'R') && (ch != 'F')) {
						printf("Insert correct option\n");
						config >> ch;
						ch = toupper(ch);
					}

					cout <<"Selected " << ch << " case\n";

					if (ch == 'R') triggerDirections[inCh] = PS5000A_RISING;
					if (ch == 'F') triggerDirections[inCh] = PS5000A_FALLING;

					s_info->triggerSettings[inCh].triggerDirection = triggerDirections[inCh];

					if ((ch == 'R') || (ch == 'F')) {

						printf("\nSet trigger threshold for channel %c (mV):\n",'A'+inCh);
						printf("Threshold must be in range +/- %i mV\n",psRangesMV[unit->channelSettings[inCh].range]);
						float tmp;
						config >> tmp;

						printf("Threshold chosen: %f\n",tmp);

						while (tmp >= psRangesMV[unit->channelSettings[inCh].range]) {
							printf("Threshold must be in range +/- %i mV\n",psRangesMV[unit->channelSettings[inCh].range]);
							config >> tmp;
						}

						triggerThresholdsMV[inCh] = tmp;
						s_info->triggerSettings[inCh].triggerThreshold = triggerThresholdsMV[inCh];
						printf("Threshold set: %f\n",s_info->triggerSettings[inCh].triggerThreshold);

					}

				}
				else if (ch == 'N') trigger[inCh] = 0;
				else {
					printf("Closing the device...\n");
					closeDevice();
					printf("Bye!\n");
					if (file->IsOpen()) file->Close();
					exit(0);
				}
			}

			else if (chEnable == 'N') {
				unit->channelSettings[inCh].enabled = 0;
				unit->channelSettings[inCh].DCcoupled = PS5000A_DC;
				unit->channelSettings[inCh].range = 1;
				unit->channelSettings[inCh].analogueOffset = 0.0;			}

			else {
				printf("Closing the device...\n");
				closeDevice();
				printf("Bye!\n");
				if (file->IsOpen()) file->Close();
				exit(0);
			}

		}
	}
	while(unit->channelSettings[0].enabled == 0 && unit->channelSettings[1].enabled == 0);

	if (unit->channelSettings[0].enabled == 1) printf("Channel A enabled\n");
	else printf("Channel A not enabled\n");

	if (unit->channelSettings[1].enabled == 1) printf("Channel B enabled\n");
	else printf("Channel B not enabled\n");

	status = ps5000aSetChannel(	unit->handle,PS5000A_CHANNEL_A,unit->channelSettings[0].enabled,
								unit->channelSettings[0].DCcoupled,psRanges[unit->channelSettings[0].range],
								unit->channelSettings[0].analogueOffset);
	if (status != PICO_OK) {
		printf("PROBLEM SETTING CHANNEL %c\n",'A');
	}

	status = ps5000aSetChannel(	unit->handle,PS5000A_CHANNEL_B,unit->channelSettings[1].enabled,
								unit->channelSettings[1].DCcoupled,psRanges[unit->channelSettings[1].range],
								unit->channelSettings[1].analogueOffset);
	if (status != PICO_OK) {
		printf("PROBLEM SETTING CHANNEL %c\n",'B');
		return PICO_INVALID_CHANNEL;
	}

	s_info->channelSettings[0] = unit->channelSettings[0];
	s_info->channelSettings[1] = unit->channelSettings[1];

	// Chiama la funzione per settare il trigger
	// Imposta la direzione
	status = setSimpleTrigger(trigger,triggerDirections,triggerThresholdsMV,2);
	if (status == PICO_OK) printf("\nTriggering set correctly!\n");
	else {
		printf("PROBLEM SETTING TRIGGER\n");
		return PICO_INVALID_TRIGGER_STATES;
	}

	// Definisco numero waveform da registrare (uso la modalità rapid block mode per avere deadtime ridotto, vedi programmer guide)
	int32_t maxSamples; // Numero massimo di samples per questa timebase
	int32_t no_segments = 1024;
	int32_t no_waveforms = 100;

	printf("Setting acquisition properties...");
	printf("\nDEFAULT:\n");
	printf("\nNumber of segments:	%i\n",no_segments);
	printf("Number of waveforms:	%i\n",no_waveforms);
	printf("\nIs it ok?\n");
	config >> ch;
	ch = toupper(ch);

	while (ch == 'N') {
		printf("Insert number of segments:\n");
		config >> no_segments;
		printf("Insert number of waveforms (must be less than %i):\n",no_segments);
		config >> no_waveforms;
		printf("\nNumber of segments:	%i\n",no_segments);
		printf("Number of waveforms:	%i\n",no_waveforms);

		printf("\nIs it ok?\n");
		config >> ch;
		ch = toupper(ch);
	}

	status = ps5000aMemorySegments(unit->handle,no_segments,&maxSamples); // Imposto il numero di segmenti di memoria
	status = ps5000aSetNoOfCaptures(unit->handle, no_waveforms); // Imposto il numero di waveform da registrare

	printf("\nMax samples per div:	%i\n",maxSamples);

	// Imposta la timebase
	uint32_t nSamplesDIV = 100; // Numero di samples in una divisione richiesti
	uint32_t nSamplesTOT = nSamplesDIV*10; // Numero di samples totali

	int32_t preTrigger = nSamplesDIV*2;
	int32_t postTrigger = nSamplesDIV*5;
	uint32_t timebase = 4; // Dovrebbe corrispondere a (timebase-2)/1.25e8 s per sample (se timebase=127, corrisponde a 1 microsec/sample)

	printf("\nSetting sampling properties...\n");
	printf("\nDEFAULT:\n");
	printf("Timebase:					%i\n",timebase);
	printf("Time per SAMPLE (ns):			%f\n",(timebase-2)*1./0.125);

	printf("\nIs it ok?\n");
	config >> ch;
	ch = toupper(ch);

	while (ch == 'N') {
		printf("Time per Sample (ns) = (timebase-2)/0.125\n");
		printf("Insert timebase:\n");
		config >> timebase;

		printf("Got %i\n",timebase);
		printf("Time per SAMPLE (ns):			%f\n",(timebase-2)*1./0.125);

		printf("\nIs it ok?\n");
		config >> ch;
		ch = toupper(ch);
	}

	printf("\nTimebase:					%i\n",timebase);
	printf("Sampling interval (ns):			%f\n",(timebase-2)*1./0.125);

	printf("\nDEFAULT:\n");
	printf("Samples per DIV:		%u\n",nSamplesDIV);
	printf("Number of total samples:	%i\n",nSamplesTOT);
	printf("PreTrigger samples:		%u\n",preTrigger);
	printf("PostTrigger samples:		%u\n",postTrigger);

	printf("\nIs it ok?\n");
	config >> ch;
	ch = toupper(ch);

	while (ch == 'N') {
		printf("Insert samples per DIV:\n");
		config >> nSamplesDIV;
		nSamplesTOT = 10*nSamplesDIV;
		printf("Insert preTrigger samples:\n");
		config >> preTrigger;
		printf("Insert postTrigger samples:\n");
		config >> postTrigger;

		printf("Samples per DIV:		%u\n",nSamplesDIV);
		printf("Number of total samples:	%i\n",nSamplesTOT);
		printf("PreTrigger samples:		%u\n",preTrigger);
		printf("PostTrigger samples:		%u\n",postTrigger);

		printf("\nIs it ok?\n");
		config >> ch;
		ch = toupper(ch);
	}

	int32_t samplesRequired = preTrigger+postTrigger;
	float timeIntervalNanoseconds; // Intervallo temporale in ns tra due campionamenti

	printf("\nChecking the timebase...\n");

	for (int32_t inSegment = 0; inSegment < no_segments-1; inSegment++) {
		do
		{
			 // Per ogni segmento di memoria controlla se il numero di samples scelto è adeguato per la timebase, in caso trova una timebase adeguata per tutti
			status = ps5000aGetTimebase2(unit->handle, timebase, nSamplesTOT, &timeIntervalNanoseconds, &maxSamples, inSegment);

			if(status == PICO_INVALID_NUMBER_CHANNELS_FOR_RESOLUTION)
			{
				printf("BlockDataHandler: Error - Invalid number of channels for resolution.\n");
				closeDevice();
				if (file->IsOpen()) file->Close();
				exit(0);
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
	printf("Sampling interval (ns):	%f\n",(timebase-2)*1./0.125);
	printf("Total samples:		%i\n",nSamplesTOT);
	printf("Max samples:		%i\n",maxSamples);
	printf("Time interval (ns):	%f\n",timeIntervalNanoseconds);
	printf("Samples stored per waveform (pre+post):	%i\n",samplesRequired);

	s_info->no_segments = no_segments;
	s_info->no_waveforms = no_waveforms;
	s_info->nSamplesDIV = nSamplesDIV;
	s_info->preTrig = preTrigger;
	s_info->samplesStored = samplesRequired;
	s_info->timebase = timebase;
	s_info->timeIntervalNanoseconds = timeIntervalNanoseconds;

	uint64_t durata = 0;
	cout << "\nInsert acquisition time (s): ";
	while (durata <= 0.0) {
		config >> durata;
	}

	uint64_t nMaxSignals = 0;

	printf("Insert number of waveforms to acquire: ");
	while (nMaxSignals <= 0) {
		config >> nMaxSignals;
	}

	s_info->durata_s = durata;
	s_info->nMaxSignals = nMaxSignals;

	cout << endl << "Configuration completed\n";
	cout << "Press enter to continue\n";

	return PICO_OK;
}


/****************************************************************************
* setTrigger for channels A and B
*
* - Used to call all the functions required to set up simple triggering
*
***************************************************************************/
PICO_STATUS DeviceManager::setSimpleTrigger(
		int16_t * trigger,
		PS5000A_THRESHOLD_DIRECTION * triggerDirections,
		float * triggerThresholdMV,
		int16_t ntriggering )
{
	PICO_STATUS status;
	int16_t value;
	ps5000aMaximumValue(unit->handle, &value);
	unit->maxADCValue = value;

	status = PICO_OK;

	for (int32_t i=0; i<ntriggering; i++){
		if (trigger[i] != 0) {
			int16_t threshold = int16_t(triggerThresholdMV[i] * value / psRangesMV[unit->channelSettings[i].range]);
			cout << "MAX THRESHOLD VALUE (ADC):\t" << value << endl;
			cout << "Threshold (mV):\t" << triggerThresholdMV[i] << "\t--->\t" << threshold << endl;
			if (i == 0) {
				status = ps5000aSetSimpleTrigger(unit->handle,trigger[i],PS5000A_CHANNEL_A,threshold,triggerDirections[i],0,0);
				printf("Channel A trigger set\n");
			}
			if (i == 1) {
				status = ps5000aSetSimpleTrigger(unit->handle,trigger[i],PS5000A_CHANNEL_B,threshold,triggerDirections[i],0,0);
				printf("Channel B trigger set\n");
			}
		}
		else {
			if (i == 0) printf("Channel A trigger not set\n");
			if (i == 1) printf("Channel B trigger not set\n");
		}
	}

	return status;
}


void DeviceManager::FillStructsInfoAcq() {

	InfoAcq::chSettings* chSet1 = new InfoAcq::chSettings[1];
	InfoAcq::chSettings* chSet2 = new InfoAcq::chSettings[1];
	InfoAcq::samplingSettings *sampSet = new InfoAcq::samplingSettings[1];

	if (s_info->channelSettings[0].enabled != 0) {
		chSet1->enabled = true;
		chSet1->range = s_info->channelSettings[0].range;
		chSet1->analogueOffset = s_info->channelSettings[0].analogueOffset;

		if (s_info->channelSettings[0].DCcoupled == PS5000A_AC) chSet1->DCcoupled = 'A';
		else if (s_info->channelSettings[0].DCcoupled == PS5000A_DC) chSet1->DCcoupled = 'D';

		if (s_info->triggerSettings[0].triggerSet != 0) {
			chSet1->triggerSet = true;
			chSet1->triggerThreshold = s_info->triggerSettings[0].triggerThreshold;
			if (s_info->triggerSettings[0].triggerDirection == PS5000A_RISING) chSet1->triggerDirection = 'R';
			else if (s_info->triggerSettings[0].triggerDirection == PS5000A_FALLING) chSet1->triggerDirection = 'F';
		}
		else {
			chSet1->triggerSet = false;
			chSet1->triggerThreshold = 0.0;
			chSet1->triggerDirection = 'N';
		}
	}
	else {
		chSet1->enabled = false;
		chSet1->range = -1;
		chSet1->analogueOffset = 0.0;
		chSet1->DCcoupled = 'N';
		chSet1->triggerSet = false;
		chSet1->triggerThreshold = 0.0;
		chSet1->triggerDirection = 'N';
	}

	if (s_info->channelSettings[1].enabled != 0) {
		chSet2->enabled = true;
		chSet2->range = s_info->channelSettings[1].range;
		chSet2->analogueOffset = s_info->channelSettings[1].analogueOffset;

		if (s_info->channelSettings[1].DCcoupled == PS5000A_AC) chSet2->DCcoupled = 'A';
		else if (s_info->channelSettings[1].DCcoupled == PS5000A_DC) chSet2->DCcoupled = 'D';

		if (s_info->triggerSettings[1].triggerSet != 0) {
			chSet2->triggerSet = true;
			chSet2->triggerThreshold = s_info->triggerSettings[1].triggerThreshold;
			if (s_info->triggerSettings[1].triggerDirection == PS5000A_RISING) chSet2->triggerDirection = 'R';
			else if (s_info->triggerSettings[1].triggerDirection == PS5000A_FALLING) chSet2->triggerDirection = 'F';
		}
		else {
			chSet2->triggerSet = false;
			chSet2->triggerThreshold = 0.0;
			chSet2->triggerDirection = 'N';
		}
	}
	else {
		chSet2->enabled = false;
		chSet2->range = -1;
		chSet2->analogueOffset = 0.0;
		chSet2->DCcoupled = 'N';
		chSet2->triggerSet = false;
		chSet2->triggerThreshold = 0.0;
		chSet2->triggerDirection = 'N';
	}

	sampSet->max_adc_value = (int)unit->maxADCValue;
	sampSet->nSamplesDIV = s_info->nSamplesDIV;
	sampSet->no_segments = (uint32_t)s_info->no_segments;
	sampSet->timebase = s_info->timebase;
	sampSet->no_waveforms = (uint32_t)s_info->no_waveforms;
	sampSet->preTrig = s_info->preTrig;
	sampSet->samplesStoredPerEvent = s_info->samplesStored;
	sampSet->timeIntervalNanoseconds = s_info->timeIntervalNanoseconds;

	info->FillSettings(chSet1,chSet2,sampSet);


	return;

}

void DeviceManager::Run() {

	uint32_t nCompletedCaptures;
	uint32_t nCaptures;
	uint32_t capture;
//	int32_t timeIndisposed;
	int16_t channel;
	int16_t ***rapidBuffers;
	int16_t *overflow;

	int32_t& samplesStored = s_info->samplesStored;
	int32_t& preTrigger = s_info->preTrig;
	int32_t postTrigger = samplesStored-preTrigger;

	uint64_t index = 0;

	int64_t	*times;
	PS5000A_TIME_UNITS *timeUnits;

	uint64_t& durata = s_info->durata_s;
	uint64_t& nMaxSignals = s_info->nMaxSignals;

	uint64_t elapsedTime = 0;
	uint64_t waveformPerBlock = 0;
	uint64_t waveformStored = 0;

	auto begin = chrono::high_resolution_clock::now();
	chrono::time_point<std::chrono::system_clock> now = begin;
	auto nowOld = begin;
	auto dur = now - begin;
	auto deltaT = now - begin; // differenza di tempo tra due riempimenti dei tree
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
	auto interval = chrono::seconds(durata);
	auto autoSaveInterval = chrono::seconds(1200);

	cout << "\nAcquisition starting\n";
	cout << "MAX RUNNING TIME (s):\t" << durata << endl;
	cout << "MAX NUMBER OF SIGNALS:\t" << nMaxSignals << endl;
	cout << "AutoSave every 1200 s\n";

	PICO_STATUS status;

	while (index < nMaxSignals && dur < interval) {

		int16_t retry = 0;
		do
		{
	//		 Inizializza il sistema
			status = ps5000aRunBlock(unit->handle, preTrigger, postTrigger, s_info->timebase, NULL, 0, callBackBlock, NULL);

			if(status != PICO_OK)
			{
				if(status == PICO_POWER_SUPPLY_CONNECTED || status == PICO_POWER_SUPPLY_NOT_CONNECTED)
				{
					status = changePowerSource(status);
					retry = 1;
				}
				else
				{
					cout << "ERROR RAPID BLOCK MODE!" << endl;
					exit(0);
				}
			}

		}
		while( retry ==  1);

		g_ready = false;

		printf("Press any key to abort\n");
		while(!g_ready && !_kbhit() && dur < interval)
		{
	//		printf("SONO NEL LOOP!\n");
			Sleep(0);
			now = chrono::high_resolution_clock::now();
			dur = now - begin;
		}

		deltaT = now - nowOld;
		nowOld = now;
		ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
		elapsedTime = ms;
		cout << "Elapsed time (ms):	" << ms << endl;

		if(!g_ready)
		{
			printf("\nRapid capture aborted.\n");
			status = ps5000aGetNoOfCaptures(unit->handle, &nCompletedCaptures);
			printf("%i complete blocks were captured\n", nCompletedCaptures);
			nMaxSignals = 0;
//			printf("\nPress any key to continue...\n\n");
//			getchar();

			//Only display the blocks that were captured
			nCaptures = (uint32_t)nCompletedCaptures;
		}
		else {
			status = ps5000aGetNoOfCaptures(unit->handle, &nCompletedCaptures);
			nCaptures = (uint32_t)nCompletedCaptures;

			printf("Rapid capture ended. %i complete blocks were captured\n", nCompletedCaptures);
		}

		if (nCompletedCaptures > 0) {

			waveformPerBlock = (uint64_t)nCompletedCaptures;

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
						rapidBuffers[channel][capture] = (int16_t *) calloc(samplesStored, sizeof(int16_t));
					}
				}
			}

			for (channel = 0; channel < unit->channelCount; channel++)
			{
				if(unit->channelSettings[channel].enabled)
				{
					for (capture = 0; capture < nCaptures; capture++)
					{
						status = ps5000aSetDataBuffer(unit->handle, (PS5000A_CHANNEL)channel, rapidBuffers[channel][capture], samplesStored, capture, PS5000A_RATIO_MODE_NONE);
					}
				}
			}

			//Get data
			uint32_t samplesGot = samplesStored;
			status = ps5000aGetValuesBulk(unit->handle, &samplesGot, 0, nCaptures - 1, 1, PS5000A_RATIO_MODE_NONE, overflow);

			if (status == PICO_POWER_SUPPLY_CONNECTED || status == PICO_POWER_SUPPLY_NOT_CONNECTED)
			{
				printf("\nPower Source Changed. Data collection aborted.\n");
				closeDevice();
				if (file->IsOpen()) file->Close();
				exit(0);
			}

			status = ps5000aGetValuesTriggerTimeOffsetBulk64(unit->handle, times, timeUnits, 0, nCaptures - 1);

			//Stop
			status = ps5000aStop(unit->handle);


			if (status == PICO_OK)
			{

				for (capture = 0; capture < nCaptures; capture++)
				{

					for (channel = 0; channel < unit->channelCount; channel++)
					{
						
						if(unit->channelSettings[channel].enabled)
						{

//							for(int32_t i = 0; i < samplesStored-1; i++)
//							{
////								volts.sample[i] = (float)adc_to_mv(rapidBuffers[channel][capture][i], unit->channelSettings[PS5000A_CHANNEL_A +channel].range, unit);
//								volts.sample[i] = 1.*rapidBuffers[channel][capture][i];
//								if (i>100 && i<300) cout <<i << "\t" << volts.sample[i] << endl;
//							}
						  evt->FillEvent(index,times[capture+channel*nCaptures],timeUnits[capture+channel*nCaptures],rapidBuffers[channel][capture],channel);
//							evt->PrintEvent(0,400);
//							instant.time = (int64_t)times[capture+channel*nCaptures]; // Trigger instant
//							instant.timeUnit = (int64_t)timeUnits[capture+channel*nCaptures]; // Time unit of trigger instant
						}
					}
					treeEvt->Fill();
					index++;
				}
			}

			waveformStored = index;
			cout << "NUMBER OF WAVEFORMS\t" << waveformStored << endl;
			evt->FillRTI(waveformPerBlock,elapsedTime,waveformStored);
			treeRTI->Fill();
			if (deltaT < autoSaveInterval) {
				treeEvt->AutoSave();
				treeRTI->AutoSave();
			}

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

//	file->Write();
//	file->Print()
//	file->Close();

	cout << "Captured " << index << " waveforms\n";
	cout << "Elapsed time:	" << ms << endl << endl;


}

int8_t DeviceManager::YN_check() {

	int8_t ch = '.';

	while(ch != 'Y' && ch != 'N' && ch != 'X') {
		printf("Insert correct option (Y/N or X to exit): ");
		ch = toupper(getchar());
		if (ch == 'X')	{
			// exit key
			if (unit->openStatus == PICO_OK) {
				printf("Closing the device...");
				closeDevice();
			}
			printf("Bye!");
			if (file->IsOpen()) file->Close();
			exit(0);
		}
	}

	return ch;
}

