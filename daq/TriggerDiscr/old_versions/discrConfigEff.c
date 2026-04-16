#include "CAENVMElib.h"
#include <time.h>
#include <unistd.h>
#define nch 16

typedef enum __polarity_t{ positive, negative} polarity_t;

typedef struct __VME_board
{
	uint32_t baseaddr;
	CVBoardTypes type;
	int32_t linkNum;
	int32_t conetNode;
	int32_t jHandle;
	CVDataWidth dtsize;
	ushort am;
} VME_board;

void
caenvmecheck(CVErrorCodes  ret){
	switch (ret)
	{
		case cvSuccess: 
			printf(" Cycle(s) completed normally\n");
			break ;
		case cvBusError: 
			printf(" Bus Error !!! ");
			break ;				   
		case cvCommError: 
			printf(" Communication Error !!! ");
			break ;
		default: 
			printf(" Unknown Error !!! ");
			break ;
	}
}

uint32_t scint(char n)
{
	printf("DEBUG %d: %d	\n", n, 1 << (n));
	return 1 << (n);
}

uint32_t addresses[64];
uint32_t values[64];
CVErrorCodes debugWrite(long Handle, unsigned long Address, void *Data, CVAddressModifier AM, CVDataWidth DW)
{
	/*static int counter = 0;
	addresses[counter] = Address;
	values[counter] = *(uint32_t*)Data;
	counter += 1;*/
	return CAENVME_WriteCycle(Handle, Address, Data, AM, DW);
}

void caenvmereadcheck(CVErrorCodes ret, uint32_t vmeaddr, uint32_t jdiscdata)
{
	caenvmecheck(ret);
	printf( "checking jdiscdata: read in 0x%x value 0x%8x\n", vmeaddr, jdiscdata);
}

