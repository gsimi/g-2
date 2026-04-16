/*
 * utilities.cc
 *
 *  Created on: Dec 22, 2016
 *      Author: carlo
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

#include <sys/types.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "libps5000a-1.1/ps5000aApi.h"
#ifndef PICO_STATUS
#include "libps5000a-1.1/PicoStatus.h"
#endif

#include "DeviceManager.h"

using namespace std;

#define scanf_s scanf
#define fscanf_s fscanf
#define Sleep(a) usleep(1000*a)
#define QUAD_SCOPE		4
#define DUAL_SCOPE		2

typedef enum enBOOL{FALSE,TRUE} BOOL;

typedef enum SIGGEN_TYPE
{
	SIGGEN_NONE = 0,
	SIGGEN_FUNCTGEN = 1,
	SIGGEN_AWG = 2
} SIGGEN_TYPE;

typedef enum MODEL_TYPE
{
	MODEL_NONE = 0,
	MODEL_PS5244B = 0xB244,
} MODEL_TYPE;

/* MODIFICATA DA CARLO POGGI IL 30/11/2016*/
typedef struct CHANNEL_SETTINGS
{
	PS5000A_COUPLING DCcoupled;
	int16_t range;
	int16_t enabled;
	float analogueOffset;
} CHANNEL_SETTINGS;

/* CREATA DA CARLO POGGI IL 15/12/2016*/
typedef struct TRIGGER_SETTINGS
{
//	int16_t triggerSet = 0; // 0 if channel trigger is not set, else if set
//	PS5000A_THRESHOLD_DIRECTION triggerDirection = PS5000A_RISING; // Rising if channel is not set
//	float triggerThreshold = 0.0;
	int16_t triggerSet; // 0 if channel trigger is not set, else if set
	PS5000A_THRESHOLD_DIRECTION triggerDirection; // Rising if channel is not set
	float triggerThreshold;
} TRIGGER_SETTINGS;

static PS5000A_RANGE psRanges [PS5000A_MAX_RANGES] = {
		PS5000A_10MV,
		PS5000A_20MV,
		PS5000A_50MV,
		PS5000A_100MV,
		PS5000A_200MV,
		PS5000A_500MV,
		PS5000A_1V,
		PS5000A_2V,
		PS5000A_5V,
		PS5000A_10V,
		PS5000A_20V,
		PS5000A_50V};

static uint16_t psRangesMV [12] = {
		10,
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
		50000};

#endif

