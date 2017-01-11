'========================================================================================
'	Filename:			PS5000aVBCon.vb
'
'	Purpose
'	This file demonstrates how to use VB .NET on the PS5000 series scopes using the ps5000a driver.
'   It shows how to connect to a device, display device information and collect data in 
'   block mode.
'
'   This example is based on the PicoScope 4000 series example by CPY
'
'   (C) Pico Technology 2013 - 2014
'
'	13-Nov-14	Revision 1		HSM         Initial version. 
'   27-Nov-14   Revision 1.1    HSM         Added block mode capture
'
'========================================================================================
Imports System.IO

Structure TRIGGER_CHANNEL_PROPERTIES
    Dim thresholdUpper As Short
    Dim thresholdUpperHysteresis As UShort
    Dim thresholdLower As Short
    Dim thresholdLowerHysteresis As UShort
    Dim Channel As Short
    Dim thresholdMode As Short
End Structure


Enum BandwidthLimiter
    PS5000A_BW_FULL
    PS5000A_BW_20MHZ
End Enum

Enum Channel
    PS5000A_CHANNEL_A
    PS5000A_CHANNEL_B
    PS5000A_CHANNEL_C
    PS5000A_CHANNEL_D
    PS5000A_EXTERNAL
End Enum

Enum ChannelBufferIndex
    PS5000A_CHANNEL_A_MAX
    PS5000A_CHANNEL_A_MIN
    PS5000A_CHANNEL_B_MAX
    PS5000A_CHANNEL_B_MIN
    PS5000A_CHANNEL_C_MAX
    PS5000A_CHANNEL_C_MIN
    PS5000A_CHANNEL_D_MAX
    PS5000A_CHANNEL_D_MIN
End Enum

Enum CouplingMode
    PS5000A_AC
    PS5000A_DC
End Enum

Enum DeviceResolution
    PS5000A_DR_8BIT
    PS5000A_DR_12BIT
    PS5000A_DR_14BIT
    PS5000A_DR_15BIT
    PS5000A_DR_16BIT
End Enum

Enum ExtraOperations
    PS5000A_ES_OFF
    PS5000A_WHITENOISE
    PS5000A_PRBS 'Pseudo-Random Bit Stream 
End Enum

Enum PulseWidthType
    PS5000A_PW_TYPE_NONE
    PS5000A_PW_TYPE_LESS_THAN
    PS5000A_PW_TYPE_GREATER_THAN
    PS5000A_PW_TYPE_IN_RANGE
    PS5000A_PW_TYPE_OUT_OF_RANGE
End Enum

Enum RatioMode
    PS5000A_RATIO_MODE_NONE = 0
    PS5000A_RATIO_MODE_AGGREGATE = 1
    PS5000A_RATIO_MODE_DECIMATE = 2
    PS5000A_RATIO_MODE_AVERAGE = 4
End Enum

Enum SigGenTrigSource
    PS5000A_SIGGEN_NONE
    PS5000A_SIGGEN_SCOPE_TRIG
    PS5000A_SIGGEN_AUX_IN
    PS5000A_SIGGEN_EXT_IN
    PS5000A_SIGGEN_SOFT_TRIG
End Enum

Enum SigGenTrigType
    PS5000A_SIGGEN_RISING
    PS5000A_SIGGEN_FALLING
    PS5000A_SIGGEN_GATE_HIGH
    PS5000A_SIGGEN_GATE_LOW
End Enum

Enum SweepType
    PS5000A_UP
    PS5000A_DOWN
    PS5000A_UPDOWN
    PS5000A_DOWNUP
End Enum

