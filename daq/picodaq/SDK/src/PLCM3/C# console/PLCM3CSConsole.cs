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
 *   PLCM3 driver using C#
 **************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using PLCM3Example;

namespace PLCM3Example
{
    class ConsoleExample
    {
        private readonly short _handle;

        public const int NUM_CHANNELS = 3;
        public const short PICO_OK = 0;
        public const short PICO_NO_SAMPLES_AVAILABLE = 0x25;
        public static short status;
        public static bool USB = true;

       public static Imports.enPLCM3DataType[] channelSettings = new Imports.enPLCM3DataType[NUM_CHANNELS];



       /* ********************************************************************************************************************************
       * WaitForKey() 
       * 
       * Wait for user keypress
       *  ********************************************************************************************************************************/
       private static void WaitForKey()
        {
            while (!Console.KeyAvailable) Thread.Sleep(100);
            if (Console.KeyAvailable) Console.ReadKey(true); // clear the key
        }



        /* ********************************************************************************************************************************
        * ChannelSetUp()
        * 
        * Set each channel with the type of input it will have. Valid types are from 0 to 4.
        *  ********************************************************************************************************************************/
        void ChannelSetUp()
        {
            int index;
            int channel;
            int type;
            bool validInput;

            Console.WriteLine("\nChannel Setup\n=============\n");
            Console.WriteLine("0:\tOFF");
            Console.WriteLine("1:\t1mV range (1mV/A)");
            Console.WriteLine("2:\t10mV range (10mV/A)");
            Console.WriteLine("3:\t100mV range (100mV/A)");
            Console.WriteLine("4:\tVoltage input\n");



            for (index = 0; index < NUM_CHANNELS && status == PICO_OK; index++)
            {
                channel = 1 + index;
                do
                {
                    Console.WriteLine("channel {0}", channel);
                    try
                    {
                        Console.WriteLine("Enter measurement type: ");
                        type = int.Parse(Console.ReadLine());
                        if (type < 0 || type > 4)
                        {
                            Console.WriteLine("Invalid Input"); 
                            validInput = false;
                        }
                        else
                        {
                            channelSettings[index] = (Imports.enPLCM3DataType)type;
                            Console.WriteLine("Measurement Type: {0}", MeasurementTypeToString(((Imports.enPLCM3DataType)type)));
                            validInput = true;
                        }

                        Console.WriteLine("\n");
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.Message);
                        validInput = false;
                    }
                }
                while (!validInput);
            }
        }


        /* ********************************************************************************************************************************
        * MeasurementTypeToString()
        * 
        * Routine to allow the selected measurement type into a string to output
        *  ********************************************************************************************************************************/
        string MeasurementTypeToString(Imports.enPLCM3DataType measurementType)
        {
            System.Text.StringBuilder type = new System.Text.StringBuilder(80);

            switch (measurementType)
            {
                case Imports.enPLCM3DataType.PLCM3_OFF:
                    return "OFF";
                case Imports.enPLCM3DataType.PLCM3_1_MILLIVOLT:
                    return "PLCM3_1MV";
                case Imports.enPLCM3DataType.PLCM3_10_MILLIVOLTS:
                    return "PLCM3_10MV";
                case Imports.enPLCM3DataType.PLCM3_100_MILLIVOLTS:
                    return "PLCM3_100MV";
                case Imports.enPLCM3DataType.PLCM3_VOLTAGE:
                    return "PLCM3_VOLTAGE";
                default:
                    return "ERROR";
            }
        }




        /* ********************************************************************************************************************************
         * ChannelSetUp()
         * 
         * Routine to allow the user to change channel settings
         *  ********************************************************************************************************************************/
        double ApplyScaling(long value, int channel, ref String units)
        {
            switch (channelSettings[channel])
            {
                case Imports.enPLCM3DataType.PLCM3_OFF:
                    units = "";
                    return 0;

                case Imports.enPLCM3DataType.PLCM3_1_MILLIVOLT:
                    units = "A";
                    return value / 1000.0;

                case Imports.enPLCM3DataType.PLCM3_10_MILLIVOLTS:
                    units = "A";
                    return value / 1000.0;

                case Imports.enPLCM3DataType.PLCM3_100_MILLIVOLTS:
                    units = "mA";
                    return value;

                case Imports.enPLCM3DataType.PLCM3_VOLTAGE:
                    units = "mV";
                    return value / 1000.0;

                default:
                    return -1;
            }
        }


