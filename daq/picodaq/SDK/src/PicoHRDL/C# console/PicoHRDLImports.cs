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
using System.Runtime.InteropServices;

namespace PicoHRDL
{
    class Imports
    {

        #region constants
        private const string _DRIVER_FILENAME = "PicoHRDL.dll";

        #endregion

        #region Driver Imports

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLOpenUnit")]
        public static extern short HRDLOpenUnit();

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLCloseUnit")]
        public static extern short HRDLCloseUnit(short handle);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLGetUnitInfo")]
        public static extern short GetUnitInfo(
            short handle,
            StringBuilder infoString,
            short stringLength,
            int info);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLSetMains")]
        public static extern short SetMains(
            short handle,
            short sixtyHertz);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLSetAnalogInChannel")]
        public static extern short SetAnalogInChannel(
            short handle,
            short channel,
            short enabled,
            short range,
            short singleEnded);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLSetInterval")]
        public static extern short SetInterval(
            short handle, 
            int sampleInterval_ms, 
            short conversionTime);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLGetMinMaxAdcCounts")]
        public static extern short GetMinMaxAdcCounts(
            short handle,
            out int minAdc,
            out int maxAdc,
            short channel);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLGetNumberOfEnabledChannels")]
        public static extern short GetNumberOfEnabledChannels(
            short handle,
            out short nEnabledChannels);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLGetSingleValue")]
        public static extern short GetSingleValue(
            short handle,
            short channel,
            short range,
            short conversionTime,
            short singleEnded,
            out short overflow,
            out long value);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLGetTimesAndValues")]
        public static extern short GetTimesAndValues(
            short handle,
            long[] times,
            long[] values,
            short[] overflow,
            long noOfValues);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLGetValues")]
        public static extern short GetValues(
            short handle,
            long[] values,
            short[] overflow,
            long noOfValues);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLReady")]
        public static extern short HRDLReady(
            short handle);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLRun")]
        public static extern short HRDLRun(
            short handle,
            long nValues,
            short method);

        [DllImport(_DRIVER_FILENAME, EntryPoint = "HRDLStop")]
        public static extern short HRDLStop(
            short handle);

        #endregion

    }
}
