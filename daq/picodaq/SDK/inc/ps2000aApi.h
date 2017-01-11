/****************************************************************************
 *
 * Filename:    ps2000aApi.h
 * Copyright:   Pico Technology Limited 2010
 * Author:      MAS
 * Description:
 *
 * This header defines the interface to driver routines for the 
 *	PicoScope2000a series of PC Oscilloscopes.
 *
 ****************************************************************************/
#ifndef __PS2000AAPI_H__
#define __PS2000AAPI_H__

#include <stdint.h>

#include "PicoStatus.h"

#ifdef __cplusplus
	#define PREF0 extern "C"
	#define TYPE_ENUM
#else
	#define PREF0
	#define TYPE_ENUM enum
#endif

#ifdef WIN32
	#ifdef PREF1
	  #undef PREF1
	#endif
	#ifdef PREF2
	  #undef PREF2
	#endif
	#ifdef PREF3
	  #undef PREF3
	#endif
	/*	If you are dynamically linking PS2000A.DLL into your project #define DYNLINK here
	 */
	#ifdef DYNLINK
	  #define PREF1 typedef
		#define PREF2
		#define PREF3(x) (__stdcall *x)
	#else
	  #define PREF1
		#ifdef _USRDLL
			#define PREF2 __declspec(dllexport) __stdcall
		#else
			#define PREF2 __declspec(dllimport) __stdcall
		#endif
	  #define PREF3(x) x
	#endif
#else
	/* Define a 64-bit integer type */

	#ifdef DYNLINK
		#define PREF1 typedef
		#define PREF2
		#define PREF3(x) (*x)
	#else
		#ifdef _USRDLL
			#define PREF1 __attribute__((visibility("default")))
		#else
			#define PREF1
		#endif
		#define PREF2
		#define PREF3(x) x
	#endif
	#define __stdcall
#endif

/* Depending on the adc; oversample (collect multiple readings at each time) by up to 256.
 * the results are therefore ALWAYS scaled up to 16-bits, even if
 * oversampling is not used.
 *
 * The maximum and minimum values returned are therefore as follows:
 */

// covers the PS2208 and PS2208A
#define PS2208_MAX_ETS_CYCLES	500
#define PS2208_MAX_INTERLEAVE	 20

// covers the PS2207 and PS2207A
#define PS2207_MAX_ETS_CYCLES	500
#define PS2207_MAX_INTERLEAVE	 20

// covers the PS2206 and PS2206A
#define PS2206_MAX_ETS_CYCLES	250
#define PS2206_MAX_INTERLEAVE	 10

#define PS2000A_EXT_MAX_VALUE  32767
#define PS2000A_EXT_MIN_VALUE -32767

#define PS2000A_MAX_LOGIC_LEVEL	 32767
#define PS2000A_MIN_LOGIC_LEVEL -32767

#define MIN_SIG_GEN_FREQ 0.0f
#define MAX_SIG_GEN_FREQ 20000000.0f

#define PS2000A_MAX_SIG_GEN_BUFFER_SIZE 8192
#define PS2000A_MIN_SIG_GEN_BUFFER_SIZE 1
#define PS2000A_MIN_DWELL_COUNT				3
#define PS2000A_MAX_SWEEPS_SHOTS				((1 << 30) - 1)

#define PS2000A_MAX_ANALOGUE_OFFSET_50MV_200MV	 0.250f
#define PS2000A_MIN_ANALOGUE_OFFSET_50MV_200MV	-0.250f
#define PS2000A_MAX_ANALOGUE_OFFSET_500MV_2V		 2.500f
#define PS2000A_MIN_ANALOGUE_OFFSET_500MV_2V		-2.500f
#define PS2000A_MAX_ANALOGUE_OFFSET_5V_20V			 20.f
#define PS2000A_MIN_ANALOGUE_OFFSET_5V_20V			-20.f

// supported by the PS2206/PS2206A, PS2207/PS2207A, PS2208/PS2208A only
#define PS2000A_SHOT_SWEEP_TRIGGER_CONTINUOUS_RUN 0xFFFFFFFF

