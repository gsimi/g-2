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
 *   USB PT-104 driver.
 *
 * Examples:
 *    How to set up the channels
 *    How to collect data via both USB and ethernet connections
 *    How to enable ethernet and set the unit's IP address
 *
 *	To build this application
 *		Windows: set up a project for a 32-bit console mode application
 *			 add this file to the project
 *			 add UsbPT104.lib to the project (Microsoft C only)
 *			 add UsbPT104Api.h and picoStatus.h to the project
 *			 then build the project
 **************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <windows.h>

#include "UsbPt104Api.h"

#define NUM_CHANNELS 4

typedef struct 
{
	USBPT104_DATA_TYPES measurementType;
	short								noWires;
}PT104ChannelSettings;

PT104ChannelSettings channelSettings[NUM_CHANNELS];
short g_handle;
PICO_STATUS g_status;

//Routine to allow the user to change channel settings
void ChannelSetUp()
{
	short channel;
	int	wires;
	int	type;

	printf("0:\tOFF\n");
	printf("1:\tPT100\n");
	printf("2:\tPT1000\n");
	printf("3:\tResistance to 375 Ohms\n");
	printf("4:\tResistance to 10 kOhms\n");
	printf("5:\tDifferential voltage to 115 mV\n");
	printf("6:\tDifferential voltage to 2500 mV\n");
	printf("7:\tSingle-ended voltage to 115 mV\n");
	printf("8:\tSingle-ended voltage to 2500 mV\n\n");

	g_status = PICO_OK;

	for(channel = 0; channel < NUM_CHANNELS && g_status == PICO_OK; channel++)
	{
		printf("channel %d\n", channel + 1);
		do
		{
			printf("Enter measurement type:", channel);
			scanf_s("%d", &type);
		}while(type < 0 || type > 8);

		channelSettings[channel].measurementType = type;

		//If channel is OFF, we don't need to set the number of wires
		if(type == 0) continue;

		do
		{
			printf("Enter number of wires:", channel);
			scanf_s("%d", &wires);
		}while(wires < 2 || wires > 4);

		channelSettings[channel].noWires = wires;
	}
}

char *MeasurementTypeToString(USBPT104_DATA_TYPES measurementType)
{
	switch(measurementType)
	{
	case USBPT104_OFF:
		return "OFF";
	case USBPT104_PT100:
		return "PT100";
	case USBPT104_PT1000:
		return "PT1000";
	case USBPT104_RESISTANCE_TO_375R:
		return "Resistance to 375 Ohms";
	case USBPT104_RESISTANCE_TO_10K:
		return "Resistance to 10 kOhms";
	case USBPT104_DIFFERENTIAL_TO_115MV:
		return "Differential voltage to 115 mV";
	case USBPT104_DIFFERENTIAL_TO_2500MV:
		return "Differential voltage to 2500 mV";
	case USBPT104_SINGLE_ENDED_TO_115MV:
		return "Single-ended voltage to 115 mV";
	case USBPT104_SINGLE_ENDED_TO_2500MV:
		return "Single-ended voltage to 2500 mV";
	default:
		return "ERROR";
	}
}

//Convert values to degrees C, Ohms or millivolts
double ApplyScaling(long value, short channel)
{
	switch(channelSettings[channel].measurementType)
	{
	case USBPT104_OFF:
		return 0;
	case USBPT104_PT100:
		return value / 1000.0;
	case USBPT104_PT1000:
		return value / 1000.0;
	case USBPT104_RESISTANCE_TO_375R:
		return value / 1000000.0;
	case USBPT104_RESISTANCE_TO_10K:
		return value / 1000.0;
	case USBPT104_DIFFERENTIAL_TO_115MV:
		return value / 1000000.0;
	case USBPT104_DIFFERENTIAL_TO_2500MV:
		return value / 100000.0;
	case USBPT104_SINGLE_ENDED_TO_115MV:
		return value / 1000000.0;
	case USBPT104_SINGLE_ENDED_TO_2500MV:
		return value / 100000.0;
	default:
		return -1;
	}
}

void CollectData()
{
	short channel;
	long values[NUM_CHANNELS];
	double scaledValues[NUM_CHANNELS];

	g_status = PICO_OK;

	//Display channel settings
	printf("\n");
	printf("Settings:\n\n");
	
	for(channel = 0; channel < NUM_CHANNELS; channel++)
	{
		printf("Channel %d\n", channel + 1);
		printf("Measurement Type: %s\n", MeasurementTypeToString(channelSettings[channel].measurementType));
		printf("Number of Wires: %d\n\n", channelSettings[channel].noWires);
	}


	//Set channels
	for(channel = 0; channel < NUM_CHANNELS && g_status == PICO_OK; channel++)
	{
		g_status = UsbPt104SetChannel(	g_handle, 
																		channel + 1, 
																		channelSettings[channel].measurementType, 
																		channelSettings[channel].noWires);
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
			g_status = UsbPt104GetValue(g_handle, (USBPT104_CHANNELS)channel + 1, &values[channel], 0);
				
			if(g_status == PICO_NO_SAMPLES_AVAILABLE) values[channel] = 0;

			scaledValues[channel] = ApplyScaling(values[channel], channel);
		}

		printf("%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", scaledValues[0], scaledValues[1], scaledValues[2], scaledValues[3]);
		Sleep(1000);
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
	g_status = UsbPt104IpDetails(g_handle, &enabled, &IPAddress, &length, &port, IDT_GET);

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
		
		g_status = UsbPt104IpDetails(g_handle, &enabled, &IPAddress, &length, &port, IDT_SET);

		if(g_status != PICO_OK)
		{
			printf("IP details: Status = 0x%X", g_status);
			return;
		}
	}
}

void main()
{
	short validSelection = 0;
	short USB;
	char IPAddress[20], ch, info[20];
	short channel, requiredSize;

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
		g_status = UsbPt104OpenUnit(&g_handle, NULL);
	}
	else
	{
		//Get IP address
		printf("Enter IP address of the PT-104: ");
		gets_s(IPAddress, sizeof(IPAddress));

		g_status = UsbPt104OpenUnitViaIp(&g_handle, NULL, IPAddress);
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
		channelSettings[channel].measurementType	= USBPT104_PT100;
		channelSettings[channel].noWires					= 4;
	}

	printf("USB PT-104 Opened.\n");

	//Get unit serial number
	g_status = UsbPt104GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_BATCH_AND_SERIAL);
	printf("%s\n", info);

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
	UsbPt104CloseUnit(g_handle);
}
