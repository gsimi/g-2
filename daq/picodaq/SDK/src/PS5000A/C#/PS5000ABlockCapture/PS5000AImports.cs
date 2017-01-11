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
 *   the PS5000A scopes. It also has the enums and structs required
 *   by the (wrapped) function calls.
 **************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace PS5000AStreamingConsole
{
	class Imports
	{
		#region constants
		private const string _DRIVER_FILENAME = "ps5000A.dll";

		public const int MaxValue = 32512;
        public const int MaxLogicLevel = 32767;

        public const ulong PICO_OK = 0;
        public const ulong PICO_CANCELLED = 0x0000003AUL;
        public const ulong PICO_POWER_SUPPLY_CONNECTED = 0x00000119UL;
        public const ulong PICO_POWER_SUPPLY_NOT_CONNECTED = 0x0000011AUL;
        public const ulong PICO_POWER_SUPPLY_REQUEST_INVALID = 0x0000011BUL;
        public const ulong PICO_POWER_SUPPLY_UNDERVOLTAGE = 0x0000011CUL;
		public const ulong PICO_USB3_0_DEVICE_NON_USB3_0_PORT = 0x0000011EUL;
        #endregion

		#region Driver enums

		public enum Channel : uint
		{
			ChannelA,
			ChannelB,
			ChannelC,
			ChannelD,
			External,
			Aux,
			None
		}

		public enum Range : uint
		{
			Range_10mV,
			Range_20mV,
			Range_50mV,
			Range_100mV,
			Range_200mV,
			Range_500mV,
			Range_1V,
			Range_2V,
			Range_5V,
			Range_10V,
			Range_20V,
            Range_MAX_RANGE
		}

		public enum ReportedTimeUnits : uint
		{
			FemtoSeconds,
			PicoSeconds,
			NanoSeconds,
			MicroSeconds,
			MilliSeconds,
			Seconds,
		}

		public enum ThresholdMode : uint
		{
			Level,
			Window
		}

		public enum ThresholdDirection : uint
		{
			// Values for level threshold mode
			//
			Above,
			Below,
			Rising,
			Falling,
			RisingOrFalling,

			// Values for window threshold mode
			//
			Inside = Above,
			Outside = Below,
			Enter = Rising,
			Exit = Falling,
			EnterOrExit = RisingOrFalling,
			PositiveRunt = 9,
			NegativeRunt,

			None = Rising,
		}

		public enum DownSamplingMode : uint
		{
			None,
			Aggregate
		}

		public enum PulseWidthType : uint
		{
			None,
			LessThan,
			GreaterThan,
			InRange,
			OutOfRange
		}

		public enum TriggerState : uint
		{
			DontCare,
			True,
			False,
		}

        public enum RatioMode : uint
        {
            None,
            Aggregate,
            Average,
            Decimate
        }

        public enum DeviceResolution : uint
        {
	        PS5000A_DR_8BIT,
	        PS5000A_DR_12BIT,
	        PS5000A_DR_14BIT,
	        PS5000A_DR_15BIT,
	        PS5000A_DR_16BIT
        }

 
        #endregion

        # region structs

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
		public struct TriggerChannelProperties
		{
			public short ThresholdMajor;
			public ushort HysteresisMajor;
			public short ThresholdMinor;
			public ushort HysteresisMinor;
			public Channel Channel;
			public ThresholdMode ThresholdMode;


			public TriggerChannelProperties(
				short thresholdMajor,
				ushort hysteresisMajor,
				short thresholdMinor,
				ushort hysteresisMinor,
				Channel channel,
				ThresholdMode thresholdMode)
			{
				this.ThresholdMajor = thresholdMajor;
				this.HysteresisMajor = hysteresisMajor;
				this.ThresholdMinor = thresholdMinor;
				this.HysteresisMinor = hysteresisMinor;
				this.Channel = channel;
				this.ThresholdMode = thresholdMode;
			}
		}

		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		public struct TriggerConditions
		{
			public TriggerState ChannelA;
			public TriggerState ChannelB;
			public TriggerState ChannelC;
			public TriggerState ChannelD;
			public TriggerState External;
			public TriggerState Aux;
			public TriggerState Pwq;

			public TriggerConditions(
				TriggerState channelA,
				TriggerState channelB,
				TriggerState channelC,
				TriggerState channelD,
				TriggerState external,
				TriggerState aux,
				TriggerState pwq)
			{
				this.ChannelA = channelA;
				this.ChannelB = channelB;
				this.ChannelC = channelC;
				this.ChannelD = channelD;
				this.External = external;
				this.Aux = aux;
				this.Pwq = pwq;
			}
		}

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct TriggerConditionsV2
        {
            public TriggerState ChannelA;
            public TriggerState ChannelB;
            public TriggerState ChannelC;
            public TriggerState ChannelD;
            public TriggerState External;
            public TriggerState Aux;
            public TriggerState Pwq;
             public TriggerState Digital;

            public TriggerConditionsV2(
                TriggerState channelA,
                TriggerState channelB,
                TriggerState channelC,
                TriggerState channelD,
                TriggerState external,
                TriggerState aux,
                TriggerState pwq,
                TriggerState digital)
            {
                this.ChannelA = channelA;
                this.ChannelB = channelB;
                this.ChannelC = channelC;
                this.ChannelD = channelD;
                this.External = external;
                this.Aux = aux;
                this.Pwq = pwq;
                this.Digital = digital;
            }
        }

		[StructLayout(LayoutKind.Sequential, Pack = 1)]
		public struct PwqConditions
		{
			public TriggerState ChannelA;
			public TriggerState ChannelB;
			public TriggerState ChannelC;
			public TriggerState ChannelD;
			public TriggerState External;
			public TriggerState Aux;

			public PwqConditions(
				TriggerState channelA,
				TriggerState channelB,
				TriggerState channelC,
				TriggerState channelD,
				TriggerState external,
				TriggerState aux)
			{
				this.ChannelA = channelA;
				this.ChannelB = channelB;
				this.ChannelC = channelC;
				this.ChannelD = channelD;
				this.External = external;
				this.Aux = aux;
			}
		}

        #endregion
        
        #region Driver Imports
        #region Callback delegates
        public delegate void ps5000aBlockReady(short handle, short status, IntPtr pVoid);

		public delegate void ps5000aStreamingReady(
												short handle,
												int noOfSamples,
												uint startIndex,
												short ov,
												uint triggerAt,
												short triggered,
												short autoStop,
												IntPtr pVoid);

		public delegate void ps5000DataReady(
												short handle,
                                                short status,
												int noOfSamples,
												short overflow,
												IntPtr pVoid);
		#endregion

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aOpenUnit")]
		public static extern short OpenUnit(out short handle, StringBuilder serial, DeviceResolution resolution);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetDeviceResolution")]
        public static extern short SetDeviceResolution(short handle, DeviceResolution resolution);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aCloseUnit")]
		public static extern short CloseUnit(short handle);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aRunBlock")]
		public static extern short RunBlock(
												short handle,
												int noOfPreTriggerSamples,
												int noOfPostTriggerSamples,
												uint timebase,
                                                out int timeIndisposedMs,
                                                uint segmentIndex,
												ps5000aBlockReady lpps5000aBlockReady,
												IntPtr pVoid);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aStop")]
		public static extern short Stop(short handle);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetChannel")]
		public static extern short SetChannel(
												short handle,
												Channel channel,
												short enabled,
												short dc,
												Range range,
                                                float analogueOffset);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aMaximumValue")]
        public static extern short MaximumValue(
                                                short handle,
                                                out short value);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetDataBuffer")]
		public static extern short SetDataBuffer(
												short handle,
												Channel channel,
												short[] buffer,
												int bufferLth,
                                                uint segmentIndex,
                                                RatioMode  ratioMode);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetDataBuffers")]
        public static extern short SetDataBuffers(
                                                short handle,
                                                Channel channel,
                                                short[] bufferMax,
                                                short[] bufferMin,
                                                int bufferLth,
                                                uint segmentIndex,
                                                RatioMode ratioMode);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetTriggerChannelDirections")]
		public static extern short SetTriggerChannelDirections(
												short handle,
												ThresholdDirection channelA,
												ThresholdDirection channelB,
												ThresholdDirection channelC,
												ThresholdDirection channelD,
												ThresholdDirection ext,
												ThresholdDirection aux);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aGetTimebase")]
		public static extern short GetTimebase(
											 short handle,
											 uint timebase,
											 int noSamples,
											 out int timeIntervalNanoseconds,
											 out int maxSamples,
											 uint segmentIndex);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aGetValues")]
		public static extern short GetValues(
				short handle,
				uint startIndex,
				ref uint noOfSamples,
				uint downSampleRatio,
				DownSamplingMode downSampleRatioMode,
				uint segmentIndex,
				out short overflow);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetPulseWidthQualifier")]
		public static extern short SetPulseWidthQualifier(
			short handle,
			PwqConditions[] conditions,
			short nConditions,
			ThresholdDirection direction,
			uint lower,
			uint upper,
			PulseWidthType type);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetSimpleTrigger")]
        public static extern short SetSimpleTrigger(
            short handle,
            short enable,
            Channel channel,
            short threshold,
            ThresholdDirection direction,
            uint delay,
            short auto);


		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetTriggerChannelProperties")]
		public static extern short SetTriggerChannelProperties(
			short handle,
			TriggerChannelProperties[] channelProperties,
			short nChannelProperties,
			short auxOutputEnable,
			int autoTriggerMilliseconds);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetTriggerChannelConditions")]
		public static extern short SetTriggerChannelConditions(
			short handle,
			TriggerConditions[] conditions,
			short nConditions);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetTriggerDelay")]
		public static extern short SetTriggerDelay(short handle, uint delay);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aGetUnitInfo")]
		public static extern short GetUnitInfo(
            short handle, 
            StringBuilder infoString, 
            short stringLength, 
            out short requiredSize, 
            int info);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aRunStreaming")]
		public static extern short RunStreaming(
			short handle,
			ref uint sampleInterval,
			ReportedTimeUnits sampleIntervalTimeUnits,
			uint maxPreTriggerSamples,
			uint maxPostPreTriggerSamples,
			short autoStop,
			uint downSamplingRatio,
            RatioMode downSampleRatioMode,
			uint overviewBufferSize);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aGetStreamingLatestValues")]
		public static extern short GetStreamingLatestValues(
			short handle,
			ps5000aStreamingReady lpps5000aStreamingReady,
			IntPtr pVoid);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aSetNoOfCaptures")]
		public static extern short SetNoOfRapidCaptures(
			short handle,
			uint nCaptures);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aMemorySegments")]
		public static extern short MemorySegments(
			short handle,
			uint nSegments,
			out int nMaxSamples);


		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aGetValuesBulk")]
		public static extern short GetValuesRapid(
			short handle,
			ref uint noOfSamples,
			uint fromSegmentIndex,
			uint toSegmentIndex,
            uint downSampleRatio,
            DownSamplingMode downSampleRatioMode,
			short[] overflow);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "ps5000aChangePowerSource")]
        public static extern short ChangePowerSource(
            short handle,
            short status);

		#endregion
	}
}
