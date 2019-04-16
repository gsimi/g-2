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
 *   PS5000 driver.
 *
 * Examples:
 *    Collect a block of samples immediately
 *    Collect a block of samples when a trigger event occurs
 *    Collect a block using ETS
 *    Collect a stream of data immediately
 *    Collect a stream of data when a trigger event occurs
 *    Set Signal Generator, using built in or custom signals
 *
 * To build this application
 *	set up a project for a 32-bit console mode application
 *	add this file to the project
 *	add PS5000bc.lib to the project (Borland C only)
 *	add PS5000.lib to the project (Microsoft C only)
 *	build the project
 *
 * History:
 *		30Oct06	MJL	Created
 *		15Mar10	RPM	Added Rapid Block Mode
 *		07Apr10	RPM	Added Logic Triggering Example
 *		15Aug14	HSM Modified solution, change to get_info function		
 *
 * Revision Info: "file %n date %f revision %v"
 *						""
 *
 ***************************************************************************/

#include "windows.h"
#include <conio.h>
#include <stdio.h>

#include "ps5000Api.h"

#define BUFFER_SIZE 	1024
#define MAX_CHANNELS 4
#define QUAD_SCOPE 4
#define DUAL_SCOPE 2

typedef struct
{
	short DCcoupled;
	PS5000_RANGE range;
	short enabled;
}CHANNEL_SETTINGS;

typedef enum
{
	MODEL_NONE = 0,
	MODEL_PS5203 = 5203,
	MODEL_PS5204 = 5204
} MODEL_TYPE;

typedef struct tTriggerDirections
{
	enum enThresholdDirection channelA;
	enum enThresholdDirection channelB;
	enum enThresholdDirection channelC;
	enum enThresholdDirection channelD;
	enum enThresholdDirection ext;
	enum enThresholdDirection aux;
}TRIGGER_DIRECTIONS;

typedef struct tPwq
{
  PWQ_CONDITIONS * conditions;
	short nConditions;
	enum enThresholdDirection direction;
	unsigned long lower;
	unsigned long upper;
	enum enPulseWidthType type;
}PWQ;

typedef struct
{
	short handle;
	MODEL_TYPE model;
	PS5000_RANGE firstRange;
	PS5000_RANGE lastRange;
	unsigned char signalGenerator;
	unsigned char external;
	short ChannelCount;
	CHANNEL_SETTINGS channelSettings [MAX_CHANNELS];
	PS5000_RANGE triggerRange;
}UNIT_MODEL;

unsigned long timebase = 8;
short       oversample = 1;
int      scaleVoltages = TRUE;

unsigned short inputRanges [PS5000_MAX_RANGES] = { 10,
		                                           20,
		                                           50,
		                                           100,
		                                           200,
		                                           500,
		                                           1000,
		                                           2000,
		                                           5000,
		                                           10000,
		                                           20000,
		                                           50000 };
short     g_ready = FALSE;
long long g_times [PS5000_MAX_CHANNELS];
short     g_timeUnit;
long      g_sampleCount;
unsigned long g_startIndex;
short         g_autoStop;

/****************************************************************************
 * Callback
 * used by PS5000 data streaimng collection calls, on receipt of data.
 * used to set global flags etc checked by user routines
 ****************************************************************************/
void __stdcall CallBackStreaming
(
		short handle,
		long noOfSamples,
		unsigned long startIndex, //
		short overflow,
		unsigned long triggerAt,
		short triggered,
		short autoStop,
		void * pParameter
)
{
	// used for streaming
	g_sampleCount = noOfSamples;
	g_startIndex = startIndex;
	g_autoStop = autoStop;

	// flag to say done reading data
	g_ready = TRUE;
}

/****************************************************************************
 * Callback
 * used by PS5000 data block collection calls, on receipt of data.
 * used to set global flags etc checked by user routines
 ****************************************************************************/
void __stdcall CallBackBlock
(
		short handle,
		PICO_STATUS status,
		void * pParameter
)
{
	// flag to say done reading data
	g_ready = TRUE;
}

/****************************************************************************
 * SetDefaults - restore default settings
 ****************************************************************************/
void SetDefaults(UNIT_MODEL * unit)
{
	int i;

	ps5000SetEts(unit->handle, PS5000_ETS_OFF, 0, 0, NULL); // Turn off ETS

	for (i = 0; i < unit->ChannelCount; i++) // reset channels to most recent settings
	{
		ps5000SetChannel(unit->handle, 
                     (PS5000_CHANNEL) (PS5000_CHANNEL_A + i),
		                 unit->channelSettings[PS5000_CHANNEL_A + i].enabled,
		                 unit->channelSettings[PS5000_CHANNEL_A + i].DCcoupled,
		                 unit->channelSettings[PS5000_CHANNEL_A + i].range);
	}
}

