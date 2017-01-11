/**************************************************************************
 *
 *     ooooooooo.    o8o                       
 *     `888   `Y88.  `''                       
 *      888   .d88' oooo   .ooooo.   .ooooo.   
 *      888ooo88P'  `888  d88' `'Y8 d88' `88b  
 *      888          888  888       888   888  
 *      888          888  888   .o8 888   888  
 *     o888o        o888o `Y8bod8P' `Y8bod8P'  
 *
 *
 *    Copyright Pico Technology Ltd 1995-2015
 *
 * 
 *    For sales and general information visit
 *    https://www.picotech.com   https://www.picoauto.com
 *    
 *    For help and support visit
 *    https://www.picotech.com/tech-support
 * 
 *    If you have what it takes to join us visit
 *    http://pico.jobs/
 *
 *
 * Description:
 *   This is a console-mode program that demonstrates how to use the
 *   PLCM3 driver.
 *
 * Examples:
 *    How to set up the channels
 *    How to collect data via both USB and ethernet connections
 *    How to enable ethernet and set the unit's IP address
 *
 *	To build this application
 *		Windows: set up a project for a 32-bit console mode application
 *			 add this file to the project
 *			 add PLCM3.lib to the project (Microsoft C only)
 *			 add PLCM3Api.h and picoStatus.h to the project
 *			 then build the project
 *************************************************************************/


#include <stdio.h>
#include <conio.h>
#include <windows.h>

#include "PLCM3Api.h"

#define NUM_CHANNELS 3

typedef struct 
{
	PLCM3_DATA_TYPES measurementType;
}PLCM3ChannelSettings;

PLCM3ChannelSettings channelSettings[NUM_CHANNELS];
short g_handle;
PICO_STATUS g_status;

//Routine to allow the user to change channel settings
void ChannelSetUp()
{
	short channel;
	int	type;

	printf("0:\tOFF\n");
	printf("1:\t1mV range (1mV/A)\n");
	printf("2:\t10mV range (10mV/A)\n");
	printf("3:\t100mV range (100mV/A)\n");
	printf("4:\tVoltage input\n");
	

	g_status = PICO_OK;

	for(channel = 0; channel < NUM_CHANNELS && g_status == PICO_OK; channel++)
	{
		printf("channel %d\n", channel + 1);
		do
		{
			printf("Enter measurement type: ", channel);
			scanf_s("%d", &type);
		}while(type < 0 || type > 4);

		channelSettings[channel].measurementType = type;
	}
}

char *MeasurementTypeToString(PLCM3_DATA_TYPES measurementType)
{
	switch(measurementType)
	{
	case PLCM3_OFF:
		return "OFF";
	case PLCM3_1_MILLIVOLT:
		return "PLCM3_1MV";
	case PLCM3_10_MILLIVOLTS:
		return "PLCM3_10MV";
	case PLCM3_100_MILLIVOLTS:
		return "PLCM3_100MV";
	case PLCM3_VOLTAGE:
		return "PLCM3_VOLTAGE";
	default:
		return "ERROR";
	}
}


	
	
	
	
//Convert values depending upon measurement type
// values are returned as uV, so /1000 to give mV reading
double ApplyScaling(long value, short channel, char *units)
{
	switch(channelSettings[channel].measurementType)
	{
		case PLCM3_OFF:
			strcpy(units, "");
			return 0;

		case PLCM3_1_MILLIVOLT:
			strcpy(units, "A");
			return value / 1000;

		case PLCM3_10_MILLIVOLTS:
			strcpy(units, "A");
			return value / 1000.0;

		case PLCM3_100_MILLIVOLTS:
			strcpy(units, "mA");
			return value ;

		case PLCM3_VOLTAGE:
			strcpy(units, "mV");
			return value / 1000.0;

		default:
			return -1;
	}
}

void CollectData()
{
	short channel;
	long values[NUM_CHANNELS];
	double scaledValues[NUM_CHANNELS];
	char units[NUM_CHANNELS][10];

	g_status = PICO_OK;

	//Display channel settings
	printf("\n");
	printf("Settings:\n\n");
	
	for(channel = 0; channel < NUM_CHANNELS; channel++)
	{
		printf("Channel %d\n", channel + 1);
		printf("Measurement Type: %s\n", MeasurementTypeToString(channelSettings[channel].measurementType));
	}


	//Set channels
	for(channel = 0; channel < NUM_CHANNELS && g_status == PICO_OK; channel++)
	{
		g_status = PLCM3SetChannel(	g_handle, 
									channel + 1, 
									channelSettings[channel].measurementType);
	}

	if(g_status != PICO_OK)
	{
		printf("\n\nSetChannel: Status = 0x%X\nPress any key", g_status);
		_getch();
		return;
	}

	printf("Press any key to start.\n\n");
	_getch();

	printf("Press any key to stop...\n");

	while(!_kbhit() && (g_status == PICO_OK || g_status == PICO_NO_SAMPLES_AVAILABLE))
	{
		for(channel = 0; channel < NUM_CHANNELS && (g_status == PICO_OK || g_status == PICO_NO_SAMPLES_AVAILABLE); channel++)
		{
			g_status = PLCM3GetValue(g_handle, (PLCM3_CHANNELS)channel + 1, &values[channel]);
				
			if(g_status == PICO_NO_SAMPLES_AVAILABLE) values[channel] = 0;

			scaledValues[channel] = ApplyScaling(values[channel], channel, units[channel]);
		}

		for(channel = 0; channel < NUM_CHANNELS; channel++)
		{
			printf("%.2f%s\t\t", scaledValues[channel], units[channel]);
			
			printf(channel==NUM_CHANNELS-1?"\n":"");
			Sleep(1000);
		}
	}
		
	if(g_status != PICO_OK && g_status != PICO_NO_SAMPLES_AVAILABLE)
		printf("\n\nGetValue: Status = 0x%X\nPress any key", g_status);

	_getch();
}

