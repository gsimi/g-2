﻿/**************************************************************************
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
 *   PS4000a driver using .NET
 *
 * Examples:
 *    Collect a stream of data immediately
 *    Collect a stream of data when a trigger event occurs
 *
 **************************************************************************/

using System;
using System.IO;
using System.Threading;

namespace PS4000AStreamingConsole
{
    struct ChannelSettings
    {
        public Imports.Range range;
        public bool enabled;
    }

    class StreamingCon
    {
        private readonly short _handle;
        int _channelCount;
        private ChannelSettings[] _channelSettings;


        short[][] appBuffers;
        short[][] buffers;

        uint[] inputRanges = { 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000 };
        bool _ready = false;
        short _trig = 0;
        uint _trigAt = 0;
        int _sampleCount;
        uint _startIndex;
        bool _autoStop;
        short _maxValue;

        public Imports.Range _firstRange;
        public Imports.Range _lastRange;

        /****************************************************************************
        * Callback
        * used by PS4000a data streaming collection calls, on receipt of data.
        * used to set global flags etc checked by user routines
        ****************************************************************************/
        void StreamingCallback(short handle,
                                int noOfSamples,
                                uint startIndex,
                                short overflow,
                                uint triggerAt,
                                short triggered,
                                short autoStop,
                                IntPtr pVoid)
        {
            // used for streaming
            _sampleCount = noOfSamples;
            _startIndex = startIndex;
            _autoStop = autoStop != 0;

            _ready = true;

            // flags to show if & where a trigger has occurred
            _trig = triggered;
            _trigAt = triggerAt;

            if (_sampleCount != 0)
            {
                for (int ch = 0; ch < _channelCount * 2; ch += 2)
                {
                    if (_channelSettings[(int)(Imports.Channel.CHANNEL_A + (ch / 2))].enabled)
                    {

                        Array.Copy(buffers[ch], _startIndex, appBuffers[ch], _startIndex, _sampleCount); //max
                        Array.Copy(buffers[ch + 1], _startIndex, appBuffers[ch + 1], _startIndex, _sampleCount); //min

                    }
                }
            }
        }

        /****************************************************************************
        * WaitForKey
        *  Waits for the user to press a key
        *  
        ****************************************************************************/
        private static void WaitForKey()
        {
            while (!Console.KeyAvailable) Thread.Sleep(100);

            if (Console.KeyAvailable)
            {
                Console.ReadKey(true); // clear the key
            }
        }


        /****************************************************************************
         * adc_to_mv
         *
         * Convert an 16-bit ADC count into millivolts
         ****************************************************************************/
        int adc_to_mv(int raw, int ch)
        {
            return (raw * (int) inputRanges[ch]) / _maxValue;
        }

        /****************************************************************************
         * mv_to_adc
         *
         * Convert a millivolt value into a 16-bit ADC count
         *
         *  (useful for setting trigger thresholds)
         ****************************************************************************/
        short mv_to_adc(short mv, short ch)
        {
            return (short)((mv * _maxValue) / inputRanges[ch]);
        }

