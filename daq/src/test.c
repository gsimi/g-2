#include <CAENDigitizer.h>
#include <CAENVMElib.h>
#include "mudaq.h"
#include "WDplot.h"
#include "fft.h"
#include "keyb.h"
#include "X742CorrectionRoutines.h"

/* Error messages */
typedef enum  {
    ERR_NONE= 0,
    ERR_CONF_FILE_NOT_FOUND,
    ERR_DGZ_OPEN,
    ERR_BOARD_INFO_READ,
    ERR_INVALID_BOARD_TYPE,
    ERR_DGZ_PROGRAM,
    ERR_MALLOC,
    ERR_RESTART,
    ERR_INTERRUPT,
    ERR_READOUT,
    ERR_EVENT_BUILD,
    ERR_HISTO_MALLOC,
    ERR_UNHANDLED_BOARD,
    ERR_OUTFILE_WRITE,

    ERR_DUMMY_LAST,
} ERROR_CODES;
static char ErrMsg[ERR_DUMMY_LAST][100] = {
    "No Error",                                         /* ERR_NONE */
    "Configuration File not found",                     /* ERR_CONF_FILE_NOT_FOUND */
    "Can't open the digitizer",                         /* ERR_DGZ_OPEN */
    "Can't read the Board Info",                        /* ERR_BOARD_INFO_READ */
    "Can't run WaveDump for this digitizer",            /* ERR_INVALID_BOARD_TYPE */
    "Can't program the digitizer",                      /* ERR_DGZ_PROGRAM */
    "Can't allocate the memory for the readout buffer", /* ERR_MALLOC */
    "Restarting Error",                                 /* ERR_RESTART */
    "Interrupt Error",                                  /* ERR_INTERRUPT */
    "Readout Error",                                    /* ERR_READOUT */
    "Event Build Error",                                /* ERR_EVENT_BUILD */
    "Can't allocate the memory fro the histograms",     /* ERR_HISTO_MALLOC */
    "Unhandled board type",                             /* ERR_UNHANDLED_BOARD */
    "Output file write error",                          /* ERR_OUTFILE_WRITE */

};


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

/* ###########################################################################
*  Functions
*  ########################################################################### */
/*! \fn      static long get_time()
*   \brief   Get time in milliseconds
*
*   \return  time in msec
*/
static long get_time()
{
    long time_ms;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}