Enum ThresholdDirection
    'Above=0, Below=1, Rising / None=2, Falling=3, Rising_Or_Falling=4, Above_Lower=5, Below_Lower=6, Rising_Lower=7, Falling_Lower=8,
    PS5000A_ABOVE
    PS5000A_BELOW
    PS5000A_RISING
    PS5000A_NONE = PS5000A_RISING ' no trigger set
    PS5000A_FALLING
    PS5000A_RISING_OR_FALLING
    PS5000A_ABOVE_LOWER
    PS5000A_BELOW_LOWER
    PS5000A_RISING_LOWER
    PS5000A_FALLING_LOWER
    PS5000A_INSIDE = PS5000A_ABOVE
    PS5000A_OUTSIDE = PS5000A_BELOW
    PS5000A_ENTER = PS5000A_RISING
    PS5000A_EXIT = PS5000A_FALLING
    PS5000A_ENTER_OR_EXIT = PS5000A_RISING_OR_FALLING
    PS5000A_POSITIVE_RUNT = 9
    PS5000A_NEGATIVE_RUNT
End Enum

Enum ThresholdMode
    LEVEL
    WINDOW
End Enum

Enum TimeUnits
    PS5000A_FS
    PS5000A_PS
    PS5000A_NS
    PS5000A_US
    PS5000A_MS
    PS5000A_S
End Enum

Enum TriggerState
    CONDITION_DONT_CARE
    CONDITION_TRUE
    CONDITION_FALSE
    CONDITION_MAX
End Enum

Enum VoltageRange
    PS5000A_10MV
    PS5000A_20MV
    PS5000A_50MV
    PS5000A_100MV
    PS5000A_200MV
    PS5000A_500MV
    PS5000A_1V
    PS5000A_2V
    PS5000A_5V
    PS5000A_10V
    PS5000A_20V
    PS5000A_50V
    PS5000A_100V
End Enum

Enum WaveType
    PS5000A_SINE
    PS5000A_SQUARE
    PS5000A_TRIANGLE
    PS5000A_RAMP_UP
    PS5000A_RAMP_DOWN
    PS5000A_SINC
    PS5000A_GAUSSIAN
    PS5000A_HALF_SINE
    PS5000A_DC_VOLTAGE
    PS5000A_WHITE_NOISE
    PS5000A_MAX_WAVE_TYPES
End Enum


