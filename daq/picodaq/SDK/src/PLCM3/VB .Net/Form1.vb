

Public Class Form1

    Declare Function PLCM3OpenUnit Lib "PLCM3.dll" (ByRef handle As Integer, ByVal serial As Integer) As Integer
    Declare Function PLCM3OpenUnitViaIp Lib "PLCM3.dll" (ByRef handle As Integer, ByVal serial As String, ByVal ip As String) As Integer
    Declare Function PLCM3CloseUnit Lib "PLCM3.dll" (ByVal handle As Integer) As Integer
    Declare Function PLCM3GetUnitInfo Lib "PLCM3.dll" (ByVal handle As Integer, ByVal str As String, ByVal stringLength As Integer, ByRef requiredSize As Integer, ByVal info As Integer) As Integer
    Declare Function PLCM3SetChannel Lib "PLCM3.dll" (ByVal handle As Integer, ByVal channel As Integer, ByVal measurementType As Integer) As Integer
    Declare Function PLCM3GetValue Lib "PLCM3.dll" (ByVal handle As Integer, ByVal channel As Integer, ByRef value As Integer) As Integer

    Declare Sub Sleep Lib "kernel32.dll" (ByVal time As Integer)



    Dim handle As Integer
    Dim status As Integer
    Dim opened As Boolean


    Dim dataType(3) As Integer

    Dim combobox As List(Of ComboBox)
    Dim progressbar As List(Of ProgressBar)
    Dim textbox As List(Of TextBox)
    Dim unitslabel As List(Of Label)

    ' *******************************************************************************
    ' Form1_Load
    ' Initialisation code
    ' *******************************************************************************
    Private Sub Form1_Load(sender As System.Object, e As System.EventArgs) Handles MyBase.Load

        combobox = New List(Of ComboBox)
        progressbar = New List(Of ProgressBar)
        textbox = New List(Of TextBox)
        unitslabel = New List(Of Label)


        progressbar.Add(ProgressBar1)
        progressbar.Add(ProgressBar2)
        progressbar.Add(ProgressBar3)

        textbox.Add(TextBox1)
        textbox.Add(TextBox2)
        textbox.Add(TextBox3)

        unitslabel.Add(UnitsLabel1)
        unitslabel.Add(UnitsLabel2)
        unitslabel.Add(UnitsLabel3)

        combobox.Add(ComboBox1)
        combobox.Add(ComboBox2)
        combobox.Add(ComboBox3)

        opened = False
        Button1.Text = "Open Unit"
        UsbButton.Checked = True



  
        For i = 0 To progressbar.Count - 1 Step 1
            combobox.Item(i).Text = "Voltage"
            dataType(i) = 4 ' 4 = Voltage
            unitslabel.Item(i).Text = "mV"
        Next

    End Sub
    ' *******************************************************************************
    ' GetInfo
    ' Get the unit information and display each value
    ' *******************************************************************************
    Private Sub GetInfo()
        Dim infoText(8) As String
        Dim S As String
        Dim requiredSize As Integer


        infoText(0) = "Driver Ver:       "
        infoText(1) = "USB Ver:          "
        infoText(2) = "Hardware Ver:     "
        infoText(3) = "Variant:          "
        infoText(4) = "Batch / Serial:   "
        infoText(5) = "Cal Date:         "
        infoText(6) = "Kernel Driver Ver:"

        For i = 0 To 6
            S = ".................."
            PLCM3GetUnitInfo(handle, S, 255, requiredSize, i)
            UnitInfoTextBox.Text &= infoText(i) & vbTab
            UnitInfoTextBox.Text &= S
            UnitInfoTextBox.Text &= Environment.NewLine
        Next i
    End Sub
    ' *******************************************************************************
    ' CheckIPAddr
    ' Check that the IP Adress entered is a valid IP Address format
    ' *******************************************************************************
    Private Function CheckIPAddr(ByRef IPAddr) As Boolean

        Dim IPAddrPart(0 To 3) As String
        Dim RetVal As Boolean

        RetVal = True

        IPAddrPart = IPAddr.Split(".")

        If (IPAddrPart.Length <> 4) Then
            MsgBox("Use IP Address format xxx.xxx.xxx.xxx ", vbOKOnly, "Error Message")
            RetVal = False
        Else
            For Part = 0 To 3 Step 1
                Try
                    If (IPAddrPart(Part) < 0 Or IPAddrPart(Part) > 255) Then
                        MsgBox("IP Values out of range ", vbOKOnly, "Error Message")
                        RetVal = False
                        Exit For
                    End If
                Catch ex As Exception
                    MsgBox("IP Address contains invalid characters ", vbOKOnly, "Error Message")
                    RetVal = False
                    Exit For
                End Try
            Next Part
        End If

        Return RetVal
    End Function
    ' *******************************************************************************
    ' OpenUnit
    ' Open Unit Via USB or Ethernet, depending upon which button is selected
    ' *******************************************************************************
    Private Function OpenUnit()

        Dim serial As String
        Dim ipaddr As String

        ipaddr = vbNullString
        serial = vbNullString

        status = -1

        If EthernetButton.Checked = True Then
            ipaddr = IPAddrTextBox.Text
            If CheckIPAddr(ipaddr) Then
                status = PLCM3OpenUnitViaIp(handle, serial, ipaddr)
                If status <> 0 Then
                    MsgBox("Unit not opened status = " & status, vbOKOnly, "Error Message")
                Else
                    opened = True
                    Button1.Text = "Close Unit"     ' Chenge text on button
                End If
            End If

        Else
            status = PLCM3OpenUnit(handle, 0)
            If status <> 0 Then
                MsgBox("Unit not opened status = " & status, vbOKOnly, "Error Message")
            Else
                opened = True
                Button1.Text = "Close Unit"     ' Chenge text on button
            End If
        End If

        Return status

    End Function
    ' *******************************************************************************
    ' CloseUnit
    ' Close the unit, and clear message boxes
    ' *******************************************************************************
    Private Sub CloseUnit()
        status = PLCM3CloseUnit(handle)
        opened = False
        Button1.Text = "Open Unit"  'Change text ready for next opening

        UnitInfoTextBox.Clear()     ' clear the unit information box
        TextBox1.Clear()            ' clear Units boxes (mV mA A)
        TextBox2.Clear()
        TextBox3.Clear()
        ProgressBar1.Value = 0      ' clear progress bars
        ProgressBar2.Value = 0
        ProgressBar3.Value = 0
    End Sub

    ' *******************************************************************************
    ' ShowValues
    ' Display values read from each channel. Show on progress bar, 
    ' and in text box. Show units used
    ' *******************************************************************************

    Private Sub ShowValues()
        Dim Val As Integer
        Dim ProgBarVal As Integer
        Dim chan As Integer


        For i = 0 To progressbar.Count - 1 Step 1
            chan = i + 1 ' channels are 1, 2 & 3

            If dataType(i) > 0 Then

                PLCM3GetValue(handle, chan, Val)  ' chnnel 1,2 & 3

                If dataType(i) < 4 Then             ' 1mV/A 10mA/V or 100mA/V

                    If dataType(i) = 1 Then         ' 1mV/A
                        ProgBarVal = Val
                    ElseIf dataType(i) = 2 Then     ' 10mV/A
                        ProgBarVal = Val * 10
                    ElseIf dataType(i) = 3 Then     ' 100mV/A
                        ProgBarVal = Val * 100
                    End If


                    If Val < 10000 Then
                        unitslabel.Item(i).Text = "mA"
                    Else
                        Val = Val / 1000
                        unitslabel.Item(i).Text = "A"
                    End If


                ElseIf dataType(i) = 4 Then            ' Voltage
                    ProgBarVal = Val
                    Val = Val / 1000.0
                    unitslabel.Item(i).Text = "mV"
                Else                                ' should never get here....
                    ProgBarVal = Val
                    Val = 0
                    unitslabel.Item(i).Text = "units"

                End If

                If ProgBarVal > ProgressBar1.Maximum Then     ' If input voltage is overrange, show as max range
                    ProgBarVal = ProgressBar1.Maximum
                End If

                progressbar.Item(i).Value = ProgBarVal  ' show progress bar
                textbox.Item(i).Text = Convert.ToString(Val) ' show value


            Else ' if dataType = 0
                progressbar.Item(i).Value = 0 ' Channel is off
                textbox.Item(i).Text = Convert.ToString(0)
            End If
        Next
    End Sub

    ' *******************************************************************************
    ' Button1_Click
    ' If unit is not opened, calls the OpenUnit function.
    ' If unit is opened, calls the CloseUint function
    ' *******************************************************************************

    Private Sub Button1_Click(sender As System.Object, e As System.EventArgs) Handles Button1.Click
        Dim status As Integer


        If opened = False Then
            status = OpenUnit()
            If status = 0 Then ' unit opened ok
                GetInfo()
                For i = 0 To 3 Step 1
                    status = PLCM3SetChannel(handle, i + 1, dataType(i))
                Next
            End If
        Else
            CloseUnit()
        End If
    End Sub

    ' *******************************************************************************
    ' Ch1ComboBox_SelectedIndexChanged
    ' Selects the input range / type for channel1
    ' *******************************************************************************
    Private Sub Ch1ComboBox_SelectedIndexChanged(sender As System.Object, e As System.EventArgs) Handles ComboBox1.SelectedIndexChanged

        If ComboBox1.Text = "Voltage" Then
            dataType(0) = 4
        ElseIf ComboBox1.Text = "100mV/A" Then
            dataType(0) = 3
        ElseIf ComboBox1.Text = "10mV/A" Then
            dataType(0) = 2
        ElseIf ComboBox1.Text = "1mV/A" Then
            dataType(0) = 1
        Else
            dataType(0) = 0
        End If

        status = PLCM3SetChannel(handle, 1, dataType(0)) ' Re-initialise channel settings
    End Sub


    ' *******************************************************************************
    ' Ch2ComboBox_SelectedIndexChanged
    ' Selects the input range / type for channel2
    ' *******************************************************************************
    Private Sub ComboBox2_SelectedIndexChanged(sender As System.Object, e As System.EventArgs) Handles ComboBox2.SelectedIndexChanged
        If ComboBox2.Text = "Voltage" Then
            dataType(1) = 4
        ElseIf ComboBox2.Text = "100mV/A" Then
            dataType(1) = 3
        ElseIf ComboBox2.Text = "10mV/A" Then
            dataType(1) = 2
        ElseIf ComboBox2.Text = "1mV/A" Then
            dataType(1) = 1
        Else
            dataType(1) = 0
        End If

        status = PLCM3SetChannel(handle, 2, dataType(1)) ' Re-initialise channel settings
    End Sub

    ' *******************************************************************************
    ' Ch3ComboBox_SelectedIndexChanged
    ' Selects the input range / type for channel3
    ' *******************************************************************************
    Private Sub ComboBox3_SelectedIndexChanged(sender As System.Object, e As System.EventArgs) Handles ComboBox3.SelectedIndexChanged
        If ComboBox3.Text = "Voltage" Then
            dataType(2) = 4
        ElseIf ComboBox3.Text = "100mV/A" Then
            dataType(2) = 3
        ElseIf ComboBox3.Text = "10mV/A" Then
            dataType(2) = 2
        ElseIf ComboBox3.Text = "1mV/A" Then
            dataType(2) = 1
        Else
            dataType(2) = 0
        End If

        status = PLCM3SetChannel(handle, 3, dataType(2)) ' Re-initialise channel settings
    End Sub


    Private Sub Timer1_Tick(sender As System.Object, e As System.EventArgs) Handles Timer1.Tick
        If opened = True Then
            ShowValues()
        End If
    End Sub

End Class