/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char *argv[])
{
	WaveDumpConfig_t   WDcfg;
    WaveDumpRun_t      WDrun;
    CAEN_DGTZ_ErrorCode ret=0;
    ERROR_CODES ErrCode= ERR_NONE;
    int  jdischandle,digihandle;
    char ConfigFileName[100];
    FILE *f_ini;
    CAEN_DGTZ_BoardInfo_t       BoardInfo;



    /* *************************************************************************************** */
    /* Open and parse configuration file                                                       */
    /* *************************************************************************************** */
    memset(&WDrun, 0, sizeof(WDrun));


    uint32_t ui=0;

    /*
    printf ("programming digitizer\n");
    char *digiBaseAddress="32100000";
    sscanf(digiBaseAddress,"%x",&ui);
    if (ret = CAENComm_OpenDevice(CAEN_DGTZ_PCI_OpticalLink,0,0,ui,&digihandle)) {
      ErrCode = ERR_DGZ_OPEN;
      goto QuitProgram;
    }

    ret = CAEN_DGTZ_GetInfo(digihandle, &BoardInfo);
    if (ret) {
      ErrCode = ERR_BOARD_INFO_READ;
      goto QuitProgram;
    }
    printf("Connected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
    printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
    printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
    

    if (ret = CAENComm_CloseDevice(jdischandle))
      goto QuitProgram;

    return 0;
    */
    /*
      program the jewel discriminator trigger board made by the fantastic duo Borsato Garfagnini
    */

    printf ("programming discriminator Jewel\n");
    char *jdiscBaseAddress="FF180000";
     ui=0;
    sscanf(jdiscBaseAddress,"%x",&ui);
    printf("configuring device at 0x%x\n",ui);
    if (ret = CAENComm_OpenDevice(CAEN_DGTZ_PCI_OpticalLink,0,0,ui,&jdischandle)) {
      ErrCode = ERR_DGZ_OPEN;
      goto QuitProgram;
    }
    uint16_t jdiscdata=0x0;
    uint32_t jdiscregister=0x0;
    
    /*
    CVBoardTypes  VMEBoard;
    int32_t       BHandle;
    VMEBoard = cvV2718;	
    if( CAENVME_Init(VMEBoard, 0, 0, &BHandle) != cvSuccess ) 
      {
	printf("\n\n Error opening the device\n");
	exit(1);
      }
      
    printf("Bhandle 0x%x\n",BHandle);

    */
    //select the channel
    int channel=3; //channel=[1:16]
    printf ("configuring channel %d\n",channel);
    //set the threshold
    uint16_t channel_mem_offset=4*(channel-1);
    jdiscregister=0x1000+channel_mem_offset; 
    float jthreshold=-50; //mV
    uint32_t DACthreshold=521.3+0.607*jthreshold;
    printf("setting the jthreshold to %f mV, %d DAC, 0x%x \n",jthreshold,DACthreshold,DACthreshold);
    jdiscdata=DACthreshold;
//    CAENVME_WriteCycle(BHandle, jdiscBaseAddress+jdiscregister, &jdiscdata,
//		       cvA24_U_PGM, cvD32 );

        
    CAENComm_Write32(jdischandle, jdiscregister, DACthreshold); 
    //check
    CAENComm_Read32(jdischandle, jdiscregister, &jdiscdata );
    printf( "check jdiscdata: write in 0x%x value 0x%x, read 0x%x\n",ui+jdiscregister,DACthreshold,jdiscdata);
    
    //set the trigger slope
    printf("setting negative slope \n");
    jdiscregister=0x2000; 
    uint16_t channel_bit=0x0001<<(channel-1); 
    uint16_t tr_slope_mask=0XFFFF & (~channel_bit); //0 = trigger on positive slope,  1 = trigger on negative slope, SET TO 1 THE BITS CORRESPONDING TO THE CHANNELS NOT CONNECTED
    CAENComm_Write16(jdischandle, jdiscregister, tr_slope_mask );
    //check
    CAENComm_Read16(jdischandle, jdiscregister, &jdiscdata );
    printf( "check jdiscdata: write in 0x%x value 0x%x, read 0x%x\n",ui+jdiscregister,tr_slope_mask,jdiscdata);

    //set the signal delay
    jdiscregister=0x3000; 
    uint16_t delay = 0;//1 DAC step = 4.56ns, max 8 bits
    printf("setting the delay to %d\n",delay);
    uint16_t channel_mask=channel*16*16;
    CAENComm_Write16(jdischandle, jdiscregister, channel_mask|delay);//zero delay for ch2
    //check
    CAENComm_Read16(jdischandle, jdiscregister, &jdiscdata );
    printf( "check jdiscdata: write in 0x%x value 0x%x, read 0x%x\n",ui+jdiscregister,channel_mask|delay,jdiscdata);

    //set the signal duration
    jdiscregister=0x4000; 
    float width=50; // in ns [0:1162]
    printf("setting the width to %f ns\n",width);
    uint16_t DACwidth = (width/4.56);//1 DAC step = 4.56ns, max 8 bits
    CAENComm_Write16(jdischandle, jdiscregister, channel_mask|DACwidth);//zero delay
    //check
    CAENComm_Read16(jdischandle, jdiscregister, &jdiscdata );
    printf( "check jdiscdata: write in 0x%x value 0x%x, read 0x%x\n",ui+jdiscregister,channel_mask|DACwidth,jdiscdata);

    // global trigger generation
    //enable mask: enable channels
    printf("setting the channel mask\n");
    jdiscregister=0x5000+channel_mem_offset; 
    uint32_t maskall=1<<16;
    uint32_t mask_channel=1<<(channel-1);
    uint32_t trigger_enable_mask=maskall|mask_channel;
    CAENComm_Write32(jdischandle, jdiscregister, trigger_enable_mask);
    //check
    uint32_t jdiscdata32=0;
    CAENComm_Read32(jdischandle, jdiscregister, &jdiscdata32 );
    printf( "check jdiscdata: write in 0x%x value 0x%x, read 0x%x\n",ui+jdiscregister,trigger_enable_mask,jdiscdata32);
    
    if (ret = CAENComm_CloseDevice(jdischandle))
      goto QuitProgram;
QuitProgram:
    if (ErrCode) {
        printf("\a%s\n", ErrMsg[ErrCode]);
#ifdef WIN32
        printf("Press a key to quit\n");
		getch();
#endif
    }
    return 0;
}



/*
valid values of addressing mode mask
  5   4   3   2   1   0
------------------------
  1   1   1   x   1   1
  1   1   1   x   1   0
  1   1   1   x   0   1
  
values on CAEN controller display
  0   0   1   0   0   1

CVAddressModifier 
cvA24_U_BLT     = 0x3B,         // A24 non-privileged block transfer            
cvA24_U_PGM     = 0x3A,         // A24 non-privileged program access            
  cvA24_U_DATA    = 0x39,         // A24 non-privileged data access               

CVDataWidth
  cvD32 = 0x04,                   // 32 bit                                       


cd  /home/lhcb/CAEN/VME/CAENVMELib-2.30.2/sample
./CAENVMEDemo V2718 0 0 

R - READ
 W - WRITE
 B - BLOCK TRANSFER READ
 T - BLOCK TRANSFER WRITE
 I - CHECK INTERRUPT
 1 - ADDRESS                  [FF181008]
 2 - BASE ADDRESS             [FF180000]
 3 - DATA FORMAT              [D32]
 4 - ADDRESSING MODE          [A24]
 5 - BLOCK TRANSFER SIZE      [256]
 6 - AUTO INCREMENT ADDRESS   [OFF]
 7 - NUMBER OF CYCLES         [1]
 8 - VIEW BLT DATA
 F - FRONT PANEL I/O
 Q - QUIT MANUAL CONTROLLER


programming discriminator Jewel
configuring device at 0xff180000

configuring channel 3

setting the jthreshold to -50.000000 mV
Note that using the pulse gengerator I find that thte threshold corresponds to about [-55:-85]mV minimum signal to trigger
setting   minV  maxV
-25        
-50 -55 -85
-100 -140 -170
-150 -230 -240
==> setting -40(1f1) corresponds roughly to a threshold of -50mV
check jdiscdata: write in 0xff181008 value 0x1f1


setting negative slope 
check jdiscdata: write in 0xff182000 value 0xffff

setting the delay to 0
check jdiscdata: write in 0xff183000 value 0x20c

setting the width to 50 ns
check jdiscdata: write in 0xff184000 value 0x20e

setting the channel mask
check jdiscdata: write in 0xff185008 value 0x10004

 */



