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
    public partial class Form7 : Form
    {
        
        bool isConnected = false;
        String[] ports;
        SerialPort port;

        public Form7()
        {
            InitializeComponent();
            disableControls();
            /* Command structure [ | # | Header Byte | Size | Data | \n |]
             Software overview
             https://purdue0-my.sharepoint.com/:x:/r/personal/rodri405_purdue_edu/_layouts/15/Doc.aspx?sourcedoc=%7BBF7CB97F-7DCC-4985-B7E2-5715AE696313%7D&file=DataStructurePackages.xlsx&wdOrigin=OFFICECOM-WEB.START.REC&ct=1603399070342&action=default&mobileredirect=true
             For Week 1:
                LED Red On/Off Test	    ON:{ startCodeByte, 0x00, 0x01, 1, endCodeByte} / OFF:{ startCodeByte, 0x00, 0x01, 0, endCodeByte};
                LED Green On/Off Test	ON:{ startCodeByte, 0x01, 0x01, 1, endCodeByte}; / OFF:{ startCodeByte, 0x01, 0x01, 0, endCodeByte};
                Store Credential	    { startCodeByte, 0x04, MsgSize, DisplayName,URL,UserName,Password, endCodeByte}; 
                Request Credential	    { startCodeByte, 0x03, MsgSize, [DisplayName,URL,UserName,Password], endCodeByte};
                Request Entries	        { startCodeByte, 0x02, 0x00, 0, endCodeByte};
            */

            ports = SerialPort.GetPortNames();

            foreach (string port in ports)
            {
                comboBox1.Items.Add(port);
                Console.WriteLine(port);
                if (ports[0] != null)
                {
                    comboBox1.SelectedItem = ports[0];
                }
            }
        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (!isConnected)
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
            button1.Text = "Disconnect";
            enableControls();

            // start of Andrew's addiion
            byte[] endCodeByte = Encoding.ASCII.GetBytes("\n");
            byte[] startCodeByte = {Convert.ToByte('#'), Convert.ToByte(0xa)};

            byte[] bytesToSend = Combine(startCodeByte, endCodeByte);

            port.Write(bytesToSend, 0, 3);
            string str = port.ReadLine();
            if (str == "1") {
                MessageBox.Show("UART connection established");
            }
            else if (str == "0") {
                MessageBox.Show("Bluetooth connection established");
            }
            commMode = Int32.Parse(str);
            // end of Andrew's addiion
        }
        private void disconnectFromESP()
        {
            isConnected = false;
            port.Close();
            button1.Text = "Connect";
            disableControls();
            resetDefaults();
        }

        private void button7_Click(object sender, EventArgs e)
        {
            //Disconnect
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            //Red LED
            if (isConnected)
            {

                if (checkBox1.Checked)
                {
                    char startCode = '#';
                    byte startCodeByte = Convert.ToByte(startCode);
                    char endCode = '\n';
                    byte endCodeByte = Convert.ToByte(endCode);
                    byte[] LEDRON = { startCodeByte, 0x00, 0x01, 1, endCodeByte };
                    port.Write(LEDRON, 0, 5);

                }
                else
                {
                    char startCode = '#';
                    byte startCodeByte = Convert.ToByte(startCode);
                    char endCode = '\n';
                    byte endCodeByte = Convert.ToByte(endCode);
                    byte[] LEDROF = { startCodeByte, 0x00, 0x01, 0, endCodeByte };
                    port.Write(LEDROF, 0, 5);
                }
            }
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            //Green LED
            if (isConnected)
            {
                if (checkBox2.Checked)
                {
                    char startCode = '#';
                    byte startCodeByte = Convert.ToByte(startCode);
                    char endCode = '\n';
                    byte endCodeByte = Convert.ToByte(endCode);
                    byte[] LEDGON = { startCodeByte, 0x01, 0x01, 1, endCodeByte };
                    port.Write(LEDGON, 0, 5);
                }
                else
                {
                    char startCode = '#';
                    byte startCodeByte = Convert.ToByte(startCode);
                    char endCode = '\n';
                    byte endCodeByte = Convert.ToByte(endCode);
                    byte[] LEDGOF = { startCodeByte, 0x01, 0x01, 0, endCodeByte };
                    port.Write(LEDGOF, 0, 5);
                }
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            //Request entries  { startCodeByte, 0x02, 0x0000, endCodeByte};
            string MSG = textBox1.Text; 
            string end = "\n";
            int MSGsize = MSG.Length;
            byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
            byte[] endBYTE = Encoding.ASCII.GetBytes(end);
            byte MSGsizeByte = Convert.ToByte(MSGsize);

            char startCode = '#';
            byte startCodeByte = Convert.ToByte(startCode);
            char endCode = '\n';
            byte endCodeByte = Convert.ToByte(endCode);
            byte[] RequestEntry = { startCodeByte, 0x02, endCodeByte };

            port.Write(RequestEntry, 0, 3);
            System.Threading.Thread.Sleep(2000);

            //The ESP32 sends back the request.
            int intBuffer;
            intBuffer = port.BytesToRead;
            byte[] byteBuffer = new byte[intBuffer];
            port.Read(byteBuffer, 0, intBuffer);

            string str = System.Text.Encoding.Default.GetString(byteBuffer);
            if(str == null)
            {
                MessageBox.Show("There are no Current Entries");
            }
            textBox1.Text = str;
        }


        public static byte[] Combine(byte[] first, byte[] second)
        {
            byte[] bytes = new byte[first.Length + second.Length];
            Buffer.BlockCopy(first, 0, bytes, 0, first.Length);
            Buffer.BlockCopy(second, 0, bytes, first.Length, second.Length);
            return bytes;
        }

        private async void button4_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                //Store Credential	    { startCodeByte, 0x04, MsgSize, [DisplayName,UserName,url,Password,], endCodeByte}; 
                //Facebook,Rodri405@purdue.edu,www.facebook.com,Boomer1950!,

                string MSG = textBox1.Text;
                string end = "\n";
                int MSGsize = MSG.Length;
                byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
                byte[] endBYTE = Encoding.ASCII.GetBytes(end);
                byte MSGsizeByte = Convert.ToByte(MSGsize);

                byte Endbyte = Convert.ToByte('\n');

                char startCode = '#';
                byte startCodeByte = Convert.ToByte(startCode);

                byte[] StoreCred = { startCodeByte, 0x04, MSGsizeByte };

                byte[] newBytes = Combine(StoreCred,MSGByte);
                byte[] FinalBytes = Combine(newBytes, endBYTE);

                int freq = MSG.Split(',').Length - 1;
                if(freq == 4)
                {
                    port.Write(FinalBytes, 0, MSGsize + 4);
                    MessageBox.Show("Please Scan Fingerprint");
                    
                    string str = port.ReadLine(); 

                    if (str == "1")
                    {
                        MessageBox.Show("Credential Stored");
                        textBox1.Clear();
                    }
                    else if (str== "0")
                    {
                        MessageBox.Show("Operation Failure");
                        textBox1.Text = str;
                    }

                }
                else
                {
                    MessageBox.Show("Incorrect Format. Try Again.");
                }

            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                //Request Credential   { startCodeByte, 0x03, DisplayName size, displayName, endCodeByte};
                //Facebook,Rodri405@purdue.edu,
                string MSG = textBox1.Text;
                string end = "\n";
                int MSGsize = MSG.Length;
                byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
                byte[] endBYTE = Encoding.ASCII.GetBytes(end);
                byte MSGsizeByte = Convert.ToByte(MSGsize);

                char startCode = '#';
                byte startCodeByte = Convert.ToByte(startCode);
                byte[] RequestCred = { startCodeByte, 0x03, MSGsizeByte};

                byte[] newBytes = Combine(RequestCred, MSGByte);
                byte[] FinalBytes = Combine(newBytes, endBYTE);

                int freq = MSG.Split(',').Length - 1;
                if (freq == 2)
                {

                    port.Write(FinalBytes, 0, MSGsize + 4);
                    MessageBox.Show("Please Scan Fingerprint");

                    string str = port.ReadLine();
                    if (str == "0")
                    {
                        MessageBox.Show("Operation Failure");
                        textBox1.Clear();
                    }
                    else
                    {
                        MessageBox.Show("Operation Success");
                        textBox1.Text = str;
                    }


                }
                else
                {
                    MessageBox.Show("Incorrect Format. Try Again.");
                }
            }
        }


        private void enableControls()
        {
            checkBox1.Enabled = true;
            checkBox2.Enabled = true;
            button6.Enabled = true;
            button4.Enabled = true;
            button2.Enabled = true;
            button5.Enabled = true;
            button3.Enabled = true;
            textBox1.Enabled = true;


        }

        private void disableControls()
        {
            checkBox1.Enabled = false;
            checkBox2.Enabled = false;
            button6.Enabled = false;
            button4.Enabled = false;
            button2.Enabled = false;
            button5.Enabled = false;
            button3.Enabled = false;
            textBox1.Enabled = false;

        }

        private void resetDefaults()
        {
            checkBox1.Checked = false;
            checkBox2.Checked = false;
            textBox1.Text = "";

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void pictureBox2_Click(object sender, EventArgs e)
        {

        }

        private void button7_Click_1(object sender, EventArgs e)
        {
            port.Close();
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //Delete
            //Facebook,Rodri405@purdue.edu,
            string MSG = textBox1.Text;
            string end = "\n";
            int MSGsize = MSG.Length;
            byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
            byte[] endBYTE = Encoding.ASCII.GetBytes(end);
            byte MSGsizeByte = Convert.ToByte(MSGsize);

            char startCode = '#';
            byte startCodeByte = Convert.ToByte(startCode);


            byte[] DeleteCred = { startCodeByte, 0x06, MSGsizeByte };

            byte[] newBytes = Combine(DeleteCred, MSGByte);
            byte[] FinalBytes = Combine(newBytes, endBYTE);


            int freq = MSG.Split(',').Length - 1;
            if (freq == 2)
            {
                port.Write(FinalBytes, 0, MSGsize + 4);
                MessageBox.Show("Please Scan Fingerprint");

                string str = port.ReadLine();

                if (str == "1")
                {
                    MessageBox.Show("Credential Stored");
                    textBox1.Clear();
                }
                else if (str == "0")
                {
                    MessageBox.Show("Operation Failure");
                    textBox1.Clear();
                }
            }
            else
            {
                MessageBox.Show("Incorrect Format. Try Again.");
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            //Modify
            //Facebook,Rodri405@purdue.edu,newpassword,
            string MSG = textBox1.Text;
            string end = "\n";
            int MSGsize = MSG.Length;
            byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
            byte[] endBYTE = Encoding.ASCII.GetBytes(end);
            byte MSGsizeByte = Convert.ToByte(MSGsize);

            char startCode = '#';
            byte startCodeByte = Convert.ToByte(startCode);

            byte[] ModCred = { startCodeByte, 0x08, MSGsizeByte };

            byte[] newBytes = Combine(ModCred, MSGByte);
            byte[] FinalBytes = Combine(newBytes, endBYTE);

            int freq = MSG.Split(',').Length - 1;
            if (freq == 3)
            {
                    port.Write(FinalBytes, 0, MSGsize + 4);
                    MessageBox.Show("Please Scan Fingerprint");

                    string str = port.ReadLine();

                    if (str == "1")
                    {
                        MessageBox.Show("Credential Stored");
                        textBox1.Clear();
                    }
                    else if (str == "0")
                    {
                        MessageBox.Show("Operation Failure");
                        textBox1.Clear();
                    }
            }
            else
            {
                MessageBox.Show("Incorrect Format. Try Again.");
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