typedef enum enPS2000AChannelBufferIndex
{
	PS2000A_CHANNEL_A_MAX,
	PS2000A_CHANNEL_A_MIN,
	PS2000A_CHANNEL_B_MAX,
	PS2000A_CHANNEL_B_MIN,
	PS2000A_CHANNEL_C_MAX,
	PS2000A_CHANNEL_C_MIN,
	PS2000A_CHANNEL_D_MAX,
	PS2000A_CHANNEL_D_MIN,
	PS2000A_MAX_CHANNEL_BUFFERS
} PS2000A_CHANNEL_BUFFER_INDEX;

typedef enum enPS2000AChannel
{
	PS2000A_CHANNEL_A,
	PS2000A_CHANNEL_B,
	PS2000A_CHANNEL_C,
	PS2000A_CHANNEL_D,
	PS2000A_EXTERNAL,	
	PS2000A_MAX_CHANNELS = PS2000A_EXTERNAL,
	PS2000A_TRIGGER_AUX,
	PS2000A_MAX_TRIGGER_SOURCES

}	PS2000A_CHANNEL;

typedef enum enPS2000ATriggerOperand
{
	PS2000A_OPERAND_NONE,
	PS2000A_OPERAND_OR,
	PS2000A_OPERAND_AND,
	PS2000A_OPERAND_THEN
} PS2000A_TRIGGER_OPERAND;

typedef enum enPS2000DigitalPort
{
	PS2000A_DIGITAL_PORT0 = 0x80, // digital channel 0 - 7
	PS2000A_DIGITAL_PORT1,			 // digital channel 8 - 15
	PS2000A_DIGITAL_PORT2,			 // digital channel 16 - 23
	PS2000A_DIGITAL_PORT3,			 // digital channel 24 - 31
	PS2000A_MAX_DIGITAL_PORTS = (PS2000A_DIGITAL_PORT3 - PS2000A_DIGITAL_PORT0) + 1
}	PS2000A_DIGITAL_PORT;

typedef enum enPS2000ADigitalChannel
{
	PS2000A_DIGITAL_CHANNEL_0,
	PS2000A_DIGITAL_CHANNEL_1,
	PS2000A_DIGITAL_CHANNEL_2,
	PS2000A_DIGITAL_CHANNEL_3,
	PS2000A_DIGITAL_CHANNEL_4,
	PS2000A_DIGITAL_CHANNEL_5,
	PS2000A_DIGITAL_CHANNEL_6,
	PS2000A_DIGITAL_CHANNEL_7,
	PS2000A_DIGITAL_CHANNEL_8,
	PS2000A_DIGITAL_CHANNEL_9,
	PS2000A_DIGITAL_CHANNEL_10,
	PS2000A_DIGITAL_CHANNEL_11,
	PS2000A_DIGITAL_CHANNEL_12,
	PS2000A_DIGITAL_CHANNEL_13,
	PS2000A_DIGITAL_CHANNEL_14,
	PS2000A_DIGITAL_CHANNEL_15,
	PS2000A_DIGITAL_CHANNEL_16,
	PS2000A_DIGITAL_CHANNEL_17,
	PS2000A_DIGITAL_CHANNEL_18,
	PS2000A_DIGITAL_CHANNEL_19,
	PS2000A_DIGITAL_CHANNEL_20,
	PS2000A_DIGITAL_CHANNEL_21,
	PS2000A_DIGITAL_CHANNEL_22,
	PS2000A_DIGITAL_CHANNEL_23,
	PS2000A_DIGITAL_CHANNEL_24,
	PS2000A_DIGITAL_CHANNEL_25,
	PS2000A_DIGITAL_CHANNEL_26,
	PS2000A_DIGITAL_CHANNEL_27,
	PS2000A_DIGITAL_CHANNEL_28,
	PS2000A_DIGITAL_CHANNEL_29,
	PS2000A_DIGITAL_CHANNEL_30,
	PS2000A_DIGITAL_CHANNEL_31,
	PS2000A_MAX_DIGITAL_CHANNELS
} PS2000A_DIGITAL_CHANNEL;