Module PS5000aVBCon

    Const PICO_OK = 0                       ' PICO_OK returned when API call succeeds

    Dim inputRanges() As Integer = New Integer(12) {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000} ' ranges in mV

    Dim mvConvert = True        ' flag to show if values get converted to mV or stay as ADC counts


    Declare Function ps5000aOpenUnit Lib "ps5000a.dll" (ByRef handle As Short, ByVal serial As String, ByVal resolution As DeviceResolution) As UInteger
    Declare Function ps5000aCloseUnit Lib "ps5000a.dll" (ByVal handle As Short) As UInteger

    Declare Function ps5000aGetUnitInfo Lib "ps5000a.dll" (ByVal handle As Short, ByVal str As String, ByVal stringLength As Short, ByRef requiredSize As Short, ByVal info As UInteger) As UInteger

    Declare Function ps5000aSetChannel Lib "ps5000a.dll" (ByVal handle As Short, ByVal channel As Channel, ByVal enabled As Short, ByVal dc As CouplingMode, ByVal range As VoltageRange, ByVal analogueOffset As Single) As UInteger

    Declare Function ps5000aGetTimebase2 Lib "ps5000a.dll" (ByVal handle As Short, ByVal timebase As UInteger, ByVal noSamples As Integer, ByRef timeIntervalNs As Single, ByRef maxSamples As Integer, ByVal segment As UInteger) As UInteger
    Declare Function ps5000aSetDataBuffer Lib "ps5000a.dll" (ByVal handle As Short, ByVal channel As Channel, ByRef buffer As Short, ByVal length As Integer, ByVal segmentIndex As UInteger, ByVal downSampleRatioMode As RatioMode) As UInteger
    Declare Function ps5000aSetDataBuffers Lib "ps5000a.dll" (ByVal handle As Short, ByVal channel As Channel, ByRef bufferMax As Short, ByRef bufferMin As Short, ByVal length As Integer, ByVal segmentIndex As UInteger, ByVal downSampleRatioMode As RatioMode) As UInteger
    Declare Function ps5000aGetValues Lib "ps5000a.dll" (ByVal handle As Short, ByVal startIndex As UInteger, ByRef numSamples As UInteger, ByVal downSampleRatio As UInteger, ByVal downSampleRatioMode As RatioMode, ByVal segmentIndex As UInteger, ByRef overflow As Short) As UInteger

    Declare Function ps5000aRunBlock Lib "ps5000a.dll" (ByVal handle As Short, noOfPreTriggerSamples As Integer, noOfPostTriggerSamples As Integer, timebase As UInteger, ByRef timeIndisposedMs As Integer, ByVal segmentIndex As UInteger, ByVal lpps5000aBlockReady As ps5000aBlockReady, pParam As IntPtr) As UInteger

    Declare Function ps5000aStop Lib "ps5000a.dll" (ByVal handle As Short) As UInteger

    Declare Function ps5000aChangePowerSource Lib "ps5000a.dll" (ByVal handle As Short, ByVal powerState As UInteger) As UInteger
    Declare Function ps5000aCurrentPowerSource Lib "ps5000a.dll" (ByVal handle As Short) As UInteger
    Declare Function ps5000aGetDeviceResolution Lib "ps5000a.dll" (ByVal handle As Short, ByRef resolution As DeviceResolution) As UInteger
    Declare Function ps5000aSetDeviceResolution Lib "ps5000a.dll" (ByVal handle As Short, ByVal resolution As DeviceResolution) As UInteger

    Declare Function ps5000aMaximumValue Lib "ps5000a.dll" (ByVal handle As Short, ByRef value As Short) As UInteger
    Declare Function ps5000aMinimumValue Lib "ps5000a.dll" (ByVal handle As Short, ByRef value As Short) As UInteger

    Declare Function ps5000aSetSimpleTrigger Lib "ps5000a.dll" (ByVal handle As Short, ByVal enable As Short, ByVal channel As Channel, ByVal threshold As Short, ByVal direction As ThresholdDirection, ByVal delay As UInteger, ByVal autoTriggerMs As Short) As UInteger

    Declare Sub Sleep Lib "kernel32.dll" (ByVal time As Short)

    ' Delegate declaration
    ' --------------------

    ' Block mode
    Public Delegate Sub ps5000aBlockReady(handle As Short, status As UInteger, pVoid As IntPtr)
    Public ps5000aBlockCallback As ps5000aBlockReady

    ' Define variables

    Dim handle As Short
    Dim status As UInteger
    Dim serial As String
    Dim maxADCValue As Short
    Dim channelCount As Short
    Dim totalSamples As UInteger

    Public deviceReady As Boolean

    ' ******************************************************************************************************************************************************************
    ' GetDeviceInfo - Reads and displays the scopes device information. Fills out the UnitModel Structure depending upon device type.
    '
    ' Parameters: handle - the device handle
    ' *******************************************************************************************************************************************************************
    Sub getDeviceInfo(ByVal handle As Short)

        Dim infoText(20) As String
        Dim infoStr As String
        Dim requiredSize As Integer
        Dim i As Integer
        Dim chrs(5) As Char

        infoText(0) = "Driver Ver:        "
        infoText(1) = "USB Ver:           "
        infoText(2) = "Hardware Ver:      "
        infoText(3) = "Variant:           "
        infoText(4) = "Batch / Serial:    "
        infoText(5) = "Cal Date:          "
        infoText(6) = "Kernel Driver Ver: "
        infoText(7) = "Digital H/W Ver:   "
        infoText(8) = "Analogue H/W Ver:  "

        For i = 0 To 8

            infoStr = "            "
            ps5000aGetUnitInfo(handle, infoStr, 255, requiredSize, i)
            infoText(i) += infoStr
            Console.WriteLine(infoText(i) & vbTab)

            If i = 3 Then

                channelCount = Convert.ToInt16(infoStr.ToCharArray(1, 1))

            End If

        Next i

        Console.WriteLine(vbNewLine)

    End Sub

    ' ******************************************************************************************************************************************************************
    ' adcToMv - Converts from raw ADC values to mV values. The mV value returned depends upon the ADC count, and the voltage range set for the channel. 
    '
    ' Parameters - raw    - An integer holding the ADC count to be converted to mV
    '            - range  - A value indicating where in the 'inputRanges' array the range value can be found
    '
    ' Returns    - value converted into mV
    ' *******************************************************************************************************************************************************************

    Function adcToMv(ByVal raw As Integer, ByVal range As VoltageRange)

        Dim mVVal As Integer        ' Use this variable to force data to be returned as an integer

        mVVal = (raw * inputRanges(range)) / maxADCValue

        Return mVVal

    End Function


    ' ******************************************************************************************************************************************************************
    ' mvToAdc - Converts from mV into ADC value. The ADC count returned depends upon the mV value, and the voltage range set for the channel. 
    '
    ' Parameters - mv     - An short holding the mv value to be converted to the ADC count
    '            - range  - A value indicating where in the 'inputRanges' array the range value can be found
    '
    ' Returns    - value converted into an ADC count
    ' *******************************************************************************************************************************************************************
    Function mvToAdc(ByVal mv As Short, ByVal range As VoltageRange) As Short

        Dim adcCount As Short

        adcCount = CShort((mv / inputRanges(range)) * maxADCValue)

        Return adcCount

    End Function

    ' ******************************************************************************************************************************************************************
    ' Main -        Entry point to the application
    '
    '
    ' *******************************************************************************************************************************************************************
    Sub Main()

        Dim status As UInteger
        Dim handle As Short


        Console.WriteLine("PS5000a VB.Net example program" & vbNewLine & "==============================" & vbNewLine)
        status = ps5000aOpenUnit(handle, vbNullString, DeviceResolution.PS5000A_DR_12BIT)

        If status = PICO_OK Then

            Console.WriteLine("Device DC Powered" & vbNewLine)

        ElseIf status = 282 Then

            ' Change power status if USB powered
            status = ps5000aChangePowerSource(handle, status)

            If status = PICO_OK Then

                Console.WriteLine("Device USB Powered" & vbNewLine)

            Else

                Console.WriteLine("Error changing power source - exiting application.")
                Exit Sub

            End If

        ElseIf status = 3 Then

            Console.WriteLine("Device not found." & vbNewLine)
            Exit Sub

        Else

            Console.WriteLine("Device not opened." & vbNewLine)
            Exit Sub

        End If

        ' Find maximum ADC count (resolution dependent)

        status = ps5000aMaximumValue(handle, maxADCValue)

        getDeviceInfo(handle) 'Read and display the device information


        ' Setup channel A
        status = ps5000aSetChannel(handle, Channel.PS5000A_CHANNEL_A, CShort(1), CouplingMode.PS5000A_DC, VoltageRange.PS5000A_5V, 0.0)

        ' Turn off other channels

        status = ps5000aSetChannel(handle, Channel.PS5000A_CHANNEL_B, CShort(0), CouplingMode.PS5000A_DC, VoltageRange.PS5000A_5V, 0.0)

        ' Only disable channels C and D on a 4-channel device if the power supply is connected
        Dim powerStatus As UInteger
        powerStatus = ps5000aCurrentPowerSource(handle)

        If channelCount = 4 And powerStatus = 281 Then

            status = ps5000aSetChannel(handle, Channel.PS5000A_CHANNEL_C, CShort(0), CouplingMode.PS5000A_DC, VoltageRange.PS5000A_5V, 0.0)
            status = ps5000aSetChannel(handle, Channel.PS5000A_CHANNEL_D, CShort(0), CouplingMode.PS5000A_DC, VoltageRange.PS5000A_5V, 0.0)

        End If

        ' Confirm timebase is valid

        Dim timebase As UInteger
        Dim numPreTriggerSamples As Integer
        Dim numPostTriggerSamples As Integer
        Dim timeIntervalNs As Single
        Dim maxSamples As Integer
        Dim segmentIndex As UInteger
        Dim getTimebase2Status As UInteger

        timebase = 65
        numPreTriggerSamples = 0
        numPostTriggerSamples = 10000
        totalSamples = numPreTriggerSamples + numPostTriggerSamples

        timeIntervalNs = CSng(0.0)
        maxSamples = 0
        segmentIndex = 0
        getTimebase2Status = 14 ' Initialise as invalid timebase


        Do Until getTimebase2Status = 0

            getTimebase2Status = ps5000aGetTimebase2(handle, timebase, totalSamples, timeIntervalNs, maxSamples, segmentIndex)

            If getTimebase2Status <> 0 Then

                timebase = timebase + 1

            End If

        Loop

        Console.WriteLine("Timebase: {0} Sample interval: {1}ns, Max Samples: {2}", timebase, timeIntervalNs, maxSamples, vbNewLine)

        ' Setup trigger

        Dim threshold As Short
        Dim delay As UInteger
        Dim autoTriggerMs As Short

        threshold = mvToAdc(500, VoltageRange.PS5000A_5V)

        Console.WriteLine("Trigger threshold: {0}mV ({1} ADC Counts)", 500, threshold, vbNewLine)

        delay = 0
        autoTriggerMs = 0

        status = ps5000aSetSimpleTrigger(handle, CShort(1), Channel.PS5000A_CHANNEL_A, threshold, ThresholdDirection.PS5000A_RISING, delay, autoTriggerMs)

        ' Capture block

        ' Create instance of delegate
        ps5000aBlockCallback = New ps5000aBlockReady(AddressOf BlockCallback)

        Dim timeIndisposedMs As Integer = 0

        status = ps5000aRunBlock(handle, numPreTriggerSamples, numPostTriggerSamples, timebase, timeIndisposedMs, segmentIndex, ps5000aBlockCallback, IntPtr.Zero)

        While deviceReady = False

            Sleep(10)

        End While

        ' Stop the device

        status = ps5000aStop(handle)

        ' Setup data buffers

        Dim valuesChA() As Short

        ReDim valuesChA(totalSamples - 1)

        status = ps5000aSetDataBuffer(handle, Channel.PS5000A_CHANNEL_A, valuesChA(0), CInt(totalSamples), segmentIndex, RatioMode.PS5000A_RATIO_MODE_NONE)

        ' Retrieve values

        Dim startIndex As UInteger
        Dim downSampleRatio As UInteger
        Dim overflow As Short

        startIndex = 0
        downSampleRatio = 1
        overflow = 0

        status = ps5000aGetValues(handle, startIndex, totalSamples, downSampleRatio, RatioMode.PS5000A_RATIO_MODE_NONE, segmentIndex, overflow)

        Console.WriteLine("Retrieved {0} samples.", totalSamples, vbNewLine)

        ' Convert data to milliVolts and output to file

        Dim valuesChAMv() As Short
        ReDim valuesChAMv(totalSamples - 1)

        Using outfile As New StreamWriter("triggered_block.txt")

            For index = 0 To totalSamples - 1

                valuesChAMv(index) = adcToMv(valuesChA(index), VoltageRange.PS5000A_5V)
                outfile.Write(valuesChAMv(index).ToString)
                outfile.WriteLine()

            Next

        End Using

        ' Close the unit at the end
        ps5000aCloseUnit(handle)

        Console.WriteLine(vbNewLine)

        Console.WriteLine("Exiting application..." & vbNewLine)

        Sleep(5000)

    End Sub

    ' Block callback function

    Public Sub BlockCallback(handle As Short, status As UInteger, pVoid As IntPtr)
        ' flag to say done reading data
        If status <> &H3A Then
            deviceReady = True
        End If
    End Sub

End Module
