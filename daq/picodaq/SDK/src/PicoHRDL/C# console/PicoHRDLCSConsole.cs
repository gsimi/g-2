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
 **************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;

namespace PicoHRDL
{
    class PicoHRDLCSConsole
    {
        private readonly short _handle;
        

        public PicoHRDLCSConsole(short handle)
        {
        // TODO: Complete member initialization
            _handle = handle;
        }

        

        static void Main(string[] args)
        {

            Console.WriteLine("C# PicoHRDL driver example program");

            short handle = Imports.HRDLOpenUnit();

            Console.WriteLine("Unit opened - handle {0}", handle);

            //PicoHRDLCSConsole picoHRDLConsole = new PicoHRDLCSConsole(handle);

            //GetDeviceInfo(handle);

            string[] description = {
                           "Driver Version    ",
                           "USB Version       ",
                           "Hardware Version  ",
                           "Variant Info      ",
                           "Serial            ",
                           "Cal Date          ",
                           "Kernel Ver        "
                         };

            System.Text.StringBuilder line = new System.Text.StringBuilder(80);

            if (handle >= 0)
            {
                for (int i = 0; i < 6; i++)
                {

                    Imports.GetUnitInfo(handle, line, 80, i);

                    Console.WriteLine("{0}: {1}", description[i], line);
                }
            }

            short returnSetMains = Imports.SetMains(handle, 0);   //Set noise rejection for 50Hz  
            
            short analogChannel = Imports.SetAnalogInChannel(handle, 1, 1, 0, 1); //set channel 1, enabled, range = 2500mV, single ended
            
            short returnInterval = Imports.SetInterval(handle, 80, 0); //sample interval time= 80ms, conversion time = 60ms
            //run = HRDLRun(handle, 10, 2);   // 10 samples, Continuous Stream mode

            int minAdc = 0;
            int maxAdc = 0;

            short returnAdcMaxMin = Imports.GetMinMaxAdcCounts(handle, out minAdc, out maxAdc, 1);

            Console.WriteLine("Max ADC: {0}", maxAdc);
            Console.WriteLine("Min ADC: {0}", minAdc);

            short status = Imports.HRDLCloseUnit(handle);

        }

        public void GetDeviceInfo(short handle)
        {
            short hrdl_handle = handle;

            string[] description = {
                           "Driver Version    ",
                           "USB Version       ",
                           "Hardware Version  ",
                           "Variant Info      ",
                           "Serial            ",
                           "Cal Date          ",
                           "Kernel Ver        "
                         };
            
            System.Text.StringBuilder line = new System.Text.StringBuilder(80);

            if (handle >= 0)
            {
                for (int i = 0; i < 6; i++)
                {
                    
                    //Imports.GetUnitInfo(hrdl_handle, line, 80, i);
                    
                    Console.WriteLine("{0}: {1}", description[i], line);
                }
            }

            char ch;


            do
            {
                Console.WriteLine("Press X to exit.");
                ch = char.ToUpper(Console.ReadKey(true).KeyChar);
            }

            while (ch != 'X');
          
        }

    }
}
