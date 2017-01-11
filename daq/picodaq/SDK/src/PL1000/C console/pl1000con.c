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
 *   PicoLog 1000 series driver.
 *
 * There are the following examples:
 *    Collect a block of samples immediately
 *    Collect a block of samples when a trigger event occurs
 *    Use windowing to collect a sequence of overlapped blocks
 *    Write a continuous stream of data to a disk file
 *    Take individual readings
 *		Set PWM
 *		Set ditial outputs
 *
 * To build this application
 *	set up a project for a 32-bit console mode application
 *	add this file, pl1000Api.h, picoStatus.h and 
 *	pl1000.lib (Microsoft C only) to the project and then
 *	build it
 *
 ***************************************************************************/

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "picoStatus.h"
#include "pl1000Api.h"

#define TRUE		1
#define FALSE		0

#define MAX_BLOCK_SIZE 8192

int32_t		scale_to_mv = TRUE;
uint16_t	max_adc_value;
int16_t		g_handle;
int16_t		isReady;
int16_t		d0State, d1State, d2State, d3State = 0;
PICO_STATUS status;

/****************************************************************************
 *
 * adc_to_mv
 *
 * If the user selects scaling to millivolts,
 * Convert a 10- or 12-bit ADC count into millivolts
 *
 ****************************************************************************/
int32_t adc_to_mv (int32_t raw)										
{		
	int32_t	scaled;

	if (scale_to_mv)
	{
		scaled = raw * 2500 / max_adc_value;
	}
	else
	{
		scaled = raw;
	}

	return scaled;
}

/****************************************************************************
 *
 * mv_to_adc
 *
 * Convert a millivolt value into an 10-bit ADC count
 *
 *  (useful for setting trigger thresholds)
 *
 ****************************************************************************/
int32_t mv_to_adc (int32_t mv)
{
	return mv * max_adc_value / 2500;
}

void printChannelsHeader(FILE * fp, int16_t * channels, int16_t numChannels)
{
	int16_t i;

	if(fp != NULL)
	{
		for(i = 0; i < numChannels; i++)
		{
			fprintf (fp, "Ch%3d\t", channels[i]);
		}

		fprintf(fp, "\n");
	}
}

/****************************************************************************
 *
 * Collect_block_immediate
 *  this function demonstrates how to collect a single block of data
 *  from the unit (start collecting immediately)
 *
 ****************************************************************************/
void collect_block_immediate (void)
{
	uint32_t	i;
	uint16_t	j;
	int16_t		channels [] = {1, 2};
	uint32_t	nSamples = 1000; // Should be equal to nChannels * nSamplesPerChannel
	int16_t		nChannels = 2;
	uint32_t	nSamplesPerChannel = 500;
	uint32_t	nSamplesCollected;
	uint16_t	samples[1000] = {0};	// Size of array should be equal to nChannels * nSamplesPerChannel
	uint32_t	usForBlock = 1000000;	// 1s
	uint16_t 	overflow = 0;
	uint32_t	triggerIndex = 0;
	FILE *		fp;

	printf ("Collect immediate block ...\n");
	printf ("Press a key to start\n");
	_getch();

	//Set the trigger (disabled)
	status = pl1000SetTrigger(g_handle, FALSE, 0, 0, 0, 0, 0, 0, 0);
	
	//set sampling rate and channels
	status = pl1000SetInterval(g_handle, &usForBlock, nSamples, channels, nChannels);

	fopen_s(&fp, "pl1000_block.txt", "w");

	if(fp != NULL)
	{
		printChannelsHeader(fp, channels, nChannels);
	}
	
	//Run
	status = pl1000Run(g_handle, nSamples, BM_SINGLE);

	//Wait until unit is ready
	isReady = 0; 
		
	while(isReady == 0)
	{
		status = pl1000Ready(g_handle, &isReady);
	}

	nSamplesCollected = nSamplesPerChannel;

	status = pl1000GetValues(g_handle, samples, &nSamplesCollected, &overflow, &triggerIndex);

	//Print out the first 10 readings, converting the readings to mV if required
	printf ("First 10 readings of %i\n\n", nSamplesCollected);

	for (i = 0; i < 10; i++)
	{
		printf ("%d\n", adc_to_mv(samples[i]));
	}
		
	for (i = 0; i < nSamplesCollected * nChannels; i = i + nChannels)
	{
		for(j = 0; j < nChannels; j = j + 1)
		{
			fprintf (fp, "%5d\t", adc_to_mv(samples[i + j]));
		}

		fprintf(fp, "\n");
	}
		

	printf("\n");


	fclose(fp);
	status = pl1000Stop(g_handle);

}