        /****************************************************************************
       * Stream Data Handler
       * - Used by the two stream data examples - untriggered and triggered
       * Inputs:
       * - unit - the unit to sample on
       * - preTrigger - the number of samples in the pre-trigger phase 
       *					(0 if no trigger has been set)
       ***************************************************************************/
        void StreamDataHandler(uint preTrigger)
        {
            int tempBufferSize = 1024 * 100; /*  *100 is to make sure buffer large enough */

            appBuffers = new short[_channelCount * 2][];
            buffers = new short[_channelCount * 2][];

            short setAutoStop = 1;
            
            uint totalSamples = 0;
            uint triggeredAt = 0;
            uint sampleInterval = 1;
            uint downSampleRatio = 1;
            uint status;
            uint maxPostTriggerSamples = 1000000 - preTrigger;

            // Use Pinned Arrays for the application buffers
            PinnedArray<short>[] appBuffersPinned = new PinnedArray<short>[_channelCount * 2];

            for (int ch = 0; ch < _channelCount * 2; ch += 2) // create data buffers
            {
                if (_channelSettings[ch / 2].enabled == true)
                {
                    buffers[ch] = new short[tempBufferSize];
                    buffers[ch + 1] = new short[tempBufferSize];

                    appBuffers[ch] = new short[tempBufferSize];
                    appBuffers[ch + 1] = new short[tempBufferSize];

                    appBuffersPinned[ch] = new PinnedArray<short>(appBuffers[ch]);
                    appBuffersPinned[ch + 1] = new PinnedArray<short>(appBuffers[ch + 1]);

                    status = Imports.SetDataBuffers(_handle, (Imports.Channel)(ch / 2), buffers[ch], buffers[ch + 1], tempBufferSize, 0, Imports.DownSamplingMode.None);
                }
            }

            Console.WriteLine("Waiting for trigger...Press a key to abort");
            _autoStop = false;
            status = Imports.RunStreaming(_handle, ref sampleInterval, Imports.ReportedTimeUnits.MicroSeconds, preTrigger, maxPostTriggerSamples, setAutoStop, downSampleRatio, 
                                                Imports.DownSamplingMode.None, (uint)tempBufferSize);
            
            Console.WriteLine("Run Streaming : {0} ", status);

            Console.WriteLine("Streaming data...Press a key to abort");

            TextWriter writer = new StreamWriter("stream.txt", false);


            writer.Write("For each of the enabled Channels, results shown are....");
            writer.WriteLine();
            writer.WriteLine("Maximum Aggregated value ADC Count & mV, Minimum Aggregated value ADC Count & mV");
            writer.WriteLine();

            for (int ch = 0; ch < _channelCount; ch++)
            {
                if (_channelSettings[ch].enabled)
                {
                    writer.Write("Ch  Max ADC    Max mV   Min ADC    Min mV   ");
                }
            }
            writer.WriteLine();

            while (!_autoStop && !Console.KeyAvailable)
            {
                /* Poll until data is received. Until then, GetStreamingLatestValues wont call the callback */
                Thread.Sleep(0);
                _ready = false;
                status = Imports.GetStreamingLatestValues(_handle, StreamingCallback, IntPtr.Zero);

                Console.Write((status > 0 && status != 39 /*PICO_BUSY*/) ? "Status =  {0}\n" : "", status);

                if (_ready && _sampleCount > 0) /* can be ready and have no data, if autoStop has fired */
                {
                    if (_trig > 0)
                    {
                        triggeredAt = (uint)totalSamples + _trigAt;
                    }

                    totalSamples += (uint) _sampleCount;
                    Console.Write("\nCollected {0,4} samples, index = {1,5}, Total = {2,5}", _sampleCount, _startIndex, totalSamples);

                    if (_trig > 0)
                    {
                        Console.Write("\tTrig at Index {0}", triggeredAt);
                    }

                    for (uint i = _startIndex; i < (_startIndex + _sampleCount); i++)
                    {
                        for (int ch = 0; ch < _channelCount * 2; ch += 2)
                        {
                            if (_channelSettings[ch / 2].enabled)
                            {
                                writer.Write("Ch{0} {1,7}   {2,7}   {3,7}   {4,7}   ",
                                                            (char)('A' + (ch / 2)),
                                                            appBuffersPinned[ch].Target[i],
                                                            adc_to_mv(appBuffersPinned[ch].Target[i], (int)_channelSettings[(int)(Imports.Channel.CHANNEL_A + (ch / 2))].range),
                                                            appBuffersPinned[ch + 1].Target[i],
                                                            adc_to_mv(appBuffersPinned[ch + 1].Target[i], (int)_channelSettings[(int)(Imports.Channel.CHANNEL_A + (ch / 2))].range));
                            }
                        }

                        writer.WriteLine();
                    }
                }
            }

            if (Console.KeyAvailable)
            {
                Console.ReadKey(true); // clear the key
            }

            Imports.Stop(_handle);
            writer.Close();

            if (!_autoStop)
            {
                Console.WriteLine();
                Console.WriteLine("Data collection aborted - press any key to continue.");
                WaitForKey();
            }
        }