typedef enum enPS2000ARange
{
	PS2000A_10MV,
	PS2000A_20MV,
	PS2000A_50MV,
	PS2000A_100MV,
	PS2000A_200MV,
	PS2000A_500MV,
	PS2000A_1V,
	PS2000A_2V,
	PS2000A_5V,
	PS2000A_10V,
	PS2000A_20V,
	PS2000A_50V,
	PS2000A_MAX_RANGES,
} PS2000A_RANGE;

typedef enum enPS2000ACoupling
{
	PS2000A_AC,
	PS2000A_DC
} PS2000A_COUPLING;

typedef enum enPS2000AChannelInfo
{
	PS2000A_CI_RANGES,
} PS2000A_CHANNEL_INFO;

typedef enum enPS2000AEtsMode
  {
  PS2000A_ETS_OFF,             // ETS disabled
  PS2000A_ETS_FAST,
	PS2000A_ETS_SLOW,
  PS2000A_ETS_MODES_MAX
  }	PS2000A_ETS_MODE;

typedef enum enPS2000ATimeUnits
  {
  PS2000A_FS,
  PS2000A_PS,
  PS2000A_NS,
  PS2000A_US,
  PS2000A_MS,
  PS2000A_S,
  PS2000A_MAX_TIME_UNITS,
  }	PS2000A_TIME_UNITS;

typedef enum enPS2000ASweepType
{
	PS2000A_UP,
	PS2000A_DOWN,
	PS2000A_UPDOWN,
	PS2000A_DOWNUP,
	PS2000A_MAX_SWEEP_TYPES
} PS2000A_SWEEP_TYPE;

typedef enum enPS2000AWaveType
{
	PS2000A_SINE,
	PS2000A_SQUARE,
	PS2000A_TRIANGLE,
	PS2000A_RAMP_UP,
	PS2000A_RAMP_DOWN,
	PS2000A_SINC,
	PS2000A_GAUSSIAN,
	PS2000A_HALF_SINE,
	PS2000A_DC_VOLTAGE,
	PS2000A_MAX_WAVE_TYPES
} PS2000A_WAVE_TYPE;

typedef enum enPS2000AExtraOperations
{
	PS2000A_ES_OFF,
	PS2000A_WHITENOISE,
	PS2000A_PRBS // Pseudo-Random Bit Stream 
} PS2000A_EXTRA_OPERATIONS;


#define PS2000A_SINE_MAX_FREQUENCY				1000000.f
#define PS2000A_SQUARE_MAX_FREQUENCY			1000000.f
#define PS2000A_TRIANGLE_MAX_FREQUENCY		1000000.f
#define PS2000A_SINC_MAX_FREQUENCY				1000000.f
#define PS2000A_RAMP_MAX_FREQUENCY				1000000.f
#define PS2000A_HALF_SINE_MAX_FREQUENCY		1000000.f
#define PS2000A_GAUSSIAN_MAX_FREQUENCY		1000000.f
#define PS2000A_PRBS_MAX_FREQUENCY				1000000.f
#define PS2000A_PRBS_MIN_FREQUENCY					 0.03f
#define PS2000A_MIN_FREQUENCY			  				 0.03f

typedef enum enPS2000ASigGenTrigType
{
	PS2000A_SIGGEN_RISING,
	PS2000A_SIGGEN_FALLING,
	PS2000A_SIGGEN_GATE_HIGH,
	PS2000A_SIGGEN_GATE_LOW
} PS2000A_SIGGEN_TRIG_TYPE;

typedef enum enPS2000ASigGenTrigSource
{
	PS2000A_SIGGEN_NONE,
	PS2000A_SIGGEN_SCOPE_TRIG,
	PS2000A_SIGGEN_AUX_IN,
	PS2000A_SIGGEN_EXT_IN,
	PS2000A_SIGGEN_SOFT_TRIG
} PS2000A_SIGGEN_TRIG_SOURCE;

typedef enum enPS2000AIndexMode
{
	PS2000A_SINGLE,
	PS2000A_DUAL,
	PS2000A_QUAD,
	PS2000A_MAX_INDEX_MODES
} PS2000A_INDEX_MODE;