/****************************************************************************
 *
 * Collect_block_triggered
 *  this function demonstrates how to collect a single block of data from the
 *  unit, when a trigger event occurs.
 *
 ****************************************************************************/
void collect_block_triggered (void)
{
	uint32_t	i;
	int16_t		channels [] = {1};
	uint32_t	nSamples = MAX_BLOCK_SIZE;
	int16_t		nChannels = 1;
	uint32_t	nSamplesPerChannel = MAX_BLOCK_SIZE;
	uint32_t	nSamplesCollected;
	uint16_t	samples[MAX_BLOCK_SIZE] = {0};
	uint32_t	usForBlock = 12000;
	uint16_t 	overflow = 0;
	uint32_t	triggerIndex = 0;
	FILE *		fp;

	printf ("Collect block triggered...\n");
	printf ("Collects when value rises past 1 V\n");
	printf ("Press a key to start...\n");
	_getch();

	//Set the trigger
	status = pl1000SetTrigger(g_handle, TRUE, 0, 0, PL1000_CHANNEL_1, 0, mv_to_adc(1000), 0, -50);
	
	//set sampling rate and channels
	status = pl1000SetInterval(g_handle, &usForBlock, nSamples, channels, nChannels);

	printf ("Trigger delay is set to -50%% (trigger event in centre of block)\n");
	printf ("\nWaiting for trigger...\n\n");
	printf ("Press a key to abort\n");

	fopen_s(&fp, "pl1000_triggered_block.txt", "w");

	if(fp != NULL)
	{
		printChannelsHeader(fp, channels, nChannels);
	}

	//Run
	status = pl1000Run(g_handle, nSamples, BM_SINGLE);

	//Wait until unit is ready
	isReady = 0;
	
	while(isReady == 0 && (!_kbhit ()))
	{
		status = pl1000Ready(g_handle, &isReady);
	}

	nSamplesCollected = nSamplesPerChannel;
	status = pl1000GetValues(g_handle, samples, &nSamplesCollected, &overflow, &triggerIndex);

	//Print out the first 10 readings, converting the readings to mV if required
	printf ("5 readings either side of trigger event (%i samples collected)\n\n", nSamplesCollected);
	
	for (i = triggerIndex - 5; i < triggerIndex + 6; i++)
	{
		printf ("%d\n", adc_to_mv(samples[i]));
	}
	
	for (i = 0; i < nSamplesCollected; i++)
	{
		fprintf (fp, "%d\n", adc_to_mv(samples[i]));
	}

	printf("\n");

	fclose(fp);
	status = pl1000Stop(g_handle);
}


/****************************************************************************
 *
 * Collect_windowed_blocks
 *
 * This function demonstrates how to use windowed blocks.
 *
 ****************************************************************************/
void collect_windowed_blocks (void)
{
	uint32_t i;
	int16_t	channels [] = {1};
	uint32_t nSamples = 1000;
	int16_t nChannels = 1;
	uint32_t nSamplesPerChannel = 1000;
	uint32_t nSamplesCollected;
	uint16_t samples[1000];
	uint32_t usForBlock = 10000000;	//10 seconds
	uint16_t *overflow = 0;
	uint32_t triggerIndex = 0;
	int16_t nLines = 0;
	FILE *fp;

	printf ("Collect windowed block...\n");
	printf ("First block appears after 10 seconds,\n");
	printf ("then 10 second blocks are collected every second\n");
	printf ("Press a key to start\n");
	_getch();

	//Set the trigger (disabled)
	status = pl1000SetTrigger(g_handle, FALSE, 0, 0, 0, 0, 0, 0, 0);
	
	//set sampling rate and channels
	status = pl1000SetInterval(g_handle, &usForBlock, nSamples, channels, nChannels);

	//Start streaming
	status = pl1000Run(g_handle, nSamples, BM_WINDOW);

	//Wait until unit is ready
	printf ("Waiting for first block...\n");
	isReady = 0;
	
	while(isReady == 0)
	{
		status = pl1000Ready(g_handle, &isReady);
	}

	printf("Press any key to stop\n");
	fopen_s(&fp, "pl1000_windowed_blocks.txt", "w");
  
	while(!_kbhit())
	{
		nSamplesCollected = nSamplesPerChannel;
		status = pl1000GetValues(g_handle, samples, &nSamplesCollected, overflow, &triggerIndex);

		printf("%d values\n", nSamplesCollected);
		
		if(nLines == 20)
		{
				printf("Press any key to stop\n");
				nLines = 0;
		}
		else
				nLines++;

		for (i = 0; i < nSamplesCollected; i++)
		{
			fprintf (fp, "%d\n", adc_to_mv(samples[i]));
		}

		Sleep(1000);		// Wait 1 second before collecting next 10 second block.
	}
	fclose(fp);
	status = pl1000Stop(g_handle);

	_getch();
}

