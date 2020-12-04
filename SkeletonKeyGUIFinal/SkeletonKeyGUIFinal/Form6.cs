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
            /* Command structure [ | # | Header Byte | Size | Data | \n |]
             Software overview
             https://purdue0-my.sharepoint.com/:x:/r/personal/rodri405_purdue_edu/_layouts/15/Doc.aspx?sourcedoc=%7BBF7CB97F-7DCC-4985-B7E2-5715AE696313%7D&file=DataStructurePackages.xlsx&wdOrigin=OFFICECOM-WEB.START.REC&ct=1603399070342&action=default&mobileredirect=true
             For Week 1:
                LED Red On/Off Test	    ON:{ startCodeByte, 0x00, 0x01, 1, endCodeByte} / OFF:{ startCodeByte, 0x00, 0x01, 0, endCodeByte};
                LED Green On/Off Test	ON:{ startCodeByte, 0x01, 0x01, 1, endCodeByte}; / OFF:{ startCodeByte, 0x01, 0x01, 0, endCodeByte};
                Store Credential	    { startCodeByte, 0x04, MsgSize, DisplayName,URL,UserName,Password, endCodeByte}; 
                Request Credential	    { startCodeByte, 0x03, MsgSize, [DisplayName,URL,UserName,Password], endCodeByte};
                Request Entries	        { startCodeByte, 0x02, 0x00, 0, endCodeByte};
                

               EX:

                    
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
            button1.Text = "Disconnect";
            enableControls();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (isConnected)
            {

                if (checkBox1.Checked)
                {
                    char startCode = '#';
                    byte startCodeByte = Convert.ToByte(startCode);
                    char endCode = '\n';
                    byte endCodeByte = Convert.ToByte(endCode);
                    byte[] LEDRON = { startCodeByte, 0x00, 0x01, 1, endCodeByte};
                    port.Write(LEDRON,0, 5);

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

        private void disconnectFromESP()
        {
            isConnected = false;
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
                //Store Credential	    { startCodeByte, 0x04, MsgSize, [DisplayName,UserName,url,Password,], endCodeByte}; 
                //Facebook,Rodri405@purdue.edu,www.facebook.com,Boomer1950!
                //Addd error statement if user tries to input not the correct format
                string MSG = textBox1.Text;
                string end = "\n";
                int MSGsize = MSG.Length;
                byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
                byte[] endBYTE = Encoding.ASCII.GetBytes(end);
                byte MSGsizeByte = Convert.ToByte(MSGsize);

                char startCode = '#';
                byte startCodeByte = Convert.ToByte(startCode);
  

                byte[] StoreCred = { startCodeByte, 0x04, MSGsizeByte };
                port.Write(StoreCred, 0, 3);
                port.Write(MSGByte, 0, MSGsize);
                port.Write(endBYTE, 0, 1);

                System.Threading.Thread.Sleep(2000);


                //The ESP32 sends back the request.
                int intBuffer;
                intBuffer = port.BytesToRead;
                byte[] byteBuffer = new byte[intBuffer];
                port.Read(byteBuffer, 0, intBuffer);

                string str = System.Text.Encoding.Default.GetString(byteBuffer);
                //MessageBox.Show(cred.ToString());
                if (str == "1\n")
                {
                    MessageBox.Show("Credential Stored");
                }
                else
                {
                    MessageBox.Show("Operation Failure");
                }

                

            }
        }

        //read
        private void button3_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                //Request Credential   { startCodeByte, 0x03, DisplayName size, displayName, endCodeByte};
                string MSG = textBox1.Text; //Facebook,Rodri405@purdue.edu,
                string end = "\n";
                int MSGsize = MSG.Length;
                byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
                byte[] endBYTE = Encoding.ASCII.GetBytes(end);
                byte MSGsizeByte = Convert.ToByte(MSGsize);

                char startCode = '#';
                byte startCodeByte = Convert.ToByte(startCode);
                byte[] RequestCred = { startCodeByte, 0x03, MSGsizeByte};

                port.Write(RequestCred, 0, 3);
                port.Write(MSGByte, 0, MSGsize);
                port.Write(endBYTE, 0, 1);

                System.Threading.Thread.Sleep(2000);


                //The ESP32 sends back the request.
                int intBuffer;
                intBuffer = port.BytesToRead;
                byte[] byteBuffer = new byte[intBuffer];
                port.Read(byteBuffer, 0, intBuffer);
               
                string str = System.Text.Encoding.Default.GetString(byteBuffer);
                //MessageBox.Show(cred.ToString());
                if (str != "0\n")
                {
                    MessageBox.Show(str);
                }
                else
                {
                    MessageBox.Show("Operation Failure");
                }

            }
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

        private void button5_Click(object sender, EventArgs e)
        {
            //Modify
            //Facebook,Rodri405@purdue.edu,newpassword
            string MSG = textBox1.Text;
            string end = "\n";
            int MSGsize = MSG.Length;
            byte[] MSGByte = Encoding.ASCII.GetBytes(MSG);
            byte[] endBYTE = Encoding.ASCII.GetBytes(end);
            byte MSGsizeByte = Convert.ToByte(MSGsize);

            char startCode = '#';
            byte startCodeByte = Convert.ToByte(startCode);


            byte[] StoreCred = { startCodeByte, 0x08, MSGsizeByte };
            port.Write(StoreCred, 0, 3);
            port.Write(MSGByte, 0, MSGsize);
            port.Write(endBYTE, 0, 1);

            System.Threading.Thread.Sleep(2000);

            //The ESP32 sends back the request.
            int intBuffer;
            intBuffer = port.BytesToRead;
            byte[] byteBuffer = new byte[intBuffer];
            port.Read(byteBuffer, 0, intBuffer);

            string str = System.Text.Encoding.Default.GetString(byteBuffer);
            if (str == "1\n")
            {
                MessageBox.Show("Operation Success");
            }
            else if (str == "0\n")
            {
                MessageBox.Show("Operation Failure");
            }
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


            byte[] StoreCred = { startCodeByte, 0x06, MSGsizeByte };
            port.Write(StoreCred, 0, 3);
            port.Write(MSGByte, 0, MSGsize);
            port.Write(endBYTE, 0, 1);

            System.Threading.Thread.Sleep(2000);

            //The ESP32 sends back the request.
            int intBuffer;
            intBuffer = port.BytesToRead;
            byte[] byteBuffer = new byte[intBuffer];
            port.Read(byteBuffer, 0, intBuffer);

            string str = System.Text.Encoding.Default.GetString(byteBuffer);
            if (str == "1\n")
            {
                MessageBox.Show("Operation Success");
            }
            else if (str == "0\n")
            {
                MessageBox.Show("Operation Failure");
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            //Request entries  { startCodeByte, 0x02, 0x0000, endCodeByte};
            string MSG = textBox1.Text; //Facebook,Rodri405@purdue.edu,
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
            //MessageBox.Show(cred.ToString());
            if (str != "0\n")
            {
                MessageBox.Show(str);
            }
            else
            {
                MessageBox.Show("Operation Failure");
            }

        }
    }
}
