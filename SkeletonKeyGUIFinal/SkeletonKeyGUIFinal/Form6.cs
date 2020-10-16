using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace SkeletonKeyGUIFinal
{
    public partial class Form6 : Form
    {
        bool isConnected = false;
        String[] ports;
        SerialPort port;
        public Form6()
        {
            InitializeComponent();
            disableControls();
            /* Idea for Command structure Demo [ | # | Command | Data | \n | ] with Following Commands: Strt, Stop, Read, Wrte, LedR, LedG
             Strt Command tells ESP32 to wait for GUI commands 
             Stop Command Tells ESP32 to stop waiting for GUI commands
             Read Command Tells ESP32 that The GUI wants to read to the SD card onboard
             Wrte Command Tells ESP32 That the GUI wants to write to the SD card onboard
             LEDR and LEDG tell the ESP32 which LED the GUI wants on

               EX:
                    #WrteILoveECE477\n 
                    #readSavedPassText\n
                    #LEDRON\n 
                    #LEDROF\n
                    #LEDRON\n 
                    #LEDROF\n
                    
            */
            ports = SerialPort.GetPortNames();

           foreach (string port in ports)
            {
                comboBox1.Items.Add(port);
                Console.WriteLine(port);
                if(ports[0] != null)
                {
                    comboBox1.SelectedItem = ports[0];
                }
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(!isConnected)
            {
                connectToESP();
            }
            else
            {
                disconnectFromESP();
            }
        }

        private void connectToESP()
        {
            isConnected = true;
            string selectedPort = comboBox1.GetItemText(comboBox1.SelectedItem); //Command
            port = new SerialPort(selectedPort, 115200, Parity.None, 8, StopBits.One); // important contection waht buard rate esp32 needs etc.
            port.Open();
            //port.Write("#STRT\n");
            button1.Text = "Disconnect";
            enableControls();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (isConnected)
            {

                if (checkBox1.Checked)
                {
                    port.WriteLine("#LEDRON\n");

                }
                else
                {
                    port.WriteLine("#LEDROF\n");
                }
            }
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (isConnected)
            {
                if (checkBox2.Checked)
                {
                    port.Write("#LEDGON\n");
                }
                else
                {
                    port.Write("#LEDGOF\n");
                }
            }
        }
        private void disconnectFromESP()
        {
            isConnected = false;
            //port.Write("#STOP\n");
            port.Close();
            button1.Text = "Connect";
            disableControls();
            resetDefaults();
        }

        //write
        private void button4_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                port.Write("#Wrte" + textBox1.Text + "#\n");
            }
        }

        //read
        private void button3_Click(object sender, EventArgs e)
        {
            //Port.Read()
        }

        private void enableControls()
        {
            checkBox1.Enabled = true;
            checkBox2.Enabled = true;
            button1.Enabled = true;
            button3.Enabled = true;
            button4.Enabled = true;
            textBox1.Enabled = true;


        }

        private void disableControls()
        {
            checkBox1.Enabled = false;
            checkBox2.Enabled = false;          
            button3.Enabled = false;
            button4.Enabled = false;
            textBox1.Enabled = false;

        }

        private void resetDefaults()
        {
            checkBox1.Checked = false;
            checkBox2.Checked = false;
            textBox1.Text = "";

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