/****************************************************************************
 *
 * Collect_streaming
 * 
 * This function demonstrates how to use streaming.
 *
 ****************************************************************************/

void collect_streaming (void)
{
	uint32_t	i;
	int16_t		channels [] = {1};
	uint32_t	nSamples = MAX_BLOCK_SIZE;
	int16_t		nChannels = 1;
	uint32_t	nSamplesPerChannel = MAX_BLOCK_SIZE / nChannels;
	uint32_t	nSamplesCollected;
	uint16_t	samples[MAX_BLOCK_SIZE];
	uint32_t	usForBlock = 1000000;
	uint16_t *	overflow = 0;
	uint32_t	triggerIndex = 0;
	int16_t		nLines = 0;
	FILE *		fp;
	  
	printf ("Collect streaming...\n");
	printf ("Data is written to disk file (test.out)\n");
	printf ("Press a key to start\n");
	_getch();
		
	//Set the trigger (disabled)
	status = pl1000SetTrigger(g_handle, FALSE, 0, 0, 0, 0, 0, 0, 0);
	
	//set sampling rate and channels
	status = pl1000SetInterval(g_handle, &usForBlock, nSamples, channels, nChannels);

	//Start streaming
	status = pl1000Run(g_handle, nSamples, BM_STREAM);

	//Wait until unit is ready
	isReady = 0;
	
	while(isReady == 0)
	{
		status = pl1000Ready(g_handle, &isReady);
	}

	printf("Press any key to stop\n");
	fopen_s(&fp, "test.out", "w");
  
	while(!_kbhit())
	{
		nSamplesCollected = nSamplesPerChannel;
		status = pl1000GetValues(g_handle, samples, &nSamplesCollected, overflow, &triggerIndex);

		printf("%d values\n", nSamplesCollected);

		if(nLines == 20)
		{
			printf("Press any key to stop\n");
			nLines = 0;
		}
		else
		{
			nLines++;
		}

		for (i = 0; i < nSamplesCollected; i++)
		{
			fprintf (fp, "%d\n", adc_to_mv(samples[i]));
		}

		Sleep(100);
	}
	fclose(fp);
	status = pl1000Stop(g_handle);

	_getch();
}

/****************************************************************************
 *
 * Collect_individual
 *
 ****************************************************************************/
void collect_individual (void)
{
	int32_t		sample_no;
	int32_t		c;
	int16_t		value;
  
	printf ("Collect individual...\n");
	printf ("Takes individual readings under program control\n");
	printf ("Sample from all channels\n");
	printf ("Press a key to start\n");
	_getch();

	sample_no = 20;
	
	while (!_kbhit ())
	{
		Sleep (100);
		
		if (++sample_no > 20)
		{
			sample_no = 0;
			printf ("Press any key to stop\n");
			for (c = 1; c <= 16; c++)
			{
			printf ("ch%02d ", c);
			}
			printf ("\n");
		}
		
		for (c = 1; c <= 16; c++)
		{
			value = 0;
			pl1000GetSingle(g_handle, c, &value);
			printf ("%5d", adc_to_mv (value));
		}
		
		printf ("\n");
	}
	
	_getch ();
}

/****************************************************************************
 *
 * Set/clear digital outputs
 *
 ****************************************************************************/
void outputToggle(PL1000_DO_CH doChannel)
{
	switch(doChannel)
	{
		case PL1000_DO_CHANNEL_0:
		{
			if(d0State == 0)
			{
				d0State = 1;
			}
			else
			{
				d0State = 0;
			}
			
			pl1000SetDo(g_handle, d0State, doChannel);
			break;
		}
		case PL1000_DO_CHANNEL_1:
		{
			if(d1State == 0)
			{
				d1State = 1;
			}
			else
			{
				d1State = 0;
			}
						
			pl1000SetDo(g_handle, d1State, doChannel);
			break;
		}
		case PL1000_DO_CHANNEL_2:
		{
			if(d2State == 0)
			{
				d2State = 1;
			}
			else
			{
				d2State = 0;
			}
			
			pl1000SetDo(g_handle, d2State, doChannel);
			break;
		}
		case PL1000_DO_CHANNEL_3:
		{
			if(d3State == 0)
			{
				d3State = 1;
			}
			else
			{
				d3State = 0;
			}

			pl1000SetDo(g_handle, d3State, doChannel);
			break;
		}
	}
}