/****************************************************************************
 * adc_to_mv
 *
 * If the user selects scaling to millivolts,
 * Convert an 16-bit ADC count into millivolts
 ****************************************************************************/
int adc_to_mv(long raw, int ch)
{
	return ( scaleVoltages ) ? ( raw * inputRanges[ch] ) / PS5000_MAX_VALUE : raw;
}

/****************************************************************************
 * mv_to_adc
 *
 * Convert a millivolt value into a 16-bit ADC count
 *
 *  (useful for setting trigger thresholds)
 ****************************************************************************/
short mv_to_adc(short mv, short ch)
{
	return ( ( mv * PS5000_MAX_VALUE ) / inputRanges[ch] );
}

/****************************************************************************
 * BlockDataHandler
 * - Used by all block data routines except rapid block.
 * - acquires data (user sets trigger mode before calling), displays 10 items
 *   and saves all to data.txt
 * Input :
 * - unit : the unit to use.
 * - text : the text to display before the display of data slice
 * - offset : the offset into the data buffer to start the display's slice.
 ****************************************************************************/
void BlockDataHandler(UNIT_MODEL * unit, char * text, int offset)
{
	int i, j;
	long timeInterval;
	long sampleCount= BUFFER_SIZE;
	FILE * fp;
	long maxSamples;
	short * buffers[PS5000_MAX_CHANNEL_BUFFERS * 2];
	long timeIndisposed;
	PICO_STATUS status;

	for (i = 0; i < unit->ChannelCount; i++) {
		buffers[i * 2] = (short*)malloc(sampleCount * sizeof(short));
		buffers[i * 2 + 1] = (short*)malloc(sampleCount * sizeof(short));
		ps5000SetDataBuffers(unit->handle, (PS5000_CHANNEL)i, buffers[i * 2], buffers[i * 2 + 1], sampleCount);
	}

	/*  find the maximum number of samples, the time interval (in timeUnits),
	 *		 the most suitable time units, and the maximum oversample at the current timebase*/
	while (ps5000GetTimebase(unit->handle, timebase, sampleCount,
	                         &timeInterval, oversample,
	                         &maxSamples, 0))
	{
		timebase++;
	}
	printf("timebase: %hd\toversample:%hd\n", timebase, oversample);

	/* Start it collecting, then wait for completion*/
	g_ready = FALSE;
	status = ps5000RunBlock(unit->handle, 0, sampleCount, timebase, oversample,
	                        &timeIndisposed, 0, CallBackBlock, NULL);
	if (status != PICO_OK)
	{
		printf("Immediately Block Mode: failed to call run_block successfully \n");
		return;
	}
	/*printf("Run Block : %i", status);*/
	printf("Waiting for trigger...Press a key to abort\n");

	while (!g_ready && !_kbhit())
	{
		Sleep(0);
	}

	if (g_ready) {
		ps5000GetValues(unit->handle, 0, (unsigned long*) &sampleCount, 1, RATIO_MODE_NONE, 0, NULL);

		/* Print out the first 10 readings, converting the readings to mV if required */
		printf(text);
		printf("Value (%s)\n", ( scaleVoltages ) ? ("mV") : ("ADC Counts"));

		for (i = offset; i < offset+10; i++) {
			for (j = 0; j < unit->ChannelCount; j++) {
				if (unit->channelSettings[j].enabled) {
					printf(
					       "%d\t",
					       adc_to_mv(buffers[j * 2][i],
					                 unit->channelSettings[PS5000_CHANNEL_A + j].range));
				}
			}
			printf("\n");
		}

		sampleCount = __min(sampleCount, BUFFER_SIZE);

		fopen_s(&fp, "data.txt", "w");
		for (i = 0; i < sampleCount; i++) {
			for (j = 0; j < unit->ChannelCount; j++) {
				fprintf(fp, "%ld ", g_times[j] + (long long)(i * timeInterval));
				if (unit->channelSettings[j].enabled) {
					fprintf(fp,", %d, %d, %d, %d", 
					        buffers[j * 2][i], 
					        adc_to_mv(buffers[j * 2][i], 
					                  unit->channelSettings[PS5000_CHANNEL_A + j].range),
					        buffers[j * 2 + 1][i], 
					        adc_to_mv(buffers[j * 2 + 1][i], 
					                  unit->channelSettings[PS5000_CHANNEL_A + j].range));
				}
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	} else {
		printf("data collection aborted\n");
		_getch();
	}

	ps5000Stop(unit->handle);

	for (i = 0; i < unit->ChannelCount * 2; i++) {
		free(buffers[i]);
	}
}

PICO_STATUS SetTrigger(short handle,
                       TRIGGER_CHANNEL_PROPERTIES * channelProperties,
                       short nChannelProperties,
                       TRIGGER_CONDITIONS * triggerConditions,
                       short nTriggerConditions,
                       TRIGGER_DIRECTIONS * directions,
                       PWQ * pwq,
                       unsigned long delay,
                       short auxOutputEnabled,
                       long autoTriggerMs)
{
	PICO_STATUS status;

	if ((status = ps5000SetTriggerChannelProperties(handle,
					channelProperties,
					nChannelProperties,
					auxOutputEnabled,
					autoTriggerMs)) != PICO_OK) {
		return status;
	}

	if ((status = ps5000SetTriggerChannelConditions(handle,
					(TRIGGER_CONDITIONS*)triggerConditions,
					nTriggerConditions)) != PICO_OK) {
		return status;
	}

	if ((status = ps5000SetTriggerChannelDirections(handle,
					directions->channelA,
					directions->channelB,
					directions->channelC,
					directions->channelD,
					directions->ext,
					directions->aux)) != PICO_OK) {
		return status;
	}

	if ((status = ps5000SetTriggerDelay(handle, delay)) != PICO_OK) {
		return status;
	}

	status = ps5000SetPulseWidthQualifier(handle, pwq->conditions,
	                                      pwq->nConditions, pwq->direction,
	                                      pwq->lower, pwq->upper, pwq->type);

	return status;
}

/****************************************************************************
 * CollectBlockImmediate
 *  this function demonstrates how to collect a single block of data
 *  from the unit (start collecting immediately)
 ****************************************************************************/
void CollectBlockImmediate(UNIT_MODEL * unit)
{
	struct tPwq pulseWidth;
	struct tTriggerDirections directions;

	memset(&directions, 0, sizeof(struct tTriggerDirections));
	memset(&pulseWidth, 0, sizeof(struct tPwq));

	printf("Collect block immediate...\n");
	printf("Press a key to start\n");
	_getch();

	SetDefaults(unit);

	/* Trigger disabled	*/
	SetTrigger(unit->handle, NULL, 0, NULL, 0, &directions, &pulseWidth, 0, 0, 0);

	BlockDataHandler(unit, "First 10 readings\n", 0);
}

/****************************************************************************
 * CollectBlockEts
 *  this function demonstrates how to collect a block of
 *  data using equivalent time sampling (ETS).
 ****************************************************************************/

void CollectBlockEts(UNIT_MODEL * unit)
{
	PICO_STATUS status;
	__int64 buffer[BUFFER_SIZE];
	long ets_sampletime;
	short
	        triggerVoltage = mv_to_adc(
	                                   100,
	                                   unit->channelSettings[PS5000_CHANNEL_A].range); // ChannelInfo stores ADC counts
	struct tTriggerChannelProperties sourceDetails = { triggerVoltage,
			                                           triggerVoltage,
			                                           10,
			                                           PS5000_CHANNEL_A,
			                                           LEVEL };
	struct tTriggerConditions conditions = { CONDITION_TRUE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE };
	unsigned long delay = 0;
	struct tPwq pulseWidth;
	struct tTriggerDirections directions;

	memset(&pulseWidth, 0, sizeof(struct tPwq));
	memset(&directions, 0, sizeof(struct tTriggerDirections));
	directions.channelA = RISING;

	printf("Collect ETS block...\n");
	printf("Collects when value rises past %dmV\n",
	       adc_to_mv(sourceDetails.thresholdMajor,
	                 unit->channelSettings[PS5000_CHANNEL_A].range));
	printf("Press a key to start...\n");
	_getch();

	SetDefaults(unit);

	/* Trigger enabled
	 * Rising edge
	 * Threshold = 1500mV
	 * 10% pre-trigger  (negative is pre-, positive is post-) */
	status = SetTrigger(unit->handle, &sourceDetails, 1, &conditions, 1,
	                    &directions, &pulseWidth, delay, 0, 0);

	/* printf("Set Trigger : %x" , status); */

	/* Enable ETS in fast mode, the computer will store 100 cycles but interleave only 10 */
	status = ps5000SetEts(unit->handle, PS5000_ETS_FAST, 20, 4, &ets_sampletime);
	/*printf("Set ETS : %x" , status);*/
	printf("ETS Sample Time is: %ld\n", ets_sampletime);

	//Set ETS times buffer
	status = ps5000SetEtsTimeBuffer(unit->handle, buffer, BUFFER_SIZE);

	BlockDataHandler(unit, "Ten readings after trigger\n", BUFFER_SIZE / 10 - 5); // 10% of data is pre-trigger
}

/****************************************************************************
 * CollectBlockTriggered
 *  this function demonstrates how to collect a single block of data from the
 *  unit, when a trigger event occurs.
 ****************************************************************************/
void CollectBlockTriggered(UNIT_MODEL * unit)
{
	short
	        triggerVoltage = mv_to_adc(
	                                   100,
	                                   unit->channelSettings[PS5000_CHANNEL_A].range); // ChannelInfo stores ADC counts
	struct tTriggerChannelProperties sourceDetails = { triggerVoltage,
			                                           triggerVoltage,
			                                           256 * 10,
			                                           PS5000_CHANNEL_A,
			                                           LEVEL };
	struct tTriggerConditions conditions = { CONDITION_TRUE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE };
	struct tPwq pulseWidth;
	struct tTriggerDirections directions = { RISING,
			                                 NONE,
			                                 NONE,
			                                 NONE,
			                                 NONE,
			                                 NONE };
	memset(&pulseWidth, 0, sizeof(struct tPwq));

	printf("Collect block triggered...\n");
	printf("Collects when value rises past %dmV\n",
	       adc_to_mv(sourceDetails.thresholdMajor,
	                 unit->channelSettings[PS5000_CHANNEL_A].range));
	printf("Press a key to start...\n");
	_getch();

	SetDefaults(unit);

	/* Trigger enabled
	 * Rising edge
	 * Threshold = 100mV */
	SetTrigger(unit->handle, &sourceDetails, 1, &conditions, 1, &directions,
	           &pulseWidth, 0, 0, 0);

	BlockDataHandler(unit, "Ten readings after trigger\n", 0);
}

/****************************************************************************
 * CollectBlockLogicTriggered
 *  this function demonstrates how to collect a single block of data from the
 *  unit using logic triggering (Trigger on channel A OR channel B)
 ****************************************************************************/
void CollectBlockLogicTriggered(UNIT_MODEL * unit)
{
	short
	        triggerVoltage = mv_to_adc(
	                                   100,
	                                   unit->channelSettings[PS5000_CHANNEL_A].range); // ChannelInfo stores ADC counts
	//Set properties for both channels A and B
	TRIGGER_CHANNEL_PROPERTIES sourceDetails[2] = {{ triggerVoltage,
			                                           triggerVoltage,
			                                           256 * 10,
			                                           PS5000_CHANNEL_A,
			                                           LEVEL},
																								 {triggerVoltage,
			                                           triggerVoltage,
			                                           256 * 10,
			                                           PS5000_CHANNEL_B,
																								 LEVEL }};
	//Trigger on channel A OR channel B
	TRIGGER_CONDITIONS conditions[2] = {{CONDITION_TRUE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE },
																			 { CONDITION_DONT_CARE,
			                                 CONDITION_TRUE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
																			 CONDITION_DONT_CARE }};
	PWQ pulseWidth;
	TRIGGER_DIRECTIONS directions = { RISING,
			                                 RISING,
			                                 NONE,
			                                 NONE,
			                                 NONE,
			                                 NONE };
	memset(&pulseWidth, 0, sizeof(struct tPwq));

	printf("Collect block logic triggering.\n");
	printf("Trigger on channel A OR channel B.\n");
	printf("Press a key to start...\n");
	_getch();

	SetDefaults(unit);

	SetTrigger(unit->handle, sourceDetails, 2, conditions, 2, &directions,
	           &pulseWidth, 0, 0, 0);

	BlockDataHandler(unit, "Ten readings after trigger\n", 0);
}

/****************************************************************************
 * CollectRapidBlock
 *  this function demonstrates how to use rapid block mode.
 ****************************************************************************/
void CollectRapidBlock(UNIT_MODEL * unit)
{
	long timeInterval;
	long maxSamples;
	long sampleCount = BUFFER_SIZE;
	PICO_STATUS status;
	long timeIndisposed;
	short **rapidBuffers;
	short * overflow;
	int i, j, ch, block;
	long nSamples;
	FILE * fp;

	short nCaptures = 10;		//The number of blocks we wish to capture

	printf("Data are written to Rapid Block.txt...\n");
	printf("Press a key to start...\n");
	_getch();

	//Set fastest possible timebase
	while (ps5000GetTimebase(unit->handle, timebase, sampleCount,
	                         &timeInterval, oversample,
	                         &maxSamples, 0))
	{
		timebase++;
	}

	status = ps5000MemorySegments(unit->handle, nCaptures, &nSamples);

	status = ps5000SetNoOfCaptures(unit->handle, nCaptures);

	status = ps5000RunBlock(unit->handle, 0, sampleCount, timebase, oversample,
	                        &timeIndisposed, 0, CallBackBlock, NULL);

	rapidBuffers = (short**)malloc(nCaptures * sizeof(short*) * unit->ChannelCount);
	overflow = (short *) malloc(nCaptures * sizeof(short)); 

	//Set data buffers for rapid block mode
	i=0;
	for (ch = 0; ch < unit->ChannelCount; ch++) 
	{
		for(block = 0; block < nCaptures; block++)
		{
			rapidBuffers[i] = (short*)malloc(sampleCount * sizeof(short));
			status = ps5000SetDataBufferBulk(unit->handle, ch, rapidBuffers[i], sampleCount, block);
			i++;
		}
	}

	while (!g_ready && !_kbhit())
	{
		Sleep(0);
	}

	if(g_ready)
	{
		status = ps5000GetValuesBulk(unit->handle, &sampleCount, 0, nCaptures-1, overflow);

		//Write buffers to a text file
		fopen_s(&fp, "Rapid Block.txt", "w");
			
		fprintf(fp,"Each column is one block of data\n\n"); 

		for(j = 0; j < sampleCount; j++)
		{
			for(i = 0; i < unit->ChannelCount * nCaptures; i++)
			{
				fprintf(fp,"\t%d,", rapidBuffers[i][j]); 
			}
			fprintf(fp,"\n"); 
		}
		
		fclose(fp);
	}

	
	//Free buffers
	i=0;
	for (ch = 0; ch < unit->ChannelCount; ch++) 
	{
		for(block = 0; block < nCaptures; block++)
		{
			free(rapidBuffers[i]);	
			i++;
		}

	}
	free(rapidBuffers);
	free(overflow);
}

/****************************************************************************
 * Initialise unit' structure with Variant specific defaults
 ****************************************************************************/
void get_info(UNIT_MODEL * unit)
{
	char description [7][25]= { "Driver Version",
			                    "USB Version",
			                    "Hardware Version",
			                    "Variant Info",
			                    "Serial",
			                    "Cal Date",
								"Kernel"};
	short i, r = 0;
	char line [80];
	int variant;

	PICO_STATUS status;

	if (unit->handle) 
	{
		for (i = 0; i < 7; i++) 
		{
			status = ps5000GetUnitInfo(unit->handle, line, sizeof (line), &r, i);
			//printf("Get the unit info: status = %d \n\n\n", status);
			
			if (i == 3) 
			{
				variant = atoi(line);
			}

			printf("%s: %s\n", description[i], line);
		}

		switch (variant) 
		{
			case MODEL_PS5203:
				unit->model = MODEL_PS5203;
				unit->external = TRUE;
				unit->signalGenerator = TRUE;
				unit->firstRange = PS5000_100MV;
				unit->lastRange = PS5000_20V;
				unit->ChannelCount = DUAL_SCOPE;
				break;
			case MODEL_PS5204:
				unit->model = MODEL_PS5204;
				unit->external = TRUE;
				unit->signalGenerator = TRUE;
				unit->firstRange = PS5000_100MV;
				unit->lastRange = PS5000_20V;
				unit->ChannelCount = DUAL_SCOPE;
				break;
			default:
				break;
		}
	}
}

/****************************************************************************
 * Select input voltage ranges for channels A and B
 ****************************************************************************/
void set_voltages(UNIT_MODEL * unit)
{
	int i;
	int ch;

	/* See what ranges are available... */
	for (i = unit->firstRange; i <= unit->lastRange; i++) {
		printf("%d -> %d mV\n", i, inputRanges[i]);
	}

	/* Ask the user to select a range */
	printf("Specify voltage range (%d..%d)\n", unit->firstRange,
	       unit->lastRange);
	printf("99 - switches channel off\n");
	for (ch = 0; ch < unit->ChannelCount; ch++) {
		printf("\n");
		do {
			printf("Channel %c: ", 'A' + ch);
			fflush(stdin);
			scanf_s("%d", &unit->channelSettings[ch].range);
		} while (unit->channelSettings[ch].range != 99 && (unit->channelSettings[ch].range < unit->firstRange || unit->channelSettings[ch].range > unit->lastRange));

		if (unit->channelSettings[ch].range != 99) {
			printf(" - %d mV\n", inputRanges[unit->channelSettings[ch].range]);
			unit->channelSettings[ch].enabled = TRUE;
		} else {
			printf("Channel Switched off\n");
			unit->channelSettings[ch].enabled = FALSE;
		}
	}
}

/****************************************************************************
 *
 * Select timebase, set oversample to on and time units as nano seconds
 *
 ****************************************************************************/
void SetTimebase(UNIT_MODEL unit)
{
	long timeInterval;
	long maxSamples;

	printf("Specify timebase (not 0): ");
	do {
		fflush(stdin);
		scanf_s("%u", &timebase);
	} while (timebase == 0);

	ps5000GetTimebase(unit.handle, timebase, BUFFER_SIZE, &timeInterval,
	                  1, &maxSamples, 0);
	printf("Timebase %d - %ld ns\n", timebase, timeInterval);
	oversample = TRUE;
}



/****************************************************************************
* Sets the signal generator
* - allows user to set frequency and waveform
* - allows for custom waveform (values 0..4095) of up to 8192 samples long
***************************************************************************/
void SetSignalGenerator(UNIT_MODEL unit)
{
	PICO_STATUS status;
	short waveform;
	long frequency;
	char fileName [128];
	FILE * fp;
	short arbitraryWaveform [8192];
	short waveformSize = 0;
	unsigned long pkpk = 1000000;
	long offset = 0;
	short whitenoise = 0;
	char ch;

	memset(&arbitraryWaveform, 0, 8192);

	while (_kbhit())			// use up keypress
		_getch();


	do
	{
		printf("\nSignal Generator\n================\n");
		printf("0:\tSINE      \t6:\tGAUSSIAN\n");
		printf("1:\tSQUARE    \t7:\tHALF SINE\n");
		printf("2:\tTRIANGLE  \t8:\tDC VOLTAGE\n");
		printf("3:\tRAMP UP   \t9:\tWHITE NOISE\n");
		printf("4:\tRAMP DOWN\n");
		printf("5:\tSINC\n");
		printf("A:\tAWG WAVEFORM\t");
		printf("X:\tSigGen Off\n\n");

		ch = _getch();

		if (ch >= '0' && ch <='9')
			waveform = ch -'0';
		else
			ch = toupper(ch);
	}
	while(ch != 'A' && ch != 'X' && (ch < '0' || ch > '9'));


	if(ch == 'X')				// If we're going to turn off siggen
	{
		printf("Signal generator Off\n");
		waveform = 8;		// DC Voltage
		pkpk = 0;				// 0V
		waveformSize = 0;
	}
	else
	if (ch == 'A')		// Set the AWG
	{
		waveformSize = 0;

		printf("Select a waveform file to load: ");
		scanf_s("%s", fileName, 128);
		if (fopen_s(&fp, fileName, "r") == 0) 
		{ // Having opened file, read in data - one number per line (at most 8192 lines), with values in (0..4095)
				while (EOF != fscanf_s(fp, "%hi", (arbitraryWaveform + waveformSize))&& waveformSize++ < 8192 -1);
			fclose(fp);
			printf("File successfully loaded\n");
		} 
		else 
		{
			printf("Invalid filename\n");
			return;
		}
	}
	else			// Set one of the built in waveforms
	{
		switch (waveform)
		{
			case 8:
			do 
			{
				printf("\nEnter offset in uV: (0 to 2500000)\n"); // Ask user to enter DC offset level;
				scanf_s("%lu", &offset);
			} while (offset < 0 || offset > 10000000);
			break;

			case 9:
				whitenoise = 1;
				break;

			default:
				whitenoise = 0;
				offset = 0;
		}
	}

	if(waveform < 8 || ch == 'A')				// Find out frequency if required
	{
		do 
		{
			printf("\nEnter frequency in Hz: (1 to 20000000)\n"); // Ask user to enter signal frequency;
			scanf_s("%lu", &frequency);
		} while (frequency <= 0 || frequency > 20000000);
	}

	if (waveformSize > 0)		
	{
		double delta = ((1.0 * frequency * waveformSize) / 8192.0) * 4294967296.0 * 8e-9; // delta >= 10
		
		status = ps5000SetSigGenArbitrary(	unit.handle, 
																				0, 
																				1000000, 
																				(unsigned long)delta, 
																				(unsigned long)delta, 
																				0, 
																				0, 
																				arbitraryWaveform, 
																				waveformSize, 
																				(SWEEP_TYPE) 0,
																				0, 
																				SINGLE, 
																				0, 
																				0, 
																				SIGGEN_RISING,
																				SIGGEN_NONE, 
																				0);

		printf(status?"\nps5000SetSigGenArbitrary: Status Error 0x%x \n":"", (unsigned int)status);		// If status != 0, show the error
	} 
	else 
	{
		status = ps5000SetSigGenBuiltIn(unit.handle, offset, pkpk, waveform, (float)frequency, (float)frequency, 0, 0, (SWEEP_TYPE)0, whitenoise, 0, 0, 0, 0, 0);
		printf(status?"\nps5000SetSigGenBuiltIn: Status Error 0x%x \n":"", (unsigned int)status);		// If status != 0, show the error
	}
}

/****************************************************************************
 * Stream Data Handler
 * - Used by the two stream data examples - untriggered and triggered
 * Inputs:
 * - unit - the unit to sample on
 * - preTrigger - the number of samples in the pre-trigger phase 
 *					(0 if no trigger has been set)
 ***************************************************************************/
void StreamDataHandler(UNIT_MODEL * unit, unsigned long preTrigger)
{
	long i, j;
	unsigned long sampleCount= BUFFER_SIZE * 10; /*  *10 is to make sure buffer large enough */
	FILE * fp;
	short * buffers[PS5000_MAX_CHANNEL_BUFFERS];
	PICO_STATUS status;
	unsigned long sampleInterval = 1;

	for (i = 0; i < unit->ChannelCount; i++) // create data buffers
	{
		buffers[i * 2] = (short*) malloc(sampleCount * sizeof(short));
		buffers[i * 2 + 1] = (short*)malloc(sampleCount * sizeof(short));
		ps5000SetDataBuffers(unit->handle, (PS5000_CHANNEL) i, buffers[i * 2],
		                     buffers[i * 2 + 1], sampleCount);
	}

	printf("Waiting for trigger...Press a key to abort\n");
	g_autoStop = FALSE;
	status = ps5000RunStreaming(unit->handle, &sampleInterval, PS5000_US,
	                            preTrigger, 1000000 - preTrigger, TRUE, 1000,
	                            sampleCount);
	if (status != PICO_OK)
	{
		printf("Streaming Mode: fail to call run_streaming successfully \n");
		return;
	}

	/* printf("Run Streaming : %i \n", status); */

	printf("Streaming data...Press a key to abort\n");

	fopen_s(&fp, "test.out", "w");
	while (!_kbhit() && !g_autoStop)
	{
		/* Poll until data is received. Until then, GetStreamingLatestValues wont call the callback */
		Sleep(100);
		g_ready = FALSE;
		status = ps5000GetStreamingLatestValues(unit->handle,
		                                        (ps5000StreamingReady)CallBackStreaming, NULL);

		if (g_ready && g_sampleCount > 0) /* can be ready and have no data, if autoStop has fired */
		{
			printf("Collected %i samples, index = %i\n", g_sampleCount,
			       g_startIndex);

			for (i = g_startIndex; i < (long)(g_startIndex + g_sampleCount); i++) {
				for (j = 0; j < unit->ChannelCount; j++) {
					if (unit->channelSettings[j].enabled) {
						fprintf(
						        fp,
						        "%d, %d, %d, %d,",
						        buffers[j * 2][i],
						        adc_to_mv(
						                  buffers[j * 2][i],
						                  unit->channelSettings[PS5000_CHANNEL_A + j].range),
						        buffers[j * 2 + 1][i],
						        adc_to_mv(
						                  buffers[j * 2 + 1][i],
						                  unit->channelSettings[PS5000_CHANNEL_A + j].range));
					}
				}
				fprintf(fp, "\n");
			}
		}
	}

	ps5000Stop(unit->handle);
	fclose(fp);

	if (!g_autoStop) {
		printf("data collection aborted\n");
		_getch();
	}

	for (i = 0; i < unit->ChannelCount * 2; i++) {
		free(buffers[i]);
	}
}

/****************************************************************************
 * CollectStreamingImmediate
 *  this function demonstrates how to collect a stream of data
 *  from the unit (start collecting immediately)
 ***************************************************************************/
void CollectStreamingImmediate(UNIT_MODEL * unit)
{
	struct tPwq pulseWidth;
	struct tTriggerDirections directions;

	memset(&pulseWidth, 0, sizeof(struct tPwq));
	memset(&directions, 0, sizeof(struct tTriggerDirections));

	SetDefaults(unit);

	printf("Collect streaming...\n");
	printf("Data is written to disk file (test.out)\n");
	printf("Press a key to start\n");
	_getch();

	/* Trigger disabled	*/
	SetTrigger(unit->handle, NULL, 0, NULL, 0, &directions, &pulseWidth, 0, 0,
	           0);

	StreamDataHandler(unit, 0);
}

/****************************************************************************
 * CollectStreamingTriggered
 *  this function demonstrates how to collect a stream of data
 *  from the unit (start collecting on trigger)
 ***************************************************************************/
void CollectStreamingTriggered(UNIT_MODEL * unit)
{
	short triggerVoltage = mv_to_adc(
	                                   100,
	                                   unit->channelSettings[PS5000_CHANNEL_A].range); // ChannelInfo stores ADC counts
	struct tTriggerChannelProperties sourceDetails = { triggerVoltage,
			                                           triggerVoltage,
			                                           256 * 10,
			                                           PS5000_CHANNEL_A,
			                                           LEVEL };
	struct tTriggerConditions conditions = { CONDITION_TRUE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE,
			                                 CONDITION_DONT_CARE };
	struct tPwq pulseWidth;
	struct tTriggerDirections directions = { RISING,
			                                 NONE,
			                                 NONE,
			                                 NONE,
			                                 NONE,
			                                 NONE };
	memset(&pulseWidth, 0, sizeof(struct tPwq));

	printf("Collect streaming triggered...\n");
	printf("Data is written to disk file (test.out)\n");
	printf("Press a key to start\n");
	_getch();
	SetDefaults(unit);

	/* Trigger enabled
	 * Rising edge
	 * Threshold = 100mV */

	SetTrigger(unit->handle, &sourceDetails, 1, &conditions, 1, &directions,
	           &pulseWidth, 0, 0, 0);

	StreamDataHandler(unit, 100000);
}

int FlashLed(UNIT_MODEL * unit)
{
	PICO_STATUS status;
	printf("Flash led ......\n");
	printf("Press a key to start\n");
	_getch(); 
	
	
	status = ps5000FlashLed(unit->handle, 3);
	printf("Flashing led: status = %d \n", status);
	if (status != PICO_OK)
		printf("Failed to flash the led: status = %d \n", status);
	else
		printf("Succeed to flash the led: status = %d \n", status);

	Sleep(2000);
	
	return 1;
}
/****************************************************************************
 *
 *
 ****************************************************************************/
int main(void)
{
	char ch;
	int i;
	PICO_STATUS status;
	UNIT_MODEL unit;

	printf("PS5000 driver example program\n");
	printf("Version 1.0\n\n");

	printf("\n\nOpening the device...\n");

	//open unit
	status = ps5000OpenUnit(&(unit.handle));
	printf("Handle: %d\n", unit.handle);
	if (status != PICO_OK) {
		printf("Unable to open device\n");
		printf("Error code : %d\n", status);
		while (!_kbhit())
			;
		exit( 99); // exit program - nothing after this executes
	}

	printf("Device opened successfully\n\n");

	// setup devices
	get_info(&unit);
	timebase = 1;

	for (i = 0; i < MAX_CHANNELS; i++) {
		unit.channelSettings[i].enabled = TRUE;
		unit.channelSettings[i].DCcoupled = TRUE;
		unit.channelSettings[i].range = PS5000_5V;
	}

	// main loop - read key and call routine
	ch = ' ';
	while (ch != 'X')
	{
		printf("\n");
		printf("B - Immediate block\t\tV - Set voltages\n");
		printf("T - Triggered block\t\tI - Set timebase\n");
		printf("R - Immediate rapid block\tQ - Logic triggering block\n");
		printf("E - ETS block\t\t\tF - toggle signal generator on/off\n");
		printf("S - Immediate streaming\t\tA - ADC counts/mV\n");
		printf("W - Triggered streaming\n");
		printf("L - Flash the led\t\tU - Get unit info \n");
		printf("                                X - exit\n");
		printf("Operation:");

		ch = toupper(_getch());

		printf("\n\n");
		switch (ch) {
			case 'B':
				CollectBlockImmediate(&unit);
				break;

			case 'T':
				CollectBlockTriggered(&unit);
				break;

			case 'Q':
				CollectBlockLogicTriggered(&unit);
				break;

			case 'S':
				CollectStreamingImmediate(&unit);
				break;

			case 'W':
				CollectStreamingTriggered(&unit);
				break;

			case 'R':
				CollectRapidBlock(&unit);
				break;

			case 'F':
				SetSignalGenerator(unit);
				break;

			case 'E':
				CollectBlockEts(&unit);
				break;

			case 'V':
				set_voltages(&unit);
				break;

			case 'I':
				SetTimebase(unit);
				break;

			case 'L':
				FlashLed(&unit);
				break;

			case 'U':
				printf("Get units info -- press any key to start \n");
				_getch();
				get_info(&unit);
				break;

			case 'A':
				scaleVoltages = !scaleVoltages;
				if (scaleVoltages) {
					printf("Readings will be scaled in mV\n");
				} else {
					printf("Readings will be scaled in ADC counts\n");
				}
				break;

			case 'X':
				/* Handled by outer loop */
				break;

			default:
				printf("Invalid operation\n");
				break;
		}
	}

	ps5000CloseUnit(unit.handle);
}
