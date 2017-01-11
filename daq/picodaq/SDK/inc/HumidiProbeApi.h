/****************************************************************************
*
* Filename:    HumidiProbe.h
* Copyright:   Pico Technology Limited 2004
* Description:
*
* This header defines the interface to driver routines for the 
*	HumidiProbe Data Loggers for temperature and humidity
*
****************************************************************************/
#pragma once

#include <stdint.h>

#ifndef WIN32
#error Operating system not defined
#endif

#ifdef PREF0
  #undef PREF0
#endif
#ifdef PREF1
  #undef PREF1
#endif
#ifdef PREF2
  #undef PREF2
#endif
#ifdef PREF3
  #undef PREF3
#endif

#ifdef __cplusplus
  #define PREF0 extern "C"
#else
  #define PREF0
#endif

//
//	If you are dynamically linking HumidiProbe.DLL into your project #define DYNLINK
//  somewhere in your project before here.
// 
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

#define HUMIDIPROBE_MAX_PICO_UNITS 64
#define HUMIDIPROBE_MAX_UNITS 8


typedef enum enHumidiProbeInfo
{
  HUMIDIPROBE_DRIVER_VERSION,
  HUMIDIPROBE_USB_VERSION,
  HUMIDIPROBE_HARDWARE_VERSION,
  HUMIDIPROBE_VARIANT_INFO,
  HUMIDIPROBE_BATCH_AND_SERIAL,
  HUMIDIPROBE_CAL_DATE,	
  HUMIDIPROBE_KERNEL_DRIVER_VERSION, 
  HUMIDIPROBE_ERROR,
} HUMIDIPROBE_INFO;

typedef enum enHumidiProbeErrorCode
{
	HUMIDIPROBE_OK,
	HUMIDIPROBE_KERNEL_DRIVER,
	HUMIDIPROBE_NOT_FOUND,
	HUMIDIPROBE_CONFIG_FAIL,
	HUMIDIPROBE_INVALID_PARAMETERS,
	HUMIDIPROBE_CONVERSION_FAILED,
	HUMIDIPROBE_FW_FAIL,
	HUMIDIPROBE_ERROR_OS_NOT_SUPPORTED,
	HUMIDIPROBE_COMMUNICATION,
	HUMIDIPROBE_MAX_DEVICES
} HUMIDIPROBE_ERROR_CODES;

typedef enum enHumidiProbeOpenProgress
{
  HUMIDIPROBE_OPEN_PROGRESS_FAIL     = -1,
  HUMIDIPROBE_OPEN_PROGRESS_PENDING  = 0,
  HUMIDIPROBE_OPEN_PROGRESS_COMPLETE = 1
} HUMIDIPROBE_OPEN_PROGRESS;

typedef enum enCycleReading
{
  HUMIDIPROBE_CR_NONE,
  HUMIDIPROBE_CR_OLD_READING,
  HUMIDIPROBE_CR_NEW_READING
} HUMIDIPROBE_CYCLE_READING;

typedef enum enBlockMethod
{
  HUMIDIPROBE_BM_BLOCK,
  HUMIDIPROBE_BM_WINDOW,
  HUMIDIPROBE_BM_STREAM
} HUMIDIPROBE_BLOCK_METHOD;

#define MAX_HUMIDIPROBES 64

PREF0 PREF1 int16_t PREF2 PREF3(HumidiProbeApplyFix)	 ( int32_t fixNo, int16_t value );

PREF0 PREF1 int16_t PREF2 PREF3(HumidiProbeOpenUnit)	 ( void );

PREF0 PREF1 int16_t PREF2 PREF3(HumidiProbeOpenUnitAsync) (void);

PREF0 PREF1 int16_t PREF2 PREF3(HumidiProbeOpenUnitProgress) (int16_t * handle, int16_t * progress);

PREF0 PREF1 int16_t PREF2 PREF3(HumidiProbeGetUnitInfo) (
  int16_t handle,
  int8_t * string,
  int16_t stringLength,
  int16_t info);


PREF0 PREF1 int16_t PREF2 PREF3(HumidiProbeCloseUnit) ( int16_t handle );


PREF0 PREF1 int16_t PREF2 PREF3(HumidiProbeGetSingleValue) (
  int16_t handle,
  float *temp,
	int16_t filterTemp,
	float *humidity,
	int16_t filterHumidity);