typedef enum enPS2000A_ThresholdMode
{
	PS2000A_LEVEL,
	PS2000A_WINDOW
} PS2000A_THRESHOLD_MODE;

typedef enum enPS2000AThresholdDirection
{
	PS2000A_ABOVE, //using upper threshold
	PS2000A_BELOW, //using upper threshold
	PS2000A_RISING, // using upper threshold
	PS2000A_FALLING, // using upper threshold
	PS2000A_RISING_OR_FALLING, // using both threshold
	PS2000A_ABOVE_LOWER, // using lower threshold
	PS2000A_BELOW_LOWER, // using lower threshold
	PS2000A_RISING_LOWER,			 // using lower threshold
	PS2000A_FALLING_LOWER,		 // using lower threshold

	// Windowing using both thresholds
	PS2000A_INSIDE = PS2000A_ABOVE, 
	PS2000A_OUTSIDE = PS2000A_BELOW,
	PS2000A_ENTER = PS2000A_RISING, 
	PS2000A_EXIT = PS2000A_FALLING, 
	PS2000A_ENTER_OR_EXIT = PS2000A_RISING_OR_FALLING,
	PS2000A_POSITIVE_RUNT = 9,
  PS2000A_NEGATIVE_RUNT,

	// no trigger set
  PS2000A_NONE = PS2000A_RISING 
} PS2000A_THRESHOLD_DIRECTION;

typedef enum enPS2000ADigitalDirection
{
	PS2000A_DIGITAL_DONT_CARE,
	PS2000A_DIGITAL_DIRECTION_LOW,	
	PS2000A_DIGITAL_DIRECTION_HIGH,
	PS2000A_DIGITAL_DIRECTION_RISING,
	PS2000A_DIGITAL_DIRECTION_FALLING,
	PS2000A_DIGITAL_DIRECTION_RISING_OR_FALLING,
	PS2000A_DIGITAL_MAX_DIRECTION
} PS2000A_DIGITAL_DIRECTION;

typedef enum enPS2000ATriggerState
{
  PS2000A_CONDITION_DONT_CARE,
  PS2000A_CONDITION_TRUE,
  PS2000A_CONDITION_FALSE,
	PS2000A_CONDITION_MAX
} PS2000A_TRIGGER_STATE;

#pragma pack(1)
typedef struct tPS2000ATriggerConditions
{
  PS2000A_TRIGGER_STATE channelA;
  PS2000A_TRIGGER_STATE channelB;
  PS2000A_TRIGGER_STATE channelC;
  PS2000A_TRIGGER_STATE channelD;
  PS2000A_TRIGGER_STATE external;
  PS2000A_TRIGGER_STATE aux;
	PS2000A_TRIGGER_STATE pulseWidthQualifier;
	PS2000A_TRIGGER_STATE digital;
} PS2000A_TRIGGER_CONDITIONS;
#pragma pack()

#pragma pack(1)
typedef struct tPS2000APwqConditions
{
  PS2000A_TRIGGER_STATE channelA;
  PS2000A_TRIGGER_STATE channelB;
  PS2000A_TRIGGER_STATE channelC;
  PS2000A_TRIGGER_STATE channelD;
  PS2000A_TRIGGER_STATE external;
  PS2000A_TRIGGER_STATE aux;
	PS2000A_TRIGGER_STATE digital;
} PS2000A_PWQ_CONDITIONS;
#pragma pack()

#pragma pack(1)
typedef struct tPS2000ADigitalChannelDirections
{
	PS2000A_DIGITAL_CHANNEL channel;
	PS2000A_DIGITAL_DIRECTION direction;
} PS2000A_DIGITAL_CHANNEL_DIRECTIONS;
#pragma pack()

#pragma pack(1)
typedef struct tPS2000ATriggerChannelProperties
{
  int16_t										thresholdUpper;
	uint16_t					thresholdUpperHysteresis; 
  int16_t										thresholdLower;
	uint16_t					thresholdLowerHysteresis;
	PS2000A_CHANNEL					channel;
  PS2000A_THRESHOLD_MODE	thresholdMode;
} PS2000A_TRIGGER_CHANNEL_PROPERTIES;
#pragma pack()
	