        /* ********************************************************************************************************************************
         * CollectData()
         * 
         * Routine to allow the user to change channel settings
         *  ********************************************************************************************************************************/
        void CollectData()
    {
	    //short channel;
        int channel;
        int index;
        uint[] values = new uint[NUM_CHANNELS];
        double[] scaledValues = new double[NUM_CHANNELS];
        string[] units = new string[NUM_CHANNELS];

	    status = PICO_OK;

	    //Display channel settings
	    Console.WriteLine("\n");
	    Console.WriteLine("Settings:\n\n");
	
	    for(channel = 0; channel < NUM_CHANNELS; channel++)
	    {
		    Console.WriteLine("Channel {0}", channel + 1);
		    Console.WriteLine("Measurement Type: {0}", MeasurementTypeToString(channelSettings[channel]));
	    }



        for (index = 0; index < NUM_CHANNELS && status == PICO_OK; index++)
        {
            channel = index + 1;
            status = Imports.SetChannel(_handle, (Imports.enPLCM3Channels)channel,
                                        (Imports.enPLCM3DataType)channelSettings[index]);
        }

	    if(status != PICO_OK)
	    {
		    Console.WriteLine("\n\nSetChannel: Status = {0:X}\nPress any key", status);
		   
	    }

	    Console.WriteLine("Press any key to start.\n\n");
	   WaitForKey();

	    Console.WriteLine("Press any key to stop...\n");


       

	    while((Win32Interop._kbhit() == 0) && (status == PICO_OK || status == PICO_NO_SAMPLES_AVAILABLE))
	    {
		    for(index = 0; index < NUM_CHANNELS && (status == PICO_OK || status == PICO_NO_SAMPLES_AVAILABLE); index++)
		    {
                channel = index + 1;
			    status = Imports.GetValue(_handle, (Imports.enPLCM3Channels)channel, out values[index]);
				
			    if(status == PICO_NO_SAMPLES_AVAILABLE) values[index] = 0;

			    scaledValues[index] = ApplyScaling(values[index], index, ref units[index]);
		    }

          

            // Print vaues for each channel
            for (index = 0; index < NUM_CHANNELS; index++)
		    {
                Console.Write("{0}{1, -10}", scaledValues[index], units[index]);
			    Console.Write(index==NUM_CHANNELS-1?"\n":"");
			    Thread.Sleep(1000);
		    }
	    }
		
	    if(status != PICO_OK && status != PICO_NO_SAMPLES_AVAILABLE)
		    Console.WriteLine("\n\nGetValue: Status = {0:X}\nPress any key", status);

	    WaitForKey();
    }


        /* ********************************************************************************************************************************
         * CheckPort()
         * 
         * Routine to check the port entered is in the correct format
         *  ********************************************************************************************************************************/

        static bool CheckPort(string portstr, ref ushort port)
    {
        if (ushort.TryParse(portstr, out port))
            return true;
        else
            return false;
    }

        /* ********************************************************************************************************************************
        * CheckIP()
        * 
        * Routine to check IP address entered is in the correct format
        *  ********************************************************************************************************************************/

        static bool CheckIP(string IPAddress)
    {
        // Check input is a valid IPAdress format
        string[] ipparts = IPAddress.Split('.');

        bool result = true;

        if (ipparts.Length == 4)
        {
            foreach (string part in ipparts)
            {
                int r;

                if (!int.TryParse(part, out r) || r < 0 || r > 255)
                {
                    result = false;
                    break;
                }
            }
        }
        else
        {
            result = false;
        }
        return result;
    }

