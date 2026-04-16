/* Gabriele Simi */
#include <CAENDigitizer.h>
#include "WaveDump.h"
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

/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char *argv[])
{
	WaveDumpConfig_t   WDcfg;
    WaveDumpRun_t      WDrun;
    CAEN_DGTZ_ErrorCode ret=0;
    int  handle;
    ERROR_CODES ErrCode= ERR_NONE;
    int i, ch, Nb=0, Ne=0;
    uint32_t AllocatedSize, BufferSize, NumEvents;
    char *buffer = NULL;
    char *EventPtr = NULL;
    char ConfigFileName[100];
    CAEN_DGTZ_X742_EVENT_t       *Event742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */
    FILE *f_ini;
	DataCorrection_t Table_gr0, Table_gr1;

    /* *************************************************************************************** */
    /* Open and parse configuration file                                                       */
    /* *************************************************************************************** */
    memset(&WDrun, 0, sizeof(WDrun));
    if (argc > 1)
        strcpy(ConfigFileName, argv[1]);
    else
        strcpy(ConfigFileName, DEFAULT_CONFIG_FILE);
    printf("Opening Configuration File %s\n", ConfigFileName);
    f_ini = fopen(ConfigFileName, "r");
    if (f_ini == NULL ) {
        ErrCode = ERR_CONF_FILE_NOT_FOUND;
        goto QuitProgram;
    }
    ParseConfigFile(f_ini, &WDcfg);
    fclose(f_ini);


QuitProgram:
    if (ErrCode) {
        printf("\a%s\n", ErrMsg[ErrCode]);
    }

    /* stop the acquisition */
    CAEN_DGTZ_SWStopAcquisition(handle);


    /* close the output files and free histograms*/
    for(ch=0; ch<WDcfg.Nch; ch++) {
        if( WDrun.fout[ch])
            fclose(WDrun.fout[ch]);
    }

    /* close the device and free the buffers */
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_CloseDigitizer(handle);

    return 0;

}