typedef enum enPS2000ARatioMode
{
	PS2000A_RATIO_MODE_NONE,
	PS2000A_RATIO_MODE_AGGREGATE = 1,	
	PS2000A_RATIO_MODE_DECIMATE = 2,
	PS2000A_RATIO_MODE_AVERAGE = 4,
} PS2000A_RATIO_MODE;

typedef enum enPS2000APulseWidthType
{
	PS2000A_PW_TYPE_NONE,
  PS2000A_PW_TYPE_LESS_THAN,
	PS2000A_PW_TYPE_GREATER_THAN,
	PS2000A_PW_TYPE_IN_RANGE,
	PS2000A_PW_TYPE_OUT_OF_RANGE
} PS2000A_PULSE_WIDTH_TYPE;

typedef enum enPS2000AHoldOffType
{
	PS2000A_TIME,
	PS2000A_MAX_HOLDOFF_TYPE
} PS2000A_HOLDOFF_TYPE;

typedef void (__stdcall *ps2000aBlockReady)
	(
		int16_t											handle,
		PICO_STATUS								status,
		void										*	pParameter
	); 

typedef void (__stdcall *ps2000aStreamingReady)
	(
		int16_t    									handle,
		int32_t     									noOfSamples,
		uint32_t							startIndex,
		int16_t    									overflow,
		uint32_t							triggerAt,
		int16_t    									triggered,
		int16_t    									autoStop,
		void										*	pParameter
	); 

typedef void (__stdcall *ps2000aDataReady)
	(
		int16_t    									handle,
		PICO_STATUS								status,
		uint32_t     				noOfSamples,
		int16_t    									overflow,
		void										*	pParameter
	); 

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aOpenUnit) 
  (
	  int16_t											* handle,
		int8_t											* serial
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aOpenUnitAsync)
  (
	  int16_t											* status,
		int8_t											* serial
	);


PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aOpenUnitProgress)
	(
	  int16_t 										* handle,
	  int16_t 										* progressPercent,
	  int16_t 										* complete
	); 

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetUnitInfo)
 	(
	  int16_t     								  handle, 
	  int8_t      								* string,
	  int16_t     								  stringLength,
	  int16_t     								* requiredSize,
	  PICO_INFO 								  info
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aFlashLed)
	(
	  int16_t 											handle,
		int16_t 											start
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aCloseUnit)
	(
	  int16_t												handle
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aMemorySegments)
	(
	  int16_t												handle,
		uint16_t							nSegments,
		int32_t											* nMaxSamples
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetChannel)
 	(
	  int16_t												handle,
		PS2000A_CHANNEL							channel,
	  int16_t												enabled,
	  PS2000A_COUPLING						type, 
		PS2000A_RANGE								range,
		float												analogOffset
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetDigitalPort)
 	(
	  int16_t												handle,
		PS2000A_DIGITAL_PORT				port,
	  int16_t												enabled,
		int16_t									      logicLevel
	);


PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetNoOfCaptures) 
	(
	int16_t handle,
	uint16_t nCaptures
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetTimebase)
	(
	   int16_t											handle,
	   uint32_t							timebase,
	   int32_t												noSamples,
	   int32_t											* timeIntervalNanoseconds,
	   int16_t											oversample,
		 int32_t											* maxSamples,
		 uint16_t							segmentIndex
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetTimebase2)
	(
	   int16_t											handle,
	   uint32_t							timebase,
	   int32_t												noSamples,
	   float										* timeIntervalNanoseconds,
	   int16_t											oversample,
		 int32_t											* maxSamples,
		 uint16_t							segmentIndex
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetSigGenArbitrary)
	(
	 	int16_t												handle,
	 	int32_t												offsetVoltage,
	 	uint32_t								pkToPk,
	 	uint32_t								startDeltaPhase,
	 	uint32_t								stopDeltaPhase,
	 	uint32_t								deltaPhaseIncrement, 
	 	uint32_t								dwellCount,
	 	int16_t											*	arbitraryWaveform, 
	 	int32_t												arbitraryWaveformSize,
		PS2000A_SWEEP_TYPE					sweepType,
		PS2000A_EXTRA_OPERATIONS		operation,
		PS2000A_INDEX_MODE					indexMode,
		uint32_t								shots,
		uint32_t								sweeps,
		PS2000A_SIGGEN_TRIG_TYPE		triggerType,
		PS2000A_SIGGEN_TRIG_SOURCE	triggerSource,
		int16_t												extInThreshold
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3(ps2000aSetSigGenBuiltIn)
	(
		int16_t												handle,
		int32_t												offsetVoltage,
		uint32_t								pkToPk,
		int16_t												waveType,
		float												startFrequency,
		float												stopFrequency,
		float												increment,
		float												dwellTime,
		PS2000A_SWEEP_TYPE					sweepType,
		PS2000A_EXTRA_OPERATIONS		operation,
		uint32_t								shots,
		uint32_t								sweeps,
		PS2000A_SIGGEN_TRIG_TYPE		triggerType,
		PS2000A_SIGGEN_TRIG_SOURCE	triggerSource,
		int16_t												extInThreshold
		);

PREF0 PREF1 PICO_STATUS PREF2 PREF3(ps2000aSetSigGenBuiltInV2)
	(
		int16_t												handle,
		int32_t												offsetVoltage,
		uint32_t								pkToPk,
		int16_t												waveType,
		double												startFrequency,
		double												stopFrequency,
		double												increment,
		double												dwellTime,
		PS2000A_SWEEP_TYPE					sweepType,
		PS2000A_EXTRA_OPERATIONS		operation,
		uint32_t								shots,
		uint32_t								sweeps,
		PS2000A_SIGGEN_TRIG_TYPE		triggerType,
		PS2000A_SIGGEN_TRIG_SOURCE	triggerSource,
		int16_t												extInThreshold
		);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetSigGenPropertiesArbitrary)
	(
	 	int16_t												handle,
	 	uint32_t								startDeltaPhase,
	 	uint32_t								stopDeltaPhase,
	 	uint32_t								deltaPhaseIncrement, 
	 	uint32_t								dwellCount,
		PS2000A_SWEEP_TYPE					sweepType,
		uint32_t								shots,
		uint32_t								sweeps,
		PS2000A_SIGGEN_TRIG_TYPE		triggerType,
		PS2000A_SIGGEN_TRIG_SOURCE	triggerSource,
		int16_t												extInThreshold
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3(ps2000aSetSigGenPropertiesBuiltIn)
	(
		int16_t												handle,
		double												startFrequency,
		double												stopFrequency,
		double												increment,
		double												dwellTime,	
		PS2000A_SWEEP_TYPE					sweepType,
		uint32_t								shots,
		uint32_t								sweeps,
		PS2000A_SIGGEN_TRIG_TYPE		triggerType,
		PS2000A_SIGGEN_TRIG_SOURCE	triggerSource,
		int16_t												extInThreshold
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3(ps2000aSigGenFrequencyToPhase)
	(
	int16_t												handle,
	double												frequency,
	PS2000A_INDEX_MODE						indexMode,
	uint32_t											bufferLength,
	uint32_t										* phase
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3(ps2000aSigGenArbitraryMinMaxValues)
	(
	int16_t			handle,
	int16_t		*	minArbitraryWaveformValue,
	int16_t		*	maxArbitraryWaveformValue,
	uint32_t	* minArbitraryWaveformSize,
	uint32_t	*	maxArbitraryWaveformSize
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSigGenSoftwareControl)
	(
		int16_t												handle,
		int16_t												state
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetEts)
  (
		int16_t												handle,
		PS2000A_ETS_MODE						mode,
		int16_t												etsCycles,
		int16_t												etsInterleave,
		int32_t											* sampleTimePicoseconds
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetSimpleTrigger)
	(
		int16_t handle,
		int16_t enable,
		PS2000A_CHANNEL source,
		int16_t threshold,
		PS2000A_THRESHOLD_DIRECTION direction,
		uint32_t delay,
		int16_t autoTrigger_ms
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetTriggerDigitalPortProperties)
(
	int16_t handle,
	PS2000A_DIGITAL_CHANNEL_DIRECTIONS * directions,
	int16_t															   nDirections
);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetDigitalAnalogTriggerOperand)
(
	int16_t handle,
	PS2000A_TRIGGER_OPERAND operand
);



PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetTriggerChannelProperties)
	(
		int16_t																	handle,
		PS2000A_TRIGGER_CHANNEL_PROPERTIES	*	channelProperties,
		int16_t																	nChannelProperties,
		int16_t																	auxOutputEnable,
		int32_t																	autoTriggerMilliseconds
	);


PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetTriggerChannelConditions)
	(
		int16_t													handle,
		PS2000A_TRIGGER_CONDITIONS	*	conditions,
		int16_t													nConditions
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetTriggerChannelDirections)
	(
		int16_t													handle,
		PS2000A_THRESHOLD_DIRECTION		channelA,
		PS2000A_THRESHOLD_DIRECTION		channelB,
		PS2000A_THRESHOLD_DIRECTION		channelC,
		PS2000A_THRESHOLD_DIRECTION		channelD,
		PS2000A_THRESHOLD_DIRECTION		ext,
		PS2000A_THRESHOLD_DIRECTION		aux
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetTriggerDelay)
	(
		int16_t									handle,
		uint32_t					delay
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetPulseWidthQualifier)
	(
		int16_t													handle,
		PS2000A_PWQ_CONDITIONS			*	conditions,
		int16_t													nConditions,
		PS2000A_THRESHOLD_DIRECTION		direction,
		uint32_t									lower,
		uint32_t									upper,
		PS2000A_PULSE_WIDTH_TYPE			type
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aIsTriggerOrPulseWidthQualifierEnabled)
	(
		int16_t 								handle,
		int16_t 							* triggerEnabled,
		int16_t 							* pulseWidthQualifierEnabled
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetTriggerTimeOffset)
	(
		int16_t									handle,
		uint32_t 			* timeUpper,
		uint32_t 			* timeLower,
		PS2000A_TIME_UNITS	*	timeUnits,
		uint16_t				segmentIndex	
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetTriggerTimeOffset64)
	(
		int16_t									handle,
		int64_t							* time,
		PS2000A_TIME_UNITS	*	timeUnits,
		uint16_t				segmentIndex	
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetValuesTriggerTimeOffsetBulk)
	(
	  int16_t									handle,
		uint32_t				*	timesUpper,
		uint32_t				* timesLower,
		PS2000A_TIME_UNITS	*	timeUnits,
		uint16_t				fromSegmentIndex,
		uint16_t				toSegmentIndex
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetValuesTriggerTimeOffsetBulk64)
	(
	  int16_t									handle,
		int64_t							*	times,
		PS2000A_TIME_UNITS	*	timeUnits,
		uint16_t				fromSegmentIndex,
		uint16_t				toSegmentIndex
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetNoOfCaptures)
	(
	  int16_t								handle,
		uint32_t			*	nCaptures
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetNoOfProcessedCaptures)
	(
	  int16_t								handle,
		uint32_t			*	nProcessedCaptures
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetDataBuffer)
(
   int16_t								 handle,
	 int32_t						 			 channelOrPort,
	 int16_t							*  buffer,
   int32_t									 bufferLth,
	 uint16_t				 segmentIndex,
	 PS2000A_RATIO_MODE		 mode
);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetDataBuffers)
(
   int16_t								 handle,
	 int32_t						 			 channelOrPort,
	 int16_t							 * bufferMax,
	 int16_t							 * bufferMin,
   int32_t									 bufferLth,
	 uint16_t				 segmentIndex,
	 PS2000A_RATIO_MODE		 mode
);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetEtsTimeBuffer)
(
   int16_t									handle,
	 int64_t *							buffer,
	 int32_t										bufferLth
);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aSetEtsTimeBuffers)
(
   int16_t									handle,
	 uint32_t				* timeUpper,
	 uint32_t				* timeLower,
	 int32_t										bufferLth
);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aIsReady)
	(
		int16_t handle,
		int16_t * ready
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aRunBlock)
	(
		int16_t									handle,
		int32_t									noOfPreTriggerSamples,
		int32_t									noOfPostTriggerSamples,
		uint32_t					timebase,
		int16_t									oversample,
		int32_t								* timeIndisposedMs,
		uint16_t				segmentIndex,
		ps2000aBlockReady			lpReady,
		void								* pParameter
	);


PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aRunStreaming)
  (
	  int16_t									handle,
		uint32_t				* sampleInterval,	
		PS2000A_TIME_UNITS		sampleIntervalTimeUnits,
	  uint32_t					maxPreTriggerSamples,
	  uint32_t					maxPostPreTriggerSamples,
		int16_t									autoStop,
		uint32_t					downSampleRatio,
		PS2000A_RATIO_MODE		downSampleRatioMode,
    uint32_t					overviewBufferSize
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetStreamingLatestValues)
  (
    int16_t									handle, 
    ps2000aStreamingReady	lpPs2000aReady,
		void								* pParameter
  );  

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aNoOfStreamingValues)
	(
	  int16_t								handle,
		uint32_t			*	noOfValues
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetMaxDownSampleRatio)
	(
	  int16_t								handle,
		uint32_t 			noOfUnaggreatedSamples,
		uint32_t 		* maxDownSampleRatio,
		PS2000A_RATIO_MODE	downSampleRatioMode,
		uint16_t			segmentIndex
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetValues)
	(
	  int16_t								handle,
		uint32_t 			startIndex,
	  uint32_t			*	noOfSamples,
	  uint32_t				downSampleRatio,
		PS2000A_RATIO_MODE	downSampleRatioMode,
		uint16_t			segmentIndex,
		int16_t							* overflow
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetValuesBulk)
	(
	  int16_t								handle,
		uint32_t			*	noOfSamples,
		uint16_t			fromSegmentIndex,
		uint16_t			toSegmentIndex,
	  uint32_t				downSampleRatio,
		PS2000A_RATIO_MODE 	downSampleRatioMode,
		int16_t							* overflow
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetValuesAsync)
	(
	  int16_t								handle,
		uint32_t				startIndex,
	  uint32_t				noOfSamples,
	  uint32_t				downSampleRatio,
		int16_t								downSampleRatioMode,
		uint16_t			segmentIndex,
	  void							*	lpDataReady,
		void							*	pParameter
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetValuesOverlapped)
	(
	  int16_t								handle,
	  uint32_t 			startIndex,
	  uint32_t			*	noOfSamples,
	  uint32_t				downSampleRatio,
	  PS2000A_RATIO_MODE	downSampleRatioMode,
		uint16_t      segmentIndex,
	  int16_t				*       overflow
	);


PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetValuesOverlappedBulk)
	(
	  int16_t								handle,
		uint32_t				startIndex,
  	uint32_t 	 	*	noOfSamples,
	  uint32_t				downSampleRatio,
	  PS2000A_RATIO_MODE	downSampleRatioMode,
	  uint16_t			fromSegmentIndex,
	  uint16_t			toSegmentIndex,
	  int16_t							*	overflow
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aStop)
	(
	  int16_t handle
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aHoldOff)
	(
	  int16_t								handle,	
		uint64_t						holdoff,
		PS2000A_HOLDOFF_TYPE	type
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetChannelInformation) 
	(
		int16_t handle, 
		PS2000A_CHANNEL_INFO info, 
		int32_t probe, 
		int32_t * ranges,
		int32_t * length,
		int32_t channels
  );

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aEnumerateUnits)
	(
	int16_t * count,
	int8_t * serials,
	int16_t * serialLth
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aPingUnit)
	(
	int16_t handle
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aMaximumValue)
	(
	int16_t		handle,
	int16_t * value
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aMinimumValue)
	(
	int16_t		handle,
	int16_t * value
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetAnalogueOffset)
	(
	int16_t handle, 
	PS2000A_RANGE range,
	PS2000A_COUPLING	coupling,
	float * maximumVoltage,
	float * minimumVoltage
	);

PREF0 PREF1 PICO_STATUS PREF2 PREF3 (ps2000aGetMaxSegments)
	(
	int16_t		handle,
	uint16_t * maxSegments
	);

#endif