        /* ********************************************************************************************************************************
        * EthernetSettings()
        * 
        * Routine to update the Ethernet settings on the device
        *  ********************************************************************************************************************************/
        void EthernetSettings()
    {
        short enabled;
        char ch;
        ushort length;
        ushort port = 0;

        System.Text.StringBuilder IPAddress = new System.Text.StringBuilder(20);

        length = (ushort)IPAddress.Capacity;

        //Display current ethernet settings
        status = Imports.IpDetails(_handle, out enabled, IPAddress, ref length, ref port, Imports.enIpDetailsType.PLCM3_IDT_GET);

        if (status != PICO_OK)
        {
            Console.WriteLine("IP details: Status = {0:X}", status);
            return;
        }

        Console.WriteLine("\nEthernet Settings\n");
        Console.WriteLine("Enabled:\t{0}", enabled);
        Console.WriteLine("IP Address:\t{0}", IPAddress);
        Console.WriteLine("Port:\t\t{0}", port);

        //Enter new settings
        Console.WriteLine("\nEdit settings? (Y/N)");
        ch = char.ToUpper(Console.ReadKey(true).KeyChar);

        if (ch == 'Y')
        {
            Console.WriteLine("Enable ethernet? (Y/N)\n");
            ch = char.ToUpper(Console.ReadKey(true).KeyChar);
            if (ch == 'N')
            {
                enabled = 0;
            }
            else
            {
                enabled = 1;

                Console.WriteLine("Enter IP address: ");
                string input = Console.ReadLine();

                if (CheckIP(input))
                {
                    System.Text.StringBuilder IPAddressWrite = new System.Text.StringBuilder(input);

                    Console.WriteLine("Enter port: ");

                    string portstr;
                    portstr = Console.ReadLine();


                    if (CheckPort(portstr, ref port))
                    {
                    
                        status = Imports.IpDetails(_handle, out enabled, IPAddressWrite, ref length, ref port, Imports.enIpDetailsType.PLCM3_IDT_SET);

                        if (status != PICO_OK)
                        {
                            Console.WriteLine("IP details: Status = (0:X}", status);
                            return;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Invalid Port");
                    }
                  
                }
                else
                {
                    Console.WriteLine("Invalid IP Address");
                }
            }
        }
    }

        /***********************************************************************************************
         * GetDeviceInfo()
         * Show information about device
         * 
        **********************************************************************************************/
        void GetDeviceInfo()
        {
            string[] description = {
                           "Driver Version    ",
                           "USB Version       ",
                           "Hardware Version  ",
                           "Variant Info      ",
                           "Batch and Serial  ", 
						   "Calibration Date  ", 
						   "Kernel Driver Ver "
                         };
            System.Text.StringBuilder line = new System.Text.StringBuilder(80);

            if (_handle >= 0)
            {

                for (int i = 0; i < 7; i++)
                {
                    short requiredSize;
                    Imports.GetUnitInfo(_handle, line, 80, out requiredSize, i);

                    Console.WriteLine("{0}: {1}", description[i], line);
                }
            }
        }


        /***********************************************************************************************
        * Run()
        * Show information about device
        * 
        **********************************************************************************************/
        public void Run()
        {
            char ch;
          

            GetDeviceInfo();


            do
            {
                Console.WriteLine("");
                Console.WriteLine("S:\tStart acquisition");
                Console.WriteLine("C:\tChannel Settings");
                Console.WriteLine("E:\tEthernet Settings");
                Console.WriteLine("X:\tExit\n\n");
                Console.WriteLine("\n");
                ch = char.ToUpper(Console.ReadKey(true).KeyChar);

                switch (ch)
                {
                    case 'S':
                       CollectData();
                        break;

                    case 'C':
                       ChannelSetUp();
                        break;

                    case 'E':
                        if (!USB)
                        {
                            Console.WriteLine("Connect via USB to set up ethernet.");
                            break;
                        }
                        EthernetSettings();
                        break;

                    case 'X':
                        break;

                    default:
                        Console.WriteLine("\nInvalid selection");
                        break;
                }

            } while (ch != 'X');
        }

        private ConsoleExample(short handle)
        {
            _handle = handle;
        }


         /***********************************************************************************************
         * Main()
         * 
         * 
         **********************************************************************************************/
        static void Main(string[] args)
        {
            short handle;
            char ch ;
            bool validSelection = false;
            string IPAddress = "";

           
        

            System.Text.StringBuilder str = new System.Text.StringBuilder(80);
            str = null;


            for (int chan = 0; chan < NUM_CHANNELS; chan++)
            {
                channelSettings[chan] = Imports.enPLCM3DataType.PLCM3_1_MILLIVOLT;
            }

            Console.WriteLine("PLCM3 driver example program");
            Console.WriteLine("Version 1.0\n");
      

            Console.WriteLine("\n");
            do
            {
                Console.WriteLine("");
                Console.WriteLine("Select Connection");
                Console.WriteLine("U:\tUSB");
                Console.WriteLine("E:\tEthernet");

                ch = char.ToUpper(Console.ReadKey(true).KeyChar);

                switch (ch)
                {
                    case 'U':
                        USB = true;
                        validSelection = true;
                        break;

                    case 'E':
                        USB = false;
                        validSelection = true;
                       
                        bool ValidIP = true;

                        do
                        {
                            Console.WriteLine("Enter IP Address of PLCM3 unit");
                            IPAddress = Console.ReadLine();
                            ValidIP = CheckIP(IPAddress);
                            {
                                if (!ValidIP)
                                    Console.WriteLine("Invalid IP address format.");
                            }
                        }
                        while(!ValidIP);
                        break;
                      

                    default:
                        Console.WriteLine("\nInvalid selection");
                        break;
                }

            } while (!validSelection);


            //open unit 
            Console.WriteLine("\n\nOpening the device...");

            if (USB)
                Imports.OpenUnit(out handle, str);
            else
            {
               status =  Imports.OpenUnitViaIp(out handle, str, IPAddress);
            }
            Console.WriteLine("Handle: {0}", handle);
            if (handle == 0)
            {
                Console.WriteLine("Unable to open device");
                Console.WriteLine("Error code : {0}", handle);
                WaitForKey();
            }
            else
            {
                Console.WriteLine("Device opened successfully\n");

                ConsoleExample consoleExample = new ConsoleExample(handle);
                consoleExample.Run();

                Imports.CloseUnit(handle);
            }
        }
    }
}