        /****************************************************************************
        * Select input voltage ranges for each channel
        ****************************************************************************/
        void SetVoltages()
        {
            bool valid = false;
            bool allChannelsOff = true;
            uint status;

            Console.WriteLine("Available voltage ranges are....\n");
            /* See what ranges are available... */
            for (int i = (int)_firstRange; i <= (int)_lastRange; i++)
            {
                Console.WriteLine("{0} . {1} mV", i, inputRanges[i]);
            }

            /* Ask the user to select a range */
            Console.WriteLine("Specify voltage range ({0}..{1})", (int)_firstRange, (int)_lastRange);
            Console.WriteLine("99 - switches channel off");
            
            do
            {
                for (int ch = 0; ch < _channelCount; ch++)
                {
                    Console.WriteLine("");
                    uint range = 8;

                    do
                    {
                        try
                        {
                            Console.WriteLine("Channel: {0}", (char)('A' + ch));
                            range = uint.Parse(Console.ReadLine());
                            valid = true;
                        }
                        catch (FormatException e)
                        {
                            valid = false;
                            Console.WriteLine("Error: " + e.Message);
                        }

                    } while ((range != 99 && (range < (uint)_firstRange || range > (uint)_lastRange) || !valid));


                    if (range != 99)
                    {
                        status = Imports.SetChannel(_handle, Imports.Channel.CHANNEL_A + ch, 1, 0, (Imports.Range)range, 0);
                        _channelSettings[ch].enabled = true;
                        _channelSettings[ch].range = (Imports.Range)range;
                        Console.WriteLine(" = {0} mV", inputRanges[range]);
                        allChannelsOff = false;
                    }
                    else
                    {
                        status = Imports.SetChannel(_handle, Imports.Channel.CHANNEL_A + ch, 0, 0, Imports.Range.Range_1V, 0);
                        _channelSettings[ch].enabled = false;
                        Console.WriteLine("Channel Switched off");

                    }

                    if (status != 0)
                    {
                        Console.WriteLine("Error setting channels\n Error code : {0}", status);
                    }
                }
                Console.Write(allChannelsOff ? "At least one channels must be enabled\n" : "");
            } while (allChannelsOff);
        }

        /****************************************************************************
        * Initialise unit' structure with Variant specific defaults
        ****************************************************************************/
        void GetDeviceInfo()
        {
            Imports.MaximumValue(_handle, out _maxValue);

            string[] description = {
                                       "Driver Version",
                                       "USB Version",
                                       "Hardware Version",
                                       "Variant Info",
                                       "Serial",
                                       "Cal Date",
                                       "Kernel",
                                       "Digital H/W",
                                       "Analogue H/W",
                                       "Firmware 1",
                                       "Firmware 2"
                                    };

            System.Text.StringBuilder line = new System.Text.StringBuilder(80);

            if (_handle >= 0)
            {
                for (int i = 0; i < 11; i++)
                {
                    short requiredSize;
                    Imports.GetUnitInfo(_handle, line, 80, out requiredSize, (uint) i);
                    Console.WriteLine("{0}: {1}", description[i], line);

                    if (i == 3) // Variant information
                    {
                        _channelCount = int.Parse(line[1].ToString());
                        _channelSettings = new ChannelSettings[_channelCount];

                        if (_channelCount == 8)
                        {
                            _firstRange = Imports.Range.Range_10MV;
                            _lastRange = Imports.Range.Range_50V;
                        }
                        else
                        {
                            _firstRange = Imports.Range.Range_50MV;
                            _lastRange = Imports.Range.Range_200V;
                        }
                    }
                }

                Console.WriteLine();
                Console.WriteLine("Channel Voltage Ranges:");
                Console.WriteLine();

                for (int ch = 0; ch < _channelCount; ch++)
                {
                    Imports.SetChannel(_handle, Imports.Channel.CHANNEL_A + ch, 1, 0, Imports.Range.Range_5V, 0);
                    _channelSettings[ch].enabled = true;
                    _channelSettings[ch].range = Imports.Range.Range_5V;
                    Console.WriteLine("Channel {0}: 5V", (char)('A' + ch));
                }
            }
        }

