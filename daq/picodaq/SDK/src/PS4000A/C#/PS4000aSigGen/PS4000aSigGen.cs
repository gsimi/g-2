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
 *   This is a program that lets you control the AWG/Signal Generator 
 *
 * Examples:
 *    Outputs signal from signal generator
 *    Loads in file and creates signal using the Arbitrary Waveform Generator
 *
 **************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PS4000aSigGen
{
    public partial class AWG_SIGGEN : Form
    {

        short handle = 0;
        UInt32 status;

        //intialise view 
        public AWG_SIGGEN()
        {
            InitializeComponent();
            file_name.Text = "Please select signal type";
            file_name.ReadOnly = true;
            SIGtoAWG.Checked = false;
            Sweep.Checked = false;
            signal_type.SelectedIndex = 0;
            sweep_type.SelectedIndex = 0;
        }

        //opens device
        private void Start_button_Click(object sender, EventArgs e)
        {
            //opens device 
            status = Imports.OpenUnit(out handle, null);

            //if handkle is zero there is a issue, will also need to change power source if not using a USB 3.0 port
            if (handle == 0)
            {
                MessageBox.Show("Cannot open device error code: " + status.ToString(), "Error Opening Device", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
            }
            else if (status != 0)
            {
                status = Imports.ps4000aChangePowerSource(handle, status);
            }

            controls.Visible = true;
        }

        //when the form is closed, diconnect device
        private void AWG_SIGGEN_Close(object sender, FormClosedEventArgs e)
        {
            Imports.CloseUnit(handle);
        }

        //changes from signal generator to abitary waveform geerator
        private void SIGtoAWG_CheckedChanged(object sender, EventArgs e)
        {
            awg_label.Visible = SIGtoAWG.Checked;
            awg_label2.Visible = SIGtoAWG.Checked;
            signal_type.Visible = !SIGtoAWG.Checked;

            if (SIGtoAWG.Checked)
            {
                file_name.Clear();
                file_name.ReadOnly = false;
            }
            else
            {
                file_name.Text = "Please select signal type";
                file_name.ReadOnly = true;
            }
        }

        //enables sweep controls
        private void Sweep_CheckedChanged(object sender, EventArgs e)
        {
            SweepController.Visible = Sweep.Checked;
        }

        //If dc or white noise sweep is not enable so hides button
        private void signal_type_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (signal_type.SelectedIndex == 8 || signal_type.SelectedIndex == 9)
            {
                Sweep.Checked = false;
                Sweep.Enabled = false;
            }
            else
            {
                Sweep.Enabled = true;
            }

        }


        private void Update_button_Click(object sender, EventArgs e)
        {
            Imports.SweepType sweeptype = Imports.SweepType.PS4000A_UP;
            Imports.ExtraOperations operations = Imports.ExtraOperations.PS4000A_ES_OFF;
            uint shots = 0;
            uint sweeps = 0;
            Imports.SigGenTrigType triggertype = Imports.SigGenTrigType.PS4000A_SIGGEN_RISING;
            Imports.SigGenTrigSource triggersource = Imports.SigGenTrigSource.PS4000A_SIGGEN_NONE;
            short extinthreshold = 0;
            double stopfreq;
            double startfreq;
            double increment;
            double dwelltime;
            int offset;
            uint pktopk;


            try
            {
                startfreq = Convert.ToDouble(start_freq.Text);
                pktopk = Convert.ToUInt32(pk_pk.Text) * 1000;
                offset = Convert.ToInt32(offsetvoltage.Text);

            }
            catch
            {
                MessageBox.Show("Error with start frequency, offset and/or pktopk", "INVALID VALUES", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (Sweep.Checked)
            {
                try
                {
                    stopfreq = Convert.ToDouble(stop_freq.Text);
                    increment = Convert.ToDouble(freq_incre.Text);
                    dwelltime = Convert.ToDouble(time_incre.Text);
                    sweeptype = (Imports.SweepType)(sweep_type.SelectedIndex);
                }
                catch
                {
                    MessageBox.Show("Sweep values are incorrect", "INCORRECT VALUES", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

            }
            else
            {
                stopfreq = startfreq;
                increment = 0;
                dwelltime = 0;
                sweeptype = Imports.SweepType.PS4000A_UP;
            }

            if (SIGtoAWG.Checked)
            {
                Imports.IndexMode index = Imports.IndexMode.PS4000A_SINGLE;
                int waveformsize = 0;
                string line;
                System.IO.StreamReader file;
                short[] waveform = new short[Imports.SIG_GEN_BUFFER_SIZE];

                try
                {
                    file = new System.IO.StreamReader(file_name.Text);
                }
                catch
                {
                    MessageBox.Show("Cannnot open file", "Error file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                while ((line = file.ReadLine()) != null && waveformsize < (Imports.SIG_GEN_BUFFER_SIZE))
                {
                    waveform[waveformsize] = Convert.ToInt16(line);
                    waveformsize++;
                }

                file.Close();


                Array.Resize(ref waveform, waveformsize);

                //As frequency depends on the number or points need to use delta phase 
                uint startdeltaphase = (uint)(((1.0 * startfreq * waveformsize) / Imports.SIG_GEN_BUFFER_SIZE) * (1.0 * Imports.AWG_PHASE_ACCUMULATOR / Imports.AWG_DAC_FREQUENCY));
                uint stopdeltaphase = (uint)(((1.0 * stopfreq * waveformsize) / Imports.SIG_GEN_BUFFER_SIZE) * (1.0 * Imports.AWG_PHASE_ACCUMULATOR / Imports.AWG_DAC_FREQUENCY));
                uint _increment = (uint)(((1.0 * increment * waveformsize) / Imports.SIG_GEN_BUFFER_SIZE) * (1.0 * Imports.AWG_PHASE_ACCUMULATOR / Imports.AWG_DAC_FREQUENCY));
                uint dwell = (uint)(dwelltime);



                if ((status = Imports.SetSigGenArbitrary(handle,
                    offset,
                   pktopk,
                  startdeltaphase,
                  stopdeltaphase,
                  _increment,
                  dwell,
                  ref waveform[0],
                  waveformsize,
                  sweeptype,
                  operations,
                  index,
                  shots,
                  sweeps,
                  triggertype,
                  triggersource,
                  extinthreshold)) != 0)
                {
                    MessageBox.Show("Error SetSigGenArbitray error code :" + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

            }
            else
            {
                Imports.WaveType wavetype = Imports.WaveType.PS4000A_SINE;

                if (signal_type.SelectedIndex < 9)
                {
                    if ((wavetype = (Imports.WaveType)(signal_type.SelectedIndex)) == Imports.WaveType.PS4000A_DC_VOLTAGE)
                    {
                        pktopk = 0;
                    }

                }
                else
                {
                    operations = (Imports.ExtraOperations)(signal_type.SelectedIndex - 8);
                }


                if ((status = Imports.SetSigGenBuiltIn(handle,
                    offset,
                    pktopk,
                    wavetype,
                    startfreq,
                    stopfreq,
                    increment,
                    dwelltime,
                    sweeptype,
                    operations,
                    shots,
                    sweeps,
                    triggertype,
                    triggersource,
                    extinthreshold
                    )) != 0)
                {
                    MessageBox.Show("Error SetSigGenBuiltIn error code :" + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

                }
            }
        }

    }
}