/****************************************************************************
 *
 * Display digital output states
 *
 ****************************************************************************/
void displayOutputStates()
{
		printf("\nDigital Outputs\n");
		printf("===============\n");
		printf("D0\tD1\tD2\tD3\t\n");
		printf("%i\t%i\t%i\t%i\t\n\n", d0State, d1State, d2State, d3State);
}

/****************************************************************************
 *
 * Set pulse width modulation
 *
 ****************************************************************************/
void pwm()
{
	int32_t period = 0;
	int32_t cycle = 0;

	printf("\n----------PWM----------\n");
	do
	{
		printf("Enter period (100 to 1800 microseconds):");
		scanf_s("%d", &period);
		fflush(stdin);
	}while(period < 100 || period > 1800);

	do
	{
		printf("Enter duty cycle (0 to 100%%):");
		scanf_s("%d", &cycle);
		fflush(stdin);
	}while(cycle < 0 || cycle > 100);
		
	pl1000SetPulseWidth(g_handle, (uint16_t) period, (uint8_t) cycle);
}

/****************************************************************************
 *
 * Main function
 *
 ****************************************************************************/

void main (void)
{
	int8_t	ch;
	int8_t	info[20];
	int16_t	requiredSize=0;

	printf ("pl1000 driver example program\n");
	printf ("Version 1.1\n\n");

	printf ("\nOpening the device...\n");
	status = pl1000OpenUnit(&g_handle);
	
	if (status != 0)
	{
		printf ("Unable to open device\nPress any key\n");
		_getch();
		exit(99);
	}
	else
	{
		//Set all digital o/ps to zero
		pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_0);
		pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_1);
		pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_2);
		pl1000SetDo(g_handle, 0, PL1000_DO_CHANNEL_3);

		printf ("Device opened successfully\n\n");
		status = pl1000GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_VARIANT_INFO);
		printf("Model:\t\t\t %s\n", info);

		status = pl1000GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_BATCH_AND_SERIAL);
		printf("Serial Number:\t\t %s\n", info);
		
		status = pl1000GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_CAL_DATE);
		printf("Calibration Date:\t %s\n\n", info);
		
		status = pl1000GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_USB_VERSION);
		printf("%s\n", info);
		
		status = pl1000GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_HARDWARE_VERSION);
		printf("Hardware version %s\n", info);
		
		status = pl1000GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_DRIVER_VERSION);
		printf("pl1000.dll version %s\n", info);
		
		status = pl1000GetUnitInfo(g_handle, info, 20, &requiredSize, PICO_KERNEL_VERSION);
		printf("%s\n", info);
		
		pl1000MaxValue(g_handle, &max_adc_value);  

		ch = ' ';

		while (ch != 'X')
		{
			printf ("\n");
			printf ("Select an operation\n");
			printf ("B - Immediate block\t\tA - toggle ADC/mV\n");
			printf ("T - Triggered block\t\tP - set PWM\n");
			printf ("W - Windowed block\t\tD - display digital output states\n");
			printf ("S - Streaming\t\t\t0,1,2,3 - toggle digital output\n");
			printf ("I - Individual reading\t\tX - exit\n");
			ch = toupper (_getch());
			printf ("\n");

			switch (ch)
			{
				case 'B':
					collect_block_immediate ();
					break;

				case 'T':
					collect_block_triggered ();
					break;

				case 'W':
					collect_windowed_blocks ();
					break;

				case 'S':
					collect_streaming ();
					break;

				case 'I':
					collect_individual ();
					break;
				
				case 'P':
					pwm();
					break;
				
				case 'D':
					displayOutputStates();
					break;

				case '0':
					outputToggle(PL1000_DO_CHANNEL_0);
					displayOutputStates();
					break;

				case '1':
					outputToggle(PL1000_DO_CHANNEL_1);
					displayOutputStates();
					break;

				case '2':
					outputToggle(PL1000_DO_CHANNEL_2);
					displayOutputStates();
					break;

				case '3':
					outputToggle(PL1000_DO_CHANNEL_3);
					displayOutputStates();
					break;

				case 'A':
					scale_to_mv = !scale_to_mv;

					if (scale_to_mv)
					{
						printf ("Values will be displayed in mV\n");
					}
					else
					{
						printf ("Values will be displayed in ADC counts\n");
					}
					break;

				case 'X':
				/* Handled by outer loop */
				break;

				default:
				printf ("Invalid operation\n");
				break;
			}
		}
		
		pl1000CloseUnit(g_handle);
	}
}