void EthernetSettings()
{
	int tmp;
	short enabled;
	char IPAddress[20], ch;
	unsigned short length, port;

	//Display current ethernet settings
	g_status = PLCM3IpDetails(g_handle, &enabled, &IPAddress, &length, &port, PLCM3_IDT_GET);

	if(g_status != PICO_OK)
	{
		printf("IP details: Status = 0x%X", g_status);
		return;
	}

	printf("\nEthernet Settings\n\n");
	printf("Enabled:\t%d\n", enabled);
	printf("IP Address:\t%s\n", IPAddress);
	printf("Port:\t\t%d\n", port);

	//Enter new settings
	printf("\nEdit settings? (Y/N)\n");
	ch = toupper(_getch());

	if(ch == 'Y')
	{
		printf("Enable ethernet? (Y/N)\n");
		ch = toupper(_getch());
		if(ch == 'N')
		{
			enabled = 0;
		}
		else
		{
			enabled = 1;
			
			fflush(stdin);
			printf("Enter IP address: ");
			gets_s(IPAddress, sizeof(IPAddress));
			length = sizeof(IPAddress);

			printf("Enter port: ");
			scanf_s("%d", &tmp);
			port = (unsigned short)tmp;
		}
		
		g_status = PLCM3IpDetails(g_handle, &enabled, &IPAddress, &length, &port, PLCM3_IDT_SET);

		if(g_status != PICO_OK)
		{
			printf("IP details: Status = 0x%X", g_status);
			return;
		}
	}
}

// **************************
// Read the units EEPROM data
// **************************
void get_info(int g_handle)
{
	int line;
	short requiredSize;
	char info[20];

	char description[7][25] = { "Driver Version    :", 
								"USB Version       :", 
								"Hardware Version  :", 
								"Variant Info      :", 
								"Batch and Serial  :", 
								"Calibration Date  :", 
								"Kernel Driver Ver.:"};

	

	for (line = 0; line < 7; line++)
	{
		g_status = PLCM3GetUnitInfo(g_handle, info, 20, &requiredSize, (PICO_INFO)line);
		printf("\n%s ", description[line]);
		printf("%s", info);
	}
	printf("\n");
}

void main()
{
	short validSelection = 0;
	short USB;
	char IPAddress[20], ch;
	short channel;

	//User must select USB or Ethernet before the deivce is opened
	do
	{
		printf("\n\n");
		printf("Select connection:\n");
		printf("U:\tUSB\n");
		printf("E:\tEthernet\n\n");

		ch = toupper(_getch());

		switch(ch)
		{
		case 'U':
			USB = 1;
			validSelection = 1;
			break;

		case 'E':
			USB = 0;
			validSelection = 1;
			break;

		default:
			printf("Invalid input.\n");
		}

	}while(!validSelection);


	//Open unit
	if(USB)
	{
		g_status = PLCM3OpenUnit(&g_handle, NULL);
	}
	else
	{
		//Get IP address
		printf("Enter IP address of the PLCM3: ");
		gets_s(IPAddress, sizeof(IPAddress));

		g_status = PLCM3OpenUnitViaIp(&g_handle, NULL, IPAddress);
	}
	
	if(g_status != PICO_OK)
	{
		printf("Unable to open device. Status code: 0x%X", g_status);
		_getch();
		return;
	}

	//Set default channel settings
	for(channel = 0; channel < NUM_CHANNELS; channel++)
	{
		channelSettings[channel].measurementType	= PLCM3_1_MILLIVOLT;
	}

	printf("PLCM3 Opened.\n");

	//Get unit serial number
	get_info(g_handle);
	

	if(g_status != PICO_OK)
	{
		printf("Error. Status code: 0x%X", g_status);
		_getch();
		return;
	}

	do
	{
		printf("\n\n");
		printf("S:\tStart Aquisition\n");
		printf("C:\tChannel Settings\n");
		printf("E:\tEthernet Settings\n");
		printf("X:\tExit\n\n");

		ch = toupper(_getch());

		switch(ch)
		{
		case 'S':
			CollectData();
			break;

		case 'C':
			ChannelSetUp();
			break;

		case 'E':
			if(!USB)
			{
				printf("Connect via USB to set up ethernet.");
				break;
			}
			EthernetSettings();
			break;

		case 'X':
			break;

		default:
			printf("Invalid input.\n");
		}

	}while(ch != 'X');

	//Close unit
	PLCM3CloseUnit(g_handle);
}
