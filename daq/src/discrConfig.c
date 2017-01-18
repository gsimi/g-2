#include "CAENVMElib.h"
#include<time.h>
#include <unistd.h>

void
caenvmecheck(CVErrorCodes  ret){
    switch (ret)
      {
      case cvSuccess   : printf(" Cycle(s) completed normally\n");
	break ;
      case cvBusError	 : printf(" Bus Error !!!");
	break ;				   
      case cvCommError : printf(" Communication Error !!!");
	break ;
      default          : printf(" Unknown Error !!!");
	break ;
      }
}

void
caenvmereadcheck(CVErrorCodes ret, uint32_t vmeaddr, uint32_t jdiscdata){
  caenvmecheck(ret);
  printf( "check jdiscdata: read in 0x%x value 0x%x\n",vmeaddr,jdiscdata);
}

enum polarity_t{ positive, negative};
/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char *argv[])
{


  /*
    programning the discriminator/trigger board
  */

  /*
      
    Configured correctly using CAENVMEDemo v2742 0 0
      
    CAEN VME Manual Controller
      
    R - READ
    W - WRITE
    B - BLOCK TRANSFER READ
    T - BLOCK TRANSFER WRITE
    I - CHECK INTERRUPT
    1 - ADDRESS                  [00805004]
    2 - BASE ADDRESS             [00800000]
    3 - DATA FORMAT              [D32]
    4 - ADDRESSING MODE          [A24]
    5 - BLOCK TRANSFER SIZE      [256]
    6 - AUTO INCREMENT ADDRESS   [OFF]
    7 - NUMBER OF CYCLES         [1]
    8 - VIEW BLT DATA
    F - FRONT PANEL I/O
    Q - QUIT MANUAL CONTROLLER
      
      
    Cycle(s) completed normally
    Read 256 bytes
    configurig chanel 2
    write in 0x801004 value 0x1f7 : treshold -30mv
    write in 0x802000 value 0xffff : negative polarity
    write in 0x803000 value 0x100 : 0 delay
    write in 0x804000 value 0x10a : 50 ns width
    write in 0x805004 value 0xfffefffd : trigger enabled for ch2
    write in 0x805004 value 0xfffefff9 : trigger enabled for ch2 & 3
    write in 0x805004 value 0xfffefff1 : trigger enabled for ch2 & 3 & 4
    write in 0x805004 value 0xfffefff0 : trigger enabled for ch1 & 2 & 3 & 4
      
  */
    
  /* 
     CHANNEL CONFIGURATION 
  */

  #define nch  6
  int channel[nch]  =  {1,2,3,4,5,6};  //channel=[1:16]
  float jthreshold[nch]  =  {-50,
			     -50,
                             -50,
			     -50, 
			     -50, 
			     -50}; //[mV]
  int polarity[nch]      =  {negative,     
			     negative,     
			     negative,    
			     negative,   
			     negative,    
			     negative};
  //mask of enabled channels.  This mask also determines the channels used in the coincidence
  // for lifetime 
  // uint32_t channel_mask = 0x000f;
  // uint32_t veto_mask = 0x0008;  //mask of veto channels

  /* for efficiecy studies */
  uint32_t channel_mask = 0x0007;
  uint32_t veto_mask = 0x0000;  //mask of veto channels

  /* for time calibration studies
  // uint32_t channel_mask = 0x0010; //mask of enabled channels.  This mask also determines the channels used in the coincidence
  // uint32_t veto_mask = 0x0000;  //mask of veto channels
  */

  //keep a minimum of about 58,75ns (5 DAC) delay to give time to the electronics to shape the signals
  //  range [45ns:750ns] in 2.75ns steps
  float tdelay[nch]      =  {45.0 + 20*2.75,
			     45.0 + 20*2.75,
			     45.0 + 20*2.75, 
			     45.0 + 10*2.75,  //veto 27.5 ns early
			     45.0 + 10*2.75,
			     45.0 + 10*2.75};//[ns] 

  //  range [10ns:1173ns] in 4.56ns steps
  float adcwidth=33; //  160ns
  float vetowidth=45; //215ns
  float width[nch]       =  {10.1 + adcwidth*4.56,
			     10.1 + adcwidth*4.56,
			     10.1 + adcwidth*4.56,
			     10.1 + vetowidth*4.56,
			     10.1 + adcwidth*4.56,
			     10.1 + adcwidth*4.56}; // [ns] 
  
  int32_t       JHandle;
  CVErrorCodes  cvret;
  printf ("programming trigger board\n");
  
  /* 
     BOARD INITIALIZATION
  */
  CVBoardTypes  VMEBoard = cvV2718;	
  int LinkNum = 0;
  int ConetNode = 0;
  cvret = CAENVME_Init(VMEBoard, LinkNum, ConetNode, &JHandle);
  if(  cvret != cvSuccess ) 
    {
      printf("\n\n Error opening the device\n");
      printf("arguments:vmeboard %d, link %d, device %d, handle %d\n",VMEBoard, LinkNum, ConetNode, JHandle);	
      printf("error %d\n",cvret);
      exit(1);
    }

  /* 
     VME ADDRESSING MODE CONFIGURATION 
  */
  //  uint32_t jdiscbaseaddr = 0x00800000; 
  uint32_t jdiscbaseaddr = 0x00600000; 
  uint32_t jdiscregister = 0x0;
  uint32_t vmeaddr       = jdiscbaseaddr; 
  uint32_t jdiscdata     = 0x0;
  CVDataWidth dtsize = cvD32 ;
  ushort am = cvA24_U_DATA ;            
    

  /*
    THRESHOLD SETTING
  */
  printf("\n");

int ich;
  for (ich=0;ich<nch;ich++){
    printf ("configuring channel %d\n",channel[ich]);
    uint16_t channel_mem_offset=4*(channel[ich]-1);
    //set the threshold address
    jdiscregister = 0x1000;
    jdiscregister += channel_mem_offset; 
    //compute the threshold
    jdiscdata=521.3+0.607*jthreshold[ich];
    //write
    printf("writing in register 0x%x\n",jdiscregister);
    printf("setting the jthreshold to %f mV, %d DAC, 0x%x \n",jthreshold[ich],jdiscdata,jdiscdata);
    vmeaddr=jdiscbaseaddr + jdiscregister;
    cvret = CAENVME_WriteCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
    //check
    caenvmecheck(cvret);

    /*
      SIGNAL DELAY CONFIGURATION
    */
    printf("\n");

    //set the signal delay
    jdiscregister=0x3000; 
    uint32_t DACdelay = tdelay[ich]/2.75 - 16.4;//1 DAC step = 2.75ns, max 8 bits
    jdiscdata = ((channel[ich]-1) <<8)|DACdelay;
    //write
    printf("writing in register 0x%x\n",jdiscregister);
    printf("setting the signal delay to %f ns, DAC %d, hex 0x%x \n",tdelay[ich],DACdelay,jdiscdata);
    vmeaddr = jdiscbaseaddr + jdiscregister;
    cvret = CAENVME_WriteCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
    //check
    caenvmecheck(cvret);
    usleep(10);//wait 1ms to allow the board to write the delay register
    /*
      SIGNAL DURATION CONFIGURATION
    */
    printf("\n");

    //set the signal duration
    jdiscregister=0x4000; 
    uint32_t DACwidth = (width[ich]/4.56 -2.2);//1 DAC step = 4.56ns, max 8 bits
    jdiscdata = ((channel[ich]-1) <<8)|DACwidth;
    //write
    printf("writing in register 0x%x\n",jdiscregister);
    printf("setting the width to %f ns, DAC %d, mask to 0x%x \n",width[ich],DACwidth,jdiscdata);
    vmeaddr = jdiscbaseaddr + jdiscregister;
    cvret = CAENVME_WriteCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
    //check
    caenvmecheck(cvret);
}
    /*
      SIGNAL POLARITY CONFIGURATION
    */
    printf("\n\n");
    
    //set the trigger polarity address
    jdiscregister = 0x2000;
    //set the trigger polarity mask
    uint32_t tr_polarity_mask = 0XFFFF; //0 = trigger on positive polarity,  1 = trigger on negative polarity, SET TO 1 THE BITS CORRESPONDING TO THE CHANNELS NOT CONNECTED
/*
  for (ich=0;ich<nch;ich++){
    tr_polarity_mask = tr_polarity_mask & ~(1<<(channel[ich]-1)); //fix the polarity of our channels negative
  }*/
  for (ich=0;ich<nch;ich++){
	if (polarity[ich]==positive) tr_polarity_mask = tr_polarity_mask & ~(1<<(channel[ich]-1) );
}	
    jdiscdata=tr_polarity_mask;
    //write
    printf("writing in register 0x%x\n",jdiscregister);
    printf("setting the polarity mask to 0x%x \n",jdiscdata);
    vmeaddr = jdiscbaseaddr + jdiscregister;
    cvret = CAENVME_WriteCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
    //check
    caenvmecheck(cvret);
    jdiscdata=0;
    cvret = CAENVME_ReadCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
    caenvmereadcheck(cvret,vmeaddr,jdiscdata);


  /*
    TRIGGER CONFIGURATION
  */
  printf("\n");

  // global trigger generation
  //enable mask: enable channels
  int trigger_offset[3]={0x4,0x8,0xc};// 0x4=TR1, 0x8=TR2, 0xc=TR3
  int itrig;
  for (itrig=1; itrig<=2; itrig++){
    jdiscregister=0x5000+trigger_offset[itrig]; 
    uint32_t mask_channel=0xffff & ~(channel_mask);
    uint32_t trigger_enable_mask=mask_channel;
    jdiscdata = trigger_enable_mask;
    //write
    printf("writing in register 0x%x\n",jdiscregister);
    printf("setting the channel mask to 0x%x \n",jdiscdata);
    vmeaddr = jdiscbaseaddr + jdiscregister;
    cvret = CAENVME_WriteCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
    //check
    caenvmecheck(cvret);
    cvret = CAENVME_ReadCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
    printf( "check jdiscdata: read in 0x%x value 0x%x\n",vmeaddr,jdiscdata);
    caenvmereadcheck(cvret,vmeaddr,jdiscdata);
  }

  /*
    VETO CONFIGURATION
  */
 
  printf("\n");

  // veto generation
  // enable channels
  int veto_offset=0x18;// 0x14=VETO1, 0x18=VETO2, 0x1C=VETO3
  jdiscregister=0x5000+veto_offset; 

  uint32_t veto_channel=veto_mask;
  jdiscdata = veto_channel;
  //write
  printf("writing in register 0x%x\n",jdiscregister);
  printf("setting the channel veto mask to 0x%x \n",jdiscdata);
  vmeaddr = jdiscbaseaddr + jdiscregister;
  cvret = CAENVME_WriteCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
  //check
  caenvmecheck(cvret);
  cvret = CAENVME_ReadCycle(JHandle,vmeaddr,&jdiscdata,am,dtsize);
  printf( "check jdiscdata: read in 0x%x value 0x%x\n",vmeaddr,jdiscdata);
  caenvmereadcheck(cvret,vmeaddr,jdiscdata); 

  //close the device
  CAENVME_End(JHandle);


  return 0;
}
