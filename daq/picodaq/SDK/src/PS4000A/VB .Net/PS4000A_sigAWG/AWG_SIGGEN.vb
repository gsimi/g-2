'
'
'   Filename:    AWG_SIGGEN.vb
'
'   Copyright:   Pico Technology Limited 2014
'
'   Author:      KPV
'
'   Description:
'       This is a program that lets you control the AWG/Signal Generator
'
'   Examples:
'       Outputs singal from signal generator
'       loads in file and creates signal using that
'
'
'   History:
'       11Feb14 	KPV	    Created
'
'   Revision Info: "file %n date %f revision %v"
'					

Imports System.IO

Public Class AWG_SIGGEN

    Enum SigGenTrigSource
        ps4000A_SIGGEN_NONE
        ps4000A_SIGGEN_SCOPE_TRIG
        ps4000A_SIGGEN_AUX_IN
        ps4000A_SIGGEN_EXT_IN
        ps4000A_SIGGEN_SOFT_TRIG
    End Enum

    Enum SigGenTrigType
        ps4000A_SIGGEN_RISING
        ps4000A_SIGGEN_FALLING
        ps4000A_SIGGEN_GATE_HIGH
        ps4000A_SIGGEN_GATE_LOW
    End Enum

    Enum ExtraOperations
        ps4000A_ES_OFF
        ps4000A_WHITENOISE
        ps4000A_PRBS 'Pseudo-Random Bit Stream 
    End Enum

    Enum SweepType
        ps4000A_UP
        ps4000A_DOWN
        ps4000A_UPDOWN
        ps4000A_DOWNUP
    End Enum

    Enum WaveType
        ps4000A_SINE
        ps4000A_SQUARE
        ps4000A_TRIANGLE
        ps4000A_RAMP_UP
        ps4000A_RAMP_DOWN
        ps4000A_SINC
        ps4000A_GAUSSIAN
        ps4000A_HALF_SINE
        ps4000A_DC_VOLTAGE
        ps4000A_WHITE_NOISE
    End Enum

    Enum IndexMode
        ps4000A_SINGLE
        ps4000A_DUAL
        ps4000A_QUAD
        ps4000A_MAX_INDEX_MODES
    End Enum

    Const AWG_DAC_FREQUENCY = 80000000.0
    Const AWG_PHASE_ACCUMULATOR = 4294967296.0
    Const MIN_DWELL_COUNT = 3


    Declare Function ps4000aGetUnitInfo Lib "ps4000A.dll" (ByVal handle As Short, ByVal str As String, ByVal lth As Short, ByRef requiredSize As Short, ByVal info As Short) As UInteger
    Declare Function ps4000aOpenUnit Lib "ps4000A.dll" (ByRef handle As Short, ByVal serial As String) As UInteger
    Declare Function ps4000aCloseUnit Lib "ps4000A.dll" (ByVal handle As Short) As UInteger
    Declare Function ps4000aChangePowerSource Lib "ps4000A.dll" (ByVal handle As Short, ByVal source As Short) As UInteger
    Declare Function ps4000aSetSigGenBuiltIn Lib "ps4000A.dll" (ByVal handle As Short, ByVal offsetvoltage As Integer, ByVal pktopk As UInteger, ByVal wavetype As WaveType, ByVal startfreq As Double, ByVal stopfreq As Double, ByVal increment As Double, ByVal dwelltime As Double, ByVal sweeptype As SweepType, ByVal operation As ExtraOperations, ByVal shot As UInteger, ByVal sweeps As UInteger, ByRef triggertype As SigGenTrigType, ByVal triggersource As SigGenTrigSource, ByVal extinthreshold As Short) As UInteger
    Declare Function ps4000aSetSigGenArbitrary Lib "ps4000A.dll" (ByVal handle As Short, ByVal offsetvoltage As Integer, ByVal pktopk As UInteger, ByVal startdeltaphase As UInteger, ByVal stopdeltaphase As UInteger, ByVal deltaphaseincrement As UInteger, ByVal dwellcount As UInteger, ByRef arbitrarywaveform As Short, ByVal arbitarywaveformsize As Integer, ByVal sweeptype As SweepType, ByVal operation As ExtraOperations, ByVal indexmode As IndexMode, ByVal shots As UInteger, ByVal sweeps As UInteger, ByRef triggertype As SigGenTrigType, ByVal triggersource As SigGenTrigSource, ByVal extinthreshold As Short) As UInteger

    Dim _handle As Short
    Dim status As Short
    Dim awgBufferSize As UInteger

    'starts up device and grabs relavent information
    Private Sub start_Click(sender As Object, e As EventArgs) Handles start.Click
        Dim str As String = "     "
        Dim requiredSize As Integer

        status = ps4000aOpenUnit(_handle, vbNullString)

        If _handle = 0 Then
            MessageBox.Show("Cannot open device" & vbNewLine & "error code" & status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Me.Close()
        End If

        If status <> 0 Then
            status = ps4000aChangePowerSource(_handle, status)
        End If

        status = ps4000aGetUnitInfo(_handle, str, 255, requiredSize, 3)

        Inputpanel.Visible = True

        '4824 has a AWG buffer size of 16Ks whixch 16 * 1024 = 16384
        awgBufferSize = 16384
        signal_type.SelectedIndex = 0
        sweep_type.SelectedIndex = 0
    End Sub

    'If dc or white noise sweep is not enable so hides button
    Private Sub signal_type_SelectedIndexChanged(sender As Object, e As EventArgs) Handles signal_type.SelectedIndexChanged
        If signal_type.SelectedIndex >= 8 Then
            Sweep.Visible = False
            Sweep.Checked = False
        Else
            Sweep.Visible = True
        End If
    End Sub

    'if check box is true then the control to set the sweep are visible
    Private Sub Sweep_CheckedChanged(sender As Object, e As EventArgs) Handles Sweep.CheckedChanged
        Sweep_control.Visible = Sweep.Checked
    End Sub

    'if system has AWG then the choices availble will depend on the ticked box
    Private Sub SIGtoAWG_CheckedChanged(sender As Object, e As EventArgs) Handles SIGtoAWG.CheckedChanged
        increment_text.Text = ""

        awg_label.Visible = SIGtoAWG.Checked
        awg_label2.Visible = SIGtoAWG.Checked
        file_name.Visible = SIGtoAWG.Checked


        'flips true and false whenever system is clicked
        siggen_label.Visible = Not siggen_label.Visible
        signal_type.Visible = Not signal_type.Visible

        If SIGtoAWG.Checked Then
            increment_text.Text = "Time increment (X*12.5ns)"
        Else
            increment_text.Text = "Increment Time Interval (s)"
        End If

    End Sub

    'depending if user wants awg or in built sig gen the diffrent points will turn up
    Private Sub Update_button_Click(sender As Object, e As EventArgs) Handles Update_button.Click

        Dim sweeptype As SweepType
        Dim operation As ExtraOperations = ExtraOperations.ps4000A_ES_OFF
        Dim shots As UInteger = 0
        Dim sweeps As UInteger = 0
        Dim triggertype As SigGenTrigType = SigGenTrigType.ps4000A_SIGGEN_RISING
        Dim triggersource As SigGenTrigSource = SigGenTrigSource.ps4000A_SIGGEN_NONE
        Dim extinthreshold As Short = 0
        Dim startfreq As Double
        Dim stopfreq As Double
        Dim increment As Double
        Dim dwelltime As Double
        Dim offsetvoltage As Integer
        Dim pktopk As UInteger

        'if variables are correct then can use function
        Try
            pktopk = UInt32.Parse(pk_pk.Text) * 1000
            offsetvoltage = Int32.Parse(offset.Text) * 1000
            startfreq = Double.Parse(start_freq.Text)
            If startfreq > 20000000 Or startfreq < 0.03 Then
                Throw New System.Exception
            End If
        Catch ex As Exception

            MessageBox.Show("Invalid for Start Frequency, PktoPk and/or Off Set Value", "Error Signal Generator Value", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Exit Sub

        End Try

        If Sweep.Checked Then
            Try
                dwelltime = Single.Parse(time_incre.Text)
                increment = Double.Parse(freq_incre.Text)
                stopfreq = Double.Parse(stop_freq.Text)
                If stopfreq > 20000000 Or stopfreq < startfreq Then

                    Throw New System.Exception

                End If
            Catch ex As Exception

                MessageBox.Show("Incorrect sweep parameter", "Error Signal Generator Value", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Exit Sub

            End Try
        Else
            stopfreq = startfreq
            increment = 0
            dwelltime = 0
            sweeptype = sweeptype.ps4000A_UP
        End If


        If SIGtoAWG.Checked Then
            'if the tick box is tru then the arbitrary waveform is dimension will be used 
            Dim file As StreamReader
            Dim waveformsize As Integer = 0
            Dim arbitarywaveform(49151) As Short
            Dim startdelta As UInteger
            Dim stopdelta As UInteger
            Dim deltaphase_increment As UInteger
            Dim index As IndexMode = IndexMode.ps4000A_SINGLE


            'making sure file exists
            Try
                file = New StreamReader(file_name.Text)
            Catch ex As Exception
                MessageBox.Show("Filename does not exsit try again", "File Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Exit Sub
            End Try

            'read all the line to the information from the file upto the maximum awg bufffer size
            Do Until file.EndOfStream Or waveformsize = awgBufferSize

                arbitarywaveform(waveformsize) = Convert.ToInt16(file.ReadLine)
                waveformsize += 1

            Loop


            file.Close()
            Array.Resize(arbitarywaveform, waveformsize) 'resizing array so no extra points

            'need to use detal phase rather than frequncy value as frequency will depend on the number of points
            startdelta = ((startfreq * waveformsize) / awgBufferSize) * (AWG_PHASE_ACCUMULATOR * 1.0 / AWG_DAC_FREQUENCY)
            stopdelta = ((stopfreq * waveformsize) / awgBufferSize) * (AWG_PHASE_ACCUMULATOR * 1.0 / AWG_DAC_FREQUENCY)
            deltaphase_increment = ((increment * waveformsize) / awgBufferSize) * (AWG_PHASE_ACCUMULATOR * 1.0 / AWG_DAC_FREQUENCY)

            status = ps4000aSetSigGenArbitrary(_handle,
                                      offsetvoltage,
                                      pktopk,
                                      startdelta,
                                      stopdelta,
                                      deltaphase_increment,
                                      dwelltime,
                                      arbitarywaveform(0),
                                      waveformsize,
                                      sweeptype,
                                      operation,
                                      index,
                                      shots,
                                      sweeps,
                                      triggertype,
                                      triggersource,
                                      extinthreshold)


        Else


            Dim wavetype As WaveType

            If (wavetype = signal_type.SelectedIndex) <= 8 Then

                'if dc the pktopk must be zero
                If signal_type.SelectedIndex = 8 Then

                    pktopk = 0
                End If


            Else

                operation = (signal_type.SelectedIndex - 8)

            End If

            status = ps4000aSetSigGenBuiltIn(_handle,
                                    offsetvoltage,
                                    pktopk,
                                    wavetype,
                                    startfreq,
                                    stopfreq,
                                    increment,
                                    dwelltime,
                                    sweeptype,
                                    operation,
                                    shots,
                                    sweeps,
                                    triggertype,
                                    triggersource,
                                    extinthreshold)
        End If

    End Sub

    'Whenever the form closes the device will be closed also
    Private Sub AWG_SIGGEN_FormClosed(sender As Object, e As FormClosedEventArgs) Handles Me.FormClosed
        ps4000aCloseUnit(_handle)
    End Sub

End Class
