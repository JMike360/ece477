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
    public partial class Form3 : Form
    {
        bool isConnected = false;
        String[] ports;
        SerialPort port;

        public Form3()
        {
            InitializeComponent();
            disableControls();

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

        private void enableControls()
        {
            button5.Enabled = true;
            button4.Enabled = true;
        }

        private void disableControls()
        {
            button5.Enabled = false;
            button4.Enabled = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Form3_Load(object sender, EventArgs e)
        {

        }
        public static byte[] Combine(byte[] first, byte[] second)
        {
            byte[] bytes = new byte[first.Length + second.Length];
            Buffer.BlockCopy(first, 0, bytes, 0, first.Length);
            Buffer.BlockCopy(second, 0, bytes, first.Length, second.Length);
            return bytes;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            //Enroll
            string end = "\n";
            byte[] endBYTE = Encoding.ASCII.GetBytes(end);
            char startCode = '#';
            byte startCodeByte = Convert.ToByte(startCode);

            byte[] StoreFingerprint = { startCodeByte, 0x05 };

            byte[] FinalBytes = Combine(StoreFingerprint, endBYTE);
            port.Write(FinalBytes, 0, 4);

            System.Threading.Thread.Sleep(5000);

            int intBuffer;
            intBuffer = port.BytesToRead;
            byte[] byteBuffer = new byte[intBuffer];
            port.Read(byteBuffer, 0, intBuffer);

            string str = System.Text.Encoding.Default.GetString(byteBuffer);

            if (str == "1\n")
            {
                MessageBox.Show("Fingerprint Enrollment Success");
            }
            else
            {
                MessageBox.Show("Fingerprint Enrollment Failed");
            }
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
        }
        private void disconnectFromESP()
        {
            isConnected = false;
            port.Close();
            button1.Text = "Connect";
            disableControls();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            DialogResult dialogResult = MessageBox.Show("Sure", "This will clear the device. Do you wish to countinue", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                string end = "\n";
                byte[] endBYTE = Encoding.ASCII.GetBytes(end);
                char startCode = '#';
                byte startCodeByte = Convert.ToByte(startCode);

                byte[] StoreFingerprint = { startCodeByte, 0x07 };

                byte[] FinalBytes = Combine(StoreFingerprint, endBYTE);
                port.Write(FinalBytes, 0, 4);

                MessageBox.Show("Please Scan Fingerprint");

                System.Threading.Thread.Sleep(5000);

                int intBuffer;
                intBuffer = port.BytesToRead;
                byte[] byteBuffer = new byte[intBuffer];
                port.Read(byteBuffer, 0, intBuffer);

                string str = System.Text.Encoding.Default.GetString(byteBuffer);

                if (str == "1\n")
                {
                    MessageBox.Show("Device Clear Success");
                }
                else
                {
                    MessageBox.Show("Device Clear Failure");
                }
            }
            else if (dialogResult == DialogResult.No)
            {
                this.Close();
            }
        }
    }
}