        /****************************************************************************
    * CollectStreamingImmediate
    *  this function demonstrates how to collect a stream of data
    *  from the unit (start collecting immediately)
    ***************************************************************************/
        void CollectStreamingImmediate()
        {
            Console.WriteLine("Collect streaming...");
            Console.WriteLine("Data is written to disk file (stream.txt)");
            Console.WriteLine("Press a key to start");
            WaitForKey();

            /* Trigger disabled	*/
            Imports.SetSimpleTrigger(_handle, 0, Imports.Channel.CHANNEL_A, 0, Imports.ThresholdDirection.None, 0, 0);

            StreamDataHandler(0);
        }

        /****************************************************************************
         * CollectStreamingTriggered
         *  this function demonstrates how to collect a stream of data
         *  from the unit (start collecting on trigger)
         ***************************************************************************/
        void CollectStreamingTriggered()
        {
            short triggerVoltage = mv_to_adc(1000, (short)_channelSettings[(int)Imports.Channel.CHANNEL_A].range); // ChannelInfo stores ADC counts            

            Console.WriteLine("Collect streaming triggered...");
            Console.WriteLine("Data is written to disk file (stream.txt)");
            Console.WriteLine("Press a key to start");
            WaitForKey();

            /* Trigger enabled
             * Rising edge
             * Threshold = 1000mV */

            Imports.SetSimpleTrigger(_handle, triggerVoltage, Imports.Channel.CHANNEL_A, 0, Imports.ThresholdDirection.Rising, 0, 0);

            StreamDataHandler(100000);
        }

        /*************************************************************************************
        * Run
        *  main menu
        *  
        **************************************************************************************/
        public void Run()
        {
            GetDeviceInfo();

            // main loop - read key and call routine
            char ch = ' ';

            while (ch != 'X')
            {
                Console.WriteLine("\n");
                Console.WriteLine("S - Immediate streaming         V - Set voltages");
                Console.WriteLine("W - Triggered streaming         ");
                Console.WriteLine("X - Exit");
                Console.WriteLine();
                Console.WriteLine("Operation:");

                ch = char.ToUpper(Console.ReadKey(true).KeyChar);

                Console.WriteLine("\n");
                switch (ch)
                {
                    case 'S':
                        CollectStreamingImmediate();
                        break;

                    case 'W':
                        CollectStreamingTriggered();
                        break;

                    case 'V':
                        SetVoltages();
                        break;

                    case 'X':
                        /* Handled by outer loop */
                        break;

                    default:
                        Console.WriteLine("Invalid operation");
                        break;
                }
            }

        }

        private StreamingCon(short handle)
        {
            _handle = handle;
        }

        static void Main()
        {
            Console.WriteLine("C# PS4000A driver streaming example program");
            Console.WriteLine("Version 1.2\n");

            // Open unit and show splash screen
            Console.WriteLine("\n\nOpening the device...");
            short handle;
            uint status = Imports.OpenUnit(out handle, null);
            Console.WriteLine("Handle: {0}", handle);

            if (status != 0 && handle != 0)
            {
                status = Imports.ps4000aChangePowerSource(handle, status);
            }

            if (status != 0)
            {
                Console.WriteLine("Unable to open device");
                Console.WriteLine("Error code : {0}", status);
                WaitForKey();
            }
            else
            {
                Console.WriteLine("Device opened successfully\n");

                StreamingCon consoleExample = new StreamingCon(handle);
                consoleExample.Run();

                Imports.CloseUnit(handle);
            }
        }
    }
}
