'========================================================================================
'	Filename:			USBTC08.vb
'
'
'	Purpose
'	This file demonstrates how to use vb.net on the USB TC-08 Thermocouple Data Logger.
'   It shows the use of API call functions to collect temperature data.
'
'	04-06-2014	Revision 1		HSM         Initial version. 
'
'========================================================================================

' Enumerations
Enum USBTC08Channels
    USBTC08_CHANNEL_CJC = 0
    USBTC08_CHANNEL_1 = 1
    USBTC08_CHANNEL_2 = 2
    USBTC08_CHANNEL_3 = 3
    USBTC08_CHANNEL_4 = 4
    USBTC08_CHANNEL_5 = 5
    USBTC08_CHANNEL_6 = 6
    USBTC08_CHANNEL_7 = 7
    USBTC08_CHANNEL_8 = 8
End Enum

Enum USBTC08MainsFrequency
    USBTC08_REJECT_50HZ = 0
    USBTC08_REJECT_60HZ = 1
End Enum

Enum USBTC08TempUnits
    USBTC08_UNITS_CENTIGRADE = 0
    USBTC08_UNITS_FAHRENHEIT = 1
    USBTC08_UNITS_KELVIN = 2
    USBTC08_UNITS_RANKINE = 3
End Enum

Module USBTC08

    ' Function Declarations

    Declare Function usb_tc08_set_channel Lib "usbtc08.dll" (ByVal handle As Short, ByVal channel As USBTC08Channels, ByVal tc_type As Char) As Short

    Declare Function usb_tc08_run Lib "usbtc08.dll" (ByVal handle As Short, ByVal interval_ms As Integer) As Integer

    Declare Function usb_tc08_get_temp Lib "usbtc08.dll" (ByVal handle As Short, ByRef temp_buffer As Single, ByRef times_ms_buffer As Integer, ByVal buffer_length As Integer, ByRef overflow As Short, ByVal channel As Short, units As Short, ByVal fill_missing As Short) As Integer

    Declare Function usb_tc08_get_temp_deskew Lib "usbtc08.dll" (ByVal handle As Short, ByRef temp_buffer As Single, ByRef times_ms_buffer As Integer, ByVal buffer_length As Integer, ByRef overflow As Short, ByVal channel As Short, ByVal units As Short, ByVal fill_missing As Short) As Integer

    Declare Function usb_tc08_get_single Lib "usbtc08.dll" (ByVal handle As Short, ByRef temp As Single, ByRef overflow_flags As Short, ByVal units As USBTC08TempUnits) As Integer

    Declare Function usb_tc08_open_unit Lib "usbtc08.dll" () As Short

    Declare Function usb_tc08_close_unit Lib "usbtc08.dll" (ByVal handle As Short) As Short

    Declare Function usb_tc08_stop Lib "usbtc08.dll" (ByVal handle As Short) As Short

    Declare Function usb_tc08_set_mains Lib "usbtc08.dll" (ByVal handle As Short, ByVal sixty_hertz As USBTC08MainsFrequency) As Short

    Declare Function usb_tc08_get_minimum_interval_ms Lib "usbtc08.dll" (ByVal handle As Short) As Integer

    Declare Function usb_tc08_get_formatted_info Lib "usbtc08.dll" (ByVal handle As Short, ByVal unit_info As String, ByVal string_length As Short) As Short

    Declare Function usb_tc08_get_unit_info2 Lib "usbtc08.dll" (ByVal handle As Short, ByVal unit_info As String, ByVal string_length As Short, ByVal line As Short) As Short

    Declare Function usb_tc08_get_last_error Lib "usbtc08.dll" (ByVal handle As Short) As Short

    Sub Main()

        Dim status As Short
        Dim tc08_handle As Short
        Dim info As String
        Dim minimumIntervalMS As Integer
        Dim tempBuffer(8) As Single
        Dim count As Integer
        Dim overflowFlag As Short
        Dim keypress As Boolean

        tc08_handle = 0
        keypress = False

        If (tc08_handle < 1) Then

            Console.WriteLine("USB TC-08 VB.NET Console application" & vbNewLine)
            Console.WriteLine("Opening TC-08..." & vbNewLine)

            tc08_handle = usb_tc08_open_unit()

            If (tc08_handle > 0) Then

                Console.WriteLine("TC-08 opened." & vbNewLine)

                info = "              "

                'Output info
                Call usb_tc08_get_unit_info2(tc08_handle, info, 80, 0)
                Console.WriteLine("Driver Version " & info)

                Call usb_tc08_get_unit_info2(tc08_handle, info, 80, 1)
                Console.WriteLine("Kernel Driver Version " & info)

                Call usb_tc08_get_unit_info2(tc08_handle, info, 80, 4)
                Console.WriteLine("Serial Number " & info)

                Call usb_tc08_get_unit_info2(tc08_handle, info, 80, 5)
                Console.WriteLine("Cal Date " & info & vbNewLine)

                ' Set noise rejection to 50 or 60Hz
                Call usb_tc08_set_mains(tc08_handle, USBTC08MainsFrequency.USBTC08_REJECT_50HZ)

                ' Set Channels - Cold Junction, Ch 1 and Ch 2
                status = usb_tc08_set_channel(tc08_handle, USBTC08Channels.USBTC08_CHANNEL_CJC, "K")
                status = usb_tc08_set_channel(tc08_handle, USBTC08Channels.USBTC08_CHANNEL_1, "K")
                status = usb_tc08_set_channel(tc08_handle, USBTC08Channels.USBTC08_CHANNEL_2, "K")

                ' Minimum Sampling Interval
                minimumIntervalMS = usb_tc08_get_minimum_interval_ms(tc08_handle)

                'Obtain 10 sets of temperature readings with 1 second between requests

                Console.WriteLine("Collecting 10 sets of readings..." & vbNewLine)

                Console.WriteLine("CJC" & vbTab & vbTab & "Ch1" & vbTab & vbTab & "Ch2")

                For count = 0 To 10

                    status = usb_tc08_get_single(tc08_handle, tempBuffer(0), overflowFlag, USBTC08TempUnits.USBTC08_UNITS_CENTIGRADE)

                    Console.Write(tempBuffer(0).ToString() & vbTab)
                    Console.Write(tempBuffer(1).ToString() & vbTab)
                    Console.Write(tempBuffer(2).ToString() & vbTab)
                    Console.WriteLine()

                    System.Threading.Thread.Sleep(1000)

                Next

                Console.WriteLine()
                Console.WriteLine("Data collection complete, please press any key to exit.")

                While keypress = False

                    System.Threading.Thread.Sleep(100)

                    If Console.KeyAvailable Then                ' Check if the user has hit a key to indicate they want to stop
                        keypress = True
                    End If

                End While

                Call usb_tc08_close_unit(tc08_handle)

                Console.WriteLine("Closing application...")

                System.Threading.Thread.Sleep(1000)

            Else

                Console.WriteLine("Unable to open USB TC-08")

            End If

        End If

    End Sub

End Module
