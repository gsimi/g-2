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
 *   This file contains all the .NET wrapper calls needed to support
 *   the console example. It also has the enums and structs required
 *   by the (wrapped) function calls.
 *   
 **************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Text;


public class Win32Interop
{
    [DllImport("crtdll.dll")]
    public static extern int _kbhit();
}


namespace PLCM3Example
{
	class Imports
	{
		#region constants
		private const string _DRIVER_FILENAME = "PLCM3.dll";


		#endregion

		#region Driver enums

        public enum enPLCM3Channels
        {
            PLCM3_CHANNEL_1 = 1,
            PLCM3_CHANNEL_2,
            PLCM3_CHANNEL_3,
            PLCM3_MAX_CHANNELS = PLCM3_CHANNEL_3
        }

        public enum enPLCM3DataType
        {
            PLCM3_OFF,
            PLCM3_1_MILLIVOLT,
            PLCM3_10_MILLIVOLTS,
            PLCM3_100_MILLIVOLTS,
            PLCM3_VOLTAGE
        }


        public enum enIpDetailsType
        {
            PLCM3_IDT_GET,
            PLCM3_IDT_SET,
        }

        public enum enCommunicationType
        {
            PLCM3_CT_USB = 0x00000001,
            PLCM3_CT_ETHERNET = 0x00000002
        }

       

		#endregion

		#region Driver Imports
		

        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3CloseUnit")]
        public static extern short CloseUnit( short handle);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3Enumerate")]
        public static extern short Enumerate( StringBuilder details,
                                              out UInt32 length,
                                              enCommunicationType ComType);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3GetUnitInfo")]
        public static extern short GetUnitInfo(short handle,
                                              StringBuilder infoString,
                                              short strlength,
                                              out short reqSize,
                                              int info);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3GetValue")]
        public static extern short GetValue(short handle,
                                              enPLCM3Channels channel,
                                              out UInt32 value);


        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3IpDetails")]
        public static extern short IpDetails(short handle,
                                               out short enabled,
                                               StringBuilder ipaddress,
                                               ref ushort length,
                                               ref ushort listeningPort,
                                               enIpDetailsType type);


        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3OpenUnit")]
        public static extern short OpenUnit(out short handle,
                                             StringBuilder serial);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3OpenUnitViaIp")]
        public static extern short OpenUnitViaIp(out short handle,
                                                  StringBuilder serial,
                                                  String IPAddress);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3SetChannel")]
        public static extern short SetChannel(short handle,
                                                    enPLCM3Channels channel,
                                                    enPLCM3DataType type);


        [DllImport(_DRIVER_FILENAME, EntryPoint = "PLCM3SetMains")]
        public static extern short SetMains(out short handle,
                                                ushort sixty_hertz);








	
		#endregion
	}
}