CVErrorCodes configure_threshold(VME_board *link, uint8_t ich, float threshold)
{
	if (threshold == 0)
		return cvSuccess;
    
    //set the threshold address
    const uint32_t jdiscregister = 0x001000;
    const uint32_t channel_mem_offset = 4 * ich;
    uint32_t vmeaddr = link->baseaddr | jdiscregister | channel_mem_offset;
    
    
    //set threshold value in bit
    uint32_t jdiscdata = 521.3 + 0.607*threshold;
    
    //write
    printf("writing in register 0x%x, address %06x\n", jdiscregister, vmeaddr);
    printf("setting the jthreshold to %f mV, %d DAC, 0x%08x \n", threshold, jdiscdata, jdiscdata);
    CVErrorCodes cvret = debugWrite(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
    
    //check
    caenvmecheck(cvret);
    
    return cvret;
}

CVErrorCodes configure_delay(VME_board* link, uint8_t ich, float delay)
{
	if (delay == 0)
		return cvSuccess;
	//set the signal delay
    const uint32_t jdiscregister = 0x3000;
    uint32_t vmeaddr = link->baseaddr | jdiscregister;
    
    uint32_t DACdelay = (delay - 45.0)/2.75;
    
    uint32_t jdiscdata = (ich << 8) | DACdelay;
    //write
    printf("writing in register 0x%x, address %06x\n", jdiscregister, vmeaddr);
    printf("setting the signal delay to %f ns, DAC %d, hex 0x%08x \n", delay, DACdelay, jdiscdata);
    CVErrorCodes cvret = debugWrite(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
    //check
    caenvmecheck(cvret);
    usleep(10);//wait 1ms to allow the board to write the delay register
    
    return cvret;
}

CVErrorCodes configure_width(VME_board* link, uint8_t ich, float width)
{
	if (width == 0)
		return cvSuccess;
	//set the signal delay
    const static uint32_t jdiscregister = 0x4000;
    uint32_t vmeaddr = link->baseaddr | jdiscregister;
    
    uint8_t DACwidth = (width - 10.1)/4.56;
    
    uint32_t jdiscdata = (ich << 8) | DACwidth;
    //write
    printf("writing in register 0x%x, address %06x\n", jdiscregister, vmeaddr);
    printf("setting the signal width to %f ns, DAC %d, hex 0x%8x \n", width, DACwidth, jdiscdata);
    CVErrorCodes cvret = debugWrite(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
    //check
    caenvmecheck(cvret);
    
    return cvret;
}

CVErrorCodes configure_polarity(VME_board* link, int* pol)
{			 		
	uint16_t tr_polarity_mask = 0XFFFF;
	
	int ich;
	for (ich = 0; ich < nch; ich++){
		if (pol[ich] == positive) 
			tr_polarity_mask = tr_polarity_mask & ~(scint(ich) );
	}
	//set the trigger polarity address
    const static uint32_t jdiscregister = 0x2000;
	uint32_t vmeaddr = link->baseaddr | jdiscregister;
    
    uint32_t jdiscdata = tr_polarity_mask;
    //write
    printf("writing in register 0x%x, address %06x\n", jdiscregister, vmeaddr);
	printf("setting the polarity mask to 0x%8x \n", jdiscdata);
	CVErrorCodes cvret = debugWrite(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
	//check
	caenvmecheck(cvret);
	
	cvret = CAENVME_ReadCycle(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
	printf("check jdiscdata: read in 0x%x value 0x%x\n", vmeaddr, jdiscdata);
	caenvmereadcheck(cvret, vmeaddr, jdiscdata);
    
    return cvret;
}

CVErrorCodes configure_trigger(VME_board* link, uint8_t itrig, uint32_t enabled_channel)
{
	const static int trigger_offset[3] = {0x4, 0x8, 0xc};
	//set the signal delay
    const uint32_t jdiscregister = 0x5000 | trigger_offset[itrig];
	uint32_t vmeaddr = link->baseaddr | jdiscregister;
    
    uint32_t mask_channel = 0xffff & ~(enabled_channel);
    
    uint32_t jdiscdata = mask_channel;
    //write
    printf("writing in register 0x%x, address %06x\n", jdiscregister, vmeaddr);
	printf("setting the channel mask to 0x%x \n", jdiscdata);
    CVErrorCodes cvret = debugWrite(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
    //check
    caenvmecheck(cvret);
    
    cvret = CAENVME_ReadCycle(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
	printf("check jdiscdata: read in 0x%x value 0x%x\n", vmeaddr, jdiscdata);
	caenvmereadcheck(cvret, vmeaddr, jdiscdata);
    
    return cvret;
}

CVErrorCodes configure_veto(VME_board* link, uint8_t itrig, uint32_t vetoed_channel)
{
	const static int veto_offset[3] = {0x14, 0x18, 0x1c};
	//set the signal delay
    const uint32_t jdiscregister = 0x5000 | veto_offset[itrig];
	uint32_t vmeaddr = link->baseaddr | jdiscregister;
    
    uint32_t jdiscdata = vetoed_channel;
    //write
    printf("writing in register 0x%x, address %06x\n", jdiscregister, vmeaddr);
	printf("setting the channel mask to 0x%08x \n", jdiscdata);
    CVErrorCodes cvret = debugWrite(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
    //check
    caenvmecheck(cvret);
    
    cvret = CAENVME_ReadCycle(link->jHandle, vmeaddr, &jdiscdata, link->am, link->dtsize);
	printf("check jdiscdata: read in 0x%x value 0x%x\n", vmeaddr, jdiscdata);
	caenvmereadcheck(cvret, vmeaddr, jdiscdata);
    
    return cvret;
} 


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

	float threshold[nch] = {
								-50,	//0
								0,		//1
								-50,	//2
								0, 		//3
								0,	//4
								-50,	//5
								-50,	//6
								-50,	//7
								-50, 	//8
								-50, 	//9
								-50,	//10
								-50,	//11
								-50,	//12
								0,	//13
								-50, 	//14
								-50		//15
							}; //[mV]
							
							
	// Polarity						
	int polarity[nch] = {
							negative,	//0    
				 			negative, 	//1    
				 			negative, 	//2   
				 			negative, 	//3  
				 			negative, 	//4   
				 			negative,	//5
				 			negative, 	//6    
				 			negative, 	//7    
				 			negative, 	//8  
				 			negative, 	//9  
				 			negative, 	//10   
				 			negative,	//11
				 			negative, 	//12    
				 			negative, 	//13    
				 			negative, 	//14   
				 			negative	//15
				 		};
				 			

	// Enabled channels
	uint32_t enabled_channels[3] = {0x00000, 0x00000, 0x00000};

	enabled_channels[0] = scint(0) 
						| scint(2) 
						;
	enabled_channels[1] = scint(10)
						| scint(11)
						| scint(12) 
						| scint(14) 
						| scint(15)
						;
						
	// Veto
	uint32_t veto_mask[3] = 
	{
		0x0000, 
		0x0000, 
		0x0000
	};  //mask of veto channels

	//keep a minimum of about 58,75ns (5 DAC) delay to give time to the electronics to shape the signals
	//  range [45ns:750ns] in 2.75ns steps
	float delay[nch]      = 	
	{
		45.0 + 20*2.75,	//0
		0,	//1
		45.0 + 20*2.75,	//2
		45.0 + 20*2.75,	//3
		45.0 + 20*2.75,	//4
		45.0 + 20*2.75,	//5
		45.0 + 20*2.75,	//6
		45.0 + 20*2.75,	//7
		45.0 + 20*2.75,	//8
		45.0 + 20*2.75,	//9
		45.0 + 20*2.75,	//10
		45.0 + 20*2.75,	//11
		45.0 + 20*2.75,	//12
		45.0 + 20*2.75,	//13
		45.0 + 20*2.75,	//14
		45.0 + 20*2.75	//15
	};//[ns] 

	//  range [10ns:1173ns] in 4.56ns steps
	float adcwidth = 33; //  160ns
	//  float vetowidth=45; //215ns
	float width[nch]       =	
	{
		10.1 + adcwidth*4.56,	//0
		0,	//1
		10.1 + adcwidth*4.56,	//2
		10.1 + adcwidth*4.56,	//3
		10.1 + adcwidth*4.56,	//4
		10.1 + adcwidth*4.56,	//5
		10.1 + adcwidth*4.56,	//6
		10.1 + adcwidth*4.56,	//7
		10.1 + adcwidth*4.56,	//8
		10.1 + adcwidth*4.56,	//9
		10.1 + adcwidth*4.56,	//10
		10.1 + adcwidth*4.56,	//11
		10.1 + adcwidth*4.56,	//12
		10.1 + adcwidth*4.56,	//13
		10.1 + adcwidth*4.56,	//14
		10.1 + adcwidth*4.56	//15
	}; // [ns] 

	CVErrorCodes  cvret;
	printf ("programming trigger board\n");

	/* 
	 BOARD INITIALIZATION
	*/
	VME_board link;
	
	link.type = cvV2718;
	link.linkNum = 0;
	link.conetNode = 0;
	
	cvret = CAENVME_Init(link.type, link.linkNum, link.conetNode, &link.jHandle);
	if(  cvret != cvSuccess ) 
	{
		printf("\n\n Error opening the device\n");
		printf("arguments:vmeboard %d, link %d, device %d, handle %d\n",link.type, link.linkNum, link.conetNode, link.jHandle);	
		printf("error %d\n",cvret);
		return (int)cvret;
	}
	
	// VME ADDRESSING MODE CONFIGURATION 
	link.baseaddr = 0x00600000;
	link.dtsize = cvD32;
	link.am = cvA24_U_DATA ;           


	printf("\n");

	int ich;
	for (ich = 0; ich < nch; ich++){
		printf ("configuring channel %d\n", ich);
		
		cvret = configure_threshold(&link, ich, threshold[ich]);
		
		if (cvret != cvSuccess){
			printf("\n\n Error setting threshold %f\n", threshold[ich]);
			printf("arguments:vmeboard %d, link %d, device %d, handle %d\n", link.type, link.linkNum, link.conetNode, link.jHandle);	
			printf("error %d\n", cvret);
			return (int)cvret;
		} 
		
		cvret = configure_delay(&link, ich, delay[ich]);
		
		if (cvret != cvSuccess){
			printf("\n\n Error setting delay %f\n", delay[ich]);
			printf("arguments:vmeboard %d, link %d, device %d, handle %d\n", link.type, link.linkNum, link.conetNode, link.jHandle);	
			printf("error %d\n", cvret);
			return (int)cvret;
		} 
		
		cvret = configure_width(&link, ich, width[ich]);
		
		if (cvret != cvSuccess){
			printf("\n\n Error setting width %f\n", width[ich]);
			printf("arguments:vmeboard %d, link %d, device %d, handle %d\n", link.type, link.linkNum, link.conetNode, link.jHandle);	
			printf("error %d\n", cvret);
			return (int)cvret;
		} 
		
		printf("\n");
	}
	
	cvret = configure_polarity(&link, polarity);
		
	if (cvret != cvSuccess){
		printf("\n\n Error setting polarity %d\n", polarity[ich]);
		printf("arguments:vmeboard %d, link %d, device %d, handle %d\n", link.type, link.linkNum, link.conetNode, link.jHandle);	
		printf("error %d\n", cvret);
		return (int)cvret;
	} 


	/*
	TRIGGER CONFIGURATION
	*/
	printf("\n");
	
	int itrig;
	for (itrig = 0; itrig < 3; itrig++){
		cvret = configure_trigger(&link, itrig, enabled_channels[itrig]);
		
		if (cvret != cvSuccess){
			printf("\n\n Error setting trigger %x\n", enabled_channels[itrig]);
			printf("arguments:vmeboard %d, link %d, device %d, handle %d\n", link.type, link.linkNum, link.conetNode, link.jHandle);	
			printf("error %d\n", cvret);
			return (int)cvret;
		} 
		
		cvret = configure_veto(&link, itrig, veto_mask[itrig]);
		
		if (cvret != cvSuccess){
			printf("\n\n Error setting trigger %x\n", veto_mask[itrig]);
			printf("arguments:vmeboard %d, link %d, device %d, handle %d\n", link.type, link.linkNum, link.conetNode, link.jHandle);	
			printf("error %d\n", cvret);
			return (int)cvret;
		} 
	}

	printf("\n");
	//close the device
	CAENVME_End(link.jHandle);
	/*int counter;
	for (counter = 0; counter < 64; counter++)
	{
		printf("%08x%08x\n", addresses[counter], values[counter]);
	}*/


	return 0;
}
