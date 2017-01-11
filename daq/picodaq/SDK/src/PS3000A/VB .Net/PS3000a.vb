Module PS3000a

    Public Structure TRIGGER_CONDITION
        Public channelA As UInteger
        Public channelB As UInteger
        Public channelC As UInteger
        Public channelD As UInteger
        Public external As UInteger
        Public aux As UInteger
        Public pulseWidthQualifier As UInteger
    End Structure

    Public Structure DIGITAL_DIRECTION
        Public channel As UInteger
        Public direction As UInteger
    End Structure

    ' Function declarations
    ' ---------------------

    Declare Function ps3000aOpenUnit Lib "ps3000a.dll" (ByRef handle As Short, ByVal serial As String) As UInteger
    Declare Sub ps3000aCloseUnit Lib "ps3000a.dll" (ByVal handle As Short)

    Declare Function ps3000aGetUnitInfo Lib "ps3000a.dll" (ByVal handle As Short, ByVal str As String, ByVal stringLength As Short, ByRef requiredSize As Short, ByVal info As UInteger) As UInteger
    Declare Function ps3000aChangePowerSource Lib "ps3000a.dll" (ByVal handle As Short, ByVal powerState As UInteger) As UInteger

    Declare Function ps3000aSetChannel Lib "ps3000a.dll" (ByVal handle As Short, ByVal channel As UInteger, ByVal enabled As Short, ByVal dc As UInteger, ByVal range As UInteger, ByVal analogueOffset As Single) As UInteger
    Declare Function ps3000aGetTimebase2 Lib "ps3000a.dll" (ByVal handle As Short, ByVal timebase As UInteger, ByVal noSamples As Integer, ByRef timeIntervalNs As Single, ByVal oversample As Short, ByRef maxSamples As Integer, ByVal segment As UInteger) As UInteger
    Declare Function ps3000aMaximumValue Lib "ps3000a.dll" (ByVal handle As Short, ByRef value As Short) As UInteger

    Declare Function ps3000aRunBlock Lib "ps3000a.dll" (ByVal handle As Short, noOfPreTriggerSamples As Integer, noOfPostTriggerSamples As Integer, timebase As UInteger, oversample As Short, ByRef timeIndisposedMs As Integer, ByVal segmentIndex As UInteger, ByVal lpps3000aBlockReady As ps3000aBlockReady, pParam As IntPtr) As UInteger
    Declare Function ps3000aIsReady Lib "ps3000a.dll" (ByVal handle As Short, ByRef ready As Short)

    Declare Function ps3000aSetDataBuffer Lib "ps3000a.dll" (ByVal handle As Short, ByVal channel As UInteger, ByRef buffer As Short, ByVal length As Integer, ByVal segmentIndex As UInteger, ByVal downSampleRatioMode As UInteger) As UInteger
    Declare Function ps3000aGetValues Lib "ps3000a.dll" (ByVal handle As Short, ByVal startIndex As UInteger, numSamples As UInteger, ByVal downSampleRatio As UInteger, ByVal downSampleRatioMode As UInteger, ByVal segmentIndex As UInteger, ByRef overflow As Short) As UInteger
    Declare Function ps3000aStop Lib "ps3000a.dll" (ByVal handle As Short) As UInteger

    ' Delegate declaration
    ' --------------------

    ' Block mode
    Public Delegate Sub ps3000aBlockReady(handle As Short, status As UInteger, pVoid As IntPtr)

    Public deviceReady As Boolean
    Public ps3000aBlockCallback As ps3000aBlockReady


    Sub Main()

        Dim status As UInteger
        Dim handle As Short

        Console.WriteLine("PS3000a VB.Net example program" & vbNewLine & "==============================" & vbNewLine)

        handle = 0

        status = ps3000aOpenUnit(handle, vbNullString)

        If handle = 0 Then

            Console.WriteLine("Unit not opened")

        Else

            'Change power source if using USB power for PicoScope 3400A/B or 3200/3400 DMSO products
            ' or if it is a USB 3.0 device on a USB 2.0 port
            If (status = 282 Or status = 286) Then

                status = ps3000aChangePowerSource(handle, status)

            End If

            ' Print Device information
            getDeviceInfo(handle)


            ps3000aCloseUnit(handle)

        End If

    End Sub

    Sub getDeviceInfo(ByVal handle As Short)

        Dim infoText(8) As String
        Dim s As String
        Dim requiredSize As Short
        Dim i As UInteger

        infoText(0) = "Driver Ver:       "
        infoText(1) = "USB Ver:          "
        infoText(2) = "Hardware Ver:     "
        infoText(3) = "Variant:          "
        infoText(4) = "Batch / Serial:   "
        infoText(5) = "Cal Date:         "
        infoText(6) = "Kernel Driver Ver:"
        infoText(7) = "Digital H/W Ver:  "
        infoText(8) = "Analogue H/W Ver: "

        For i = 0 To 8

            s = "            "
            ps3000aGetUnitInfo(handle, s, 45, requiredSize, i)
            infoText(i) += s
            Console.WriteLine(infoText(i) & vbTab)

        Next i

    End Sub

    ' Block callback function

    Public Sub BlockCallback(handle As Short, status As UInteger, pVoid As IntPtr)
        ' flag to say done reading data
        If status <> &H3A Then
            deviceReady = True
        End If
    End Sub

    ' ps3000aBlockCallback = New ps3000aBlockReady(AddressOf BlockCallback)

    'Dim timeIndisposedMs As Integer = 0
    'Dim pParam As UIntPtr = 0

    'Dim preTriggerSamples As Integer
    '    preTriggerSamples = 0

    'Dim postTriggerSamples As Integer
    '    postTriggerSamples = CInt(numberOfSamples)
    ' status = ps3000aRunBlock(handle, preTriggerSamples, postTriggerSamples, timebase, CShort(1), timeIndisposedMs, CUInt(0), ps3000aBlockCallback, IntPtr.Zero)

End Module
