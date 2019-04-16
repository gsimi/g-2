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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace PS5000A
{
    public partial class PS5000ABlockForm : Form
    {
        private short _handle;
        public const int BUFFER_SIZE = 1024;
        public const int MAX_CHANNELS = 4;
        public const int QUAD_SCOPE = 4;
        public const int DUAL_SCOPE = 2;


        uint _timebase = 5;
        short _oversample = 1;
        bool _scaleVoltages = true;

        ushort[] inputRanges = { 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000 };
        bool _ready = false;
        short _trig = 0;
        uint _trigAt = 0;
        int _sampleCount = 0;
        uint _startIndex = 0;
        bool _autoStop;
        //private ChannelSettings[] _channelSettings;
        private int _channelCount;
        private Imports.Range _firstRange;
        private Imports.Range _lastRange;
        private int _digitalPorts;
        private Imports.ps5000aBlockReady _callbackDelegate;
        private string StreamFile = "stream.txt";
        private string BlockFile = "block.txt";

        public PS5000ABlockForm()
        {
            InitializeComponent();

            comboRangeA.DataSource = System.Enum.GetValues(typeof(Imports.Range));
            comboRangeB.DataSource = System.Enum.GetValues(typeof(Imports.Range));
            comboRangeC.DataSource = System.Enum.GetValues(typeof(Imports.Range));
            comboRangeD.DataSource = System.Enum.GetValues(typeof(Imports.Range));
        }

       private void BlockCallback(short handle, short status, IntPtr pVoid)
        {
            // flag to say done reading data
            if (status != (short)Imports.PICO_CANCELLED)
                _ready = true;
        }

       private short SetTrigger(Imports.TriggerChannelProperties[] channelProperties,
           short nChannelProperties,
           Imports.TriggerConditions[] triggerConditions,
           short nTriggerConditions,
           Imports.ThresholdDirection[] directions,
           uint delay,
           short auxOutputEnabled,
           int autoTriggerMs)
       {
           short status;

           if (
             (status = Imports.SetTriggerChannelProperties(_handle, channelProperties, nChannelProperties, auxOutputEnabled,
                                                  autoTriggerMs)) != 0)
           {
               return status;
           }

           if ((status = Imports.SetTriggerChannelConditions(_handle, triggerConditions, nTriggerConditions)) != 0)
           {
               return status;
           }

           if (directions == null) directions = new Imports.ThresholdDirection[] { Imports.ThresholdDirection.None, 
        Imports.ThresholdDirection.None, Imports.ThresholdDirection.None, Imports.ThresholdDirection.None, 
        Imports.ThresholdDirection.None, Imports.ThresholdDirection.None};

           if ((status = Imports.SetTriggerChannelDirections(_handle,
                                                             directions[(int)Imports.Channel.ChannelA],
                                                             directions[(int)Imports.Channel.ChannelB],
                                                             directions[(int)Imports.Channel.ChannelC],
                                                             directions[(int)Imports.Channel.ChannelD],
                                                             directions[(int)Imports.Channel.External],
                                                             directions[(int)Imports.Channel.Aux])) != 0)
           {
               return status;
           }

           if ((status = Imports.SetTriggerDelay(_handle, delay)) != 0)
           {
               return status;
           }



           return status;
       }


       private void buttonOpen_Click(object sender, EventArgs e)
        {
            StringBuilder UnitInfo = new StringBuilder(80);

            short handle;

            string[] description = {
                           "Driver Version    ",
                           "USB Version       ",
                           "Hardware Version  ",
                           "Variant Info      ",
                           "Serial            ",
                           "Cal Date          ",
                           "Kernel Ver        ",
                           "Digital Hardware  ",
                           "Analogue Hardware "
                         };

            Imports.DeviceResolution resolution = Imports.DeviceResolution.PS5000A_DR_16BIT;
            //Imports.DeviceResolution resolution = Imports.DeviceResolution.PS5000A_DR_8BIT;


            if (_handle > 0)
            {
                Imports.CloseUnit(_handle);
                textBoxUnitInfo.Text = "";
                _handle = 0;
                buttonOpen.Text = "Open";
            }
            else
            {
                short status = Imports.OpenUnit(out handle, null, resolution);
                if( handle > 0) {
                    _handle = handle;

                    textBoxUnitInfo.Text = "Handle            " + _handle.ToString() + "\r\n";

                    for (int i = 0; i < 9; i++)
                    {
                        short requiredSize;
                        Imports.GetUnitInfo(_handle, UnitInfo, 80, out requiredSize, i);
                        textBoxUnitInfo.AppendText(description[i] + UnitInfo + "\r\n");
                    }
                    buttonOpen.Text = "Close";
                }
            }
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            short status;
            status = Imports.SetChannel(_handle, Imports.Channel.ChannelA, 1, 1, Imports.Range.Range_2V, 0);
            status = Imports.SetChannel(_handle, Imports.Channel.ChannelB, 0, 1, Imports.Range.Range_2V, 0);
            status = Imports.SetChannel(_handle, Imports.Channel.ChannelC, 0, 1, Imports.Range.Range_2V, 0);
            status = Imports.SetChannel(_handle, Imports.Channel.ChannelD, 0, 1, Imports.Range.Range_2V, 0);

            short enable = 0;
            uint delay = 0;
            short threshold = 25000;
            short auto = 0;

            status = Imports.SetSimpleTrigger(_handle, enable, Imports.Channel.ChannelA, threshold, Imports.ThresholdDirection.Rising, delay, auto);

            _ready = false;
            _callbackDelegate = BlockCallback;
            _channelCount = 4;
            string data;
            int x;

            textMessage.Clear();
            textData.Clear();

            bool retry;
            uint sampleCount = 1000;
            PinnedArray<short>[] minPinned = new PinnedArray<short>[_channelCount];
            PinnedArray<short>[] maxPinned = new PinnedArray<short>[_channelCount];

            int timeIndisposed;
                short[] minBuffers = new short[sampleCount];
                short[] maxBuffers = new short[sampleCount];
                minPinned[0] = new PinnedArray<short>(minBuffers);
                maxPinned[0] = new PinnedArray<short>(maxBuffers);
                status = Imports.SetDataBuffers(_handle, Imports.Channel.ChannelA, maxBuffers, minBuffers, (int)sampleCount, 0, Imports.RatioMode.None);
                textMessage.AppendText("BlockData\n");

            /*  find the maximum number of samples, the time interval (in timeUnits),
             *		 the most suitable time units, and the maximum _oversample at the current _timebase*/
            int timeInterval;
            int maxSamples;
            while (Imports.GetTimebase(_handle, _timebase, (int)sampleCount, out timeInterval, out maxSamples, 0) != 0)
            {
                textMessage.AppendText("Timebase selection\n");
                _timebase++;

            }
            textMessage.AppendText("Timebase Set\n");

            /* Start it collecting, then wait for completion*/
            _ready = false;
            _callbackDelegate = BlockCallback;

            do
            {
                retry = false;
                status = Imports.RunBlock(_handle, 0, (int)sampleCount, _timebase, out timeIndisposed, 0, _callbackDelegate, IntPtr.Zero);
                if (status == (short)Imports.PICO_POWER_SUPPLY_CONNECTED || status == (short)Imports.PICO_POWER_SUPPLY_NOT_CONNECTED || status == (short)Imports.PICO_POWER_SUPPLY_UNDERVOLTAGE)
                {
                    retry = true;
                }
                else
                {
                    textMessage.AppendText("Run Block Called\n");
                }
            }
            while (retry);

            textMessage.AppendText("Waiting for Data\n");

            while (!_ready)
            {
                Thread.Sleep(100);
            }

            Imports.Stop(_handle);

            if (_ready)
            {
                short overflow;
                status = Imports.GetValues(_handle, 0, ref sampleCount, 1, Imports.DownSamplingMode.None, 0, out overflow);
                if (status == (short)Imports.PICO_OK)
                {
                    textMessage.AppendText("Have Data\n");
                    for (x = 0; x < sampleCount; x++)
                    {
                       data = maxBuffers[x].ToString();
                       textData.AppendText(data);
                       textData.AppendText("\n");
                    }


                }
                else
                {
                    textMessage.AppendText("No Data\n");

                }
            }
            else
            {
                textMessage.AppendText("data collection aborted\n");
            }

            Imports.Stop(_handle);

                foreach (PinnedArray<short> p in minPinned)
                {
                    if (p != null)
                        p.Dispose();
                }
                foreach (PinnedArray<short> p in maxPinned)
                {
                    if (p != null)
                        p.Dispose();
                }
        }

    }
}
