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
 **************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace ps6000example
{
	class Imports
	{
		#region constants
		private const string _DRIVER_FILENAME = "ps6000.dll";

		public const int MaxValue = 32512;
		#endregion

		#region Driver enums

		public enum Channel : int
		{
			ChannelA,
			ChannelB,
			ChannelC,
			ChannelD,
			External,
			Aux,
			None,
		}

		public enum Range : int
		{
			Range_10MV,
			Range_20MV,
			Range_50MV,
			Range_100MV,
			Range_200MV,
			Range_500MV,
			Range_1V,
			Range_2V,
			Range_5V,
			Range_10V,
			Range_20V,
			Range_50V,
		}

		public enum ReportedTimeUnits : int
		{
			FemtoSeconds,
			PicoSeconds,
			NanoSeconds,
			MicroSeconds,
			MilliSeconds,
			Seconds,
		}

		public enum ThresholdMode : int
		{
			Level,
			Window
		}

		public enum ThresholdDirection : int
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

			None = Rising,
		}

		public enum PulseWidthType : int
		{
			None,
			LessThan,
			GreaterThan,
			InRange,
			OutOfRange
		}

		public enum TriggerState : int
		{
			DontCare,
			True,
			False
		}

		public enum PS6000DownSampleRatioMode : int
		{
			PS6000_RATIO_MODE_NONE,
			PS6000_RATIO_MODE_AGGREGATE,
			PS6000_RATIO_MODE_AVERAGE,
			PS6000_RATIO_MODE_DECIMATE,
			PS6000_RATIO_MODE_DISTRIBUTION
		}

		public enum PS6000BandwidthLimiter : int
		{
			PS6000_BW_FULL,
			PS6000_BW_20MHZ
		}

		public enum PS6000Coupling : int
		{
			PS6000_AC,
			PS6000_DC_1M,
			PS6000_DC_50R
		}

        public enum PS6000ExtraOperations : int
        {
	        PS6000_ES_OFF,
	        PS6000_WHITENOISE,
	        PS6000_PRBS // Pseudo-Random Bit Stream 
        }

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
		public delegate void ps6000BlockReady(short handle, short status, IntPtr pVoid);

		public delegate void ps6000StreamingReady(
												short handle,
												int noOfSamples,
												uint startIndex,
												short ov,
												uint triggerAt,
												short triggered,
												short autoStop,
												IntPtr pVoid);

		public delegate void ps6000DataReady(
												short handle,
												int noOfSamples,
												short overflow,
												uint triggerAt,
												short triggered,
												IntPtr pVoid);
		#endregion

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000OpenUnit")]
		public static extern short OpenUnit(out short handle, StringBuilder serial);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000CloseUnit")]
		public static extern short CloseUnit(short handle);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000RunBlock")]
		public static extern short RunBlock(
												short handle,
												uint noOfPreTriggerSamples,
												uint noOfPostTriggerSamples,
												uint timebase,
												short oversample,
												out int timeIndisposedMs,
												uint segmentIndex,
												ps6000BlockReady lpps6000BlockReady,
												IntPtr pVoid);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000Stop")]
		public static extern short Stop(short handle);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetChannel")]
		public static extern short SetChannel(
												short handle,
												Channel channel,
												short enabled,
												PS6000Coupling coupling,
												Range range,
												float analogueOffset,
												PS6000BandwidthLimiter bandwidth);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetDataBuffers")]
		public static extern short SetDataBuffers(
												short handle,
												Channel channel,
												short[] bufferMax,
												short[] bufferMin,
												uint bufferLth,
												PS6000DownSampleRatioMode mode);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetDataBuffer")]
        public static extern short SetDataBuffer(
                                                short handle,
                                                Channel channel,
                                                short[] buffer,
                                                uint bufferLth,
                                                PS6000DownSampleRatioMode mode);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetTriggerChannelDirections")]
		public static extern short SetTriggerChannelDirections(
												short handle,
												ThresholdDirection channelA,
												ThresholdDirection channelB,
												ThresholdDirection channelC,
												ThresholdDirection channelD,
												ThresholdDirection ext,
												ThresholdDirection aux);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000GetTimebase")]
		public static extern short GetTimebase(
											 short handle,
											 uint timebase,
											 uint noSamples,
											 out int timeIntervalNanoseconds,
											 short oversample,
											 out uint maxSamples,
											 uint segmentIndex);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000GetValues")]
		public static extern short GetValues(
				short handle,
				uint startIndex,
				ref uint noOfSamples,
				uint downSampleRatio,
				PS6000DownSampleRatioMode mode,
				uint segmentIndex,
				out short overflow);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetPulseWidthQualifier")]
		public static extern short SetPulseWidthQualifier(
			short handle,
			PwqConditions[] conditions,
			short numConditions,
			ThresholdDirection direction,
			uint lower,
			uint upper,
			PulseWidthType type);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetTriggerChannelProperties")]
		public static extern short SetTriggerChannelProperties(
			short handle,
			TriggerChannelProperties[] channelProperties,
			short numChannelProperties,
			short auxOutputEnable,
			int autoTriggerMilliseconds);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetTriggerChannelConditions")]
		public static extern short SetTriggerChannelConditions(
			short handle,
			TriggerConditions[] conditions,
			short numConditions);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetTriggerDelay")]
		public static extern short SetTriggerDelay(short handle, uint delay);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000GetUnitInfo")]
		public static extern short GetUnitInfo(short handle, StringBuilder infoString, short stringLength, out short requiredSize, uint info);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000RunStreaming")]
		public static extern short RunStreaming(
			short handle,
			ref uint sampleInterval,
			ReportedTimeUnits sampleIntervalTimeUnits,
			uint maxPreTriggerSamples,
			uint maxPostPreTriggerSamples,
			short autoStop,
			uint downSamplingRation,
			PS6000DownSampleRatioMode mode,
			uint overviewBufferSize);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000GetStreamingLatestValues")]
		public static extern short GetStreamingLatestValues(
			short handle,
			ps6000StreamingReady lpps6000StreamingReady,
			IntPtr pVoid);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetNoOfCaptures")]
		public static extern short SetNoOfRapidCaptures(
			short handle,
			uint nWaveforms);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000MemorySegments")]
		public static extern short MemorySegments(
			short handle,
			uint nSegments,
			out uint nMaxSamples);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetDataBufferBulk")]
		public static extern short SetDataBuffersRapid(
			short handle,
			Channel channel,
			short[] buffer,
			uint bufferLth,
			uint waveform,
            PS6000DownSampleRatioMode mode);

		[DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000GetValuesBulk")]
		public static extern short GetValuesRapid(
			short handle,
			ref uint noOfSamples,
			uint fromSegmentIndex,
			uint toSegmentIndex,
			uint	downSampleRatio,
			PS6000DownSampleRatioMode mode,
			short[] overflows);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetSigGenArbitrary")]
        public static extern short SetSigGenArbitrary(
           short handle,
           int offsetVoltage,
           uint pkTopk,
           uint startDeltaPhase,
           uint stopDeltaPhase,
           uint deltaPhaseIncrement,
           uint dwellCount,
           short [] arbitaryWaveForm,
           int arbitaryWaveFormSize,
           uint sweepType,
           PS6000ExtraOperations operation,
           uint indexMode,
           uint shots,
           uint sweeps,
           uint triggerType,
           uint triggerSource,
           short extInThreshold);

         [DllImport(_DRIVER_FILENAME, EntryPoint = "ps6000SetSigGenBuiltIn")]
        public static extern short SetSigGenBuiltIn(
           short handle,
           int offsetVoltage,
           uint pkTopk,
           short waveType,
           float startFrequency,
           float stopFrequency,
           float increment,
           float dwellTime,
           uint sweepType,
           PS6000ExtraOperations operation,
           uint shots,
           uint sweeps,
           uint triggerType,
           uint triggerSource,
           short extInThreshold);


            


		#endregion
	}
}
