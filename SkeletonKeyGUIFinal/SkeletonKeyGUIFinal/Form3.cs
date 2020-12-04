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
        CryptoPort port;
        int commMode = -1;


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
            button4.Enabled = true;
        }

        private void disableControls()
        {
            button4.Enabled = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            port.Close();
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
            SerialPort sPort = new SerialPort(selectedPort, 115200, Parity.None, 8, StopBits.One); // important contection waht buard rate esp32 needs etc.
            port = new CryptoPort(sPort);
            port.Open();
            button1.Text = "Disconnect";
            enableControls();

            byte[] bytesToSend = { Convert.ToByte('#'), Convert.ToByte(0xa), Convert.ToByte('\n') };

            port.Write(bytesToSend, 0, 3);

            port.DiscardInBuffer();
            string recvStr = port.ReadLine();
            byte[] receivedByte = Encoding.ASCII.GetBytes(recvStr);
            if (receivedByte[0] == 1)
            {
                MessageBox.Show("UART connection established");
                port.setBluetoothMode(false);
            }
            else if (receivedByte[0] == 0)
            {
                MessageBox.Show("Bluetooth connection established");
                port.setBluetoothMode(true);
            }
            else
            {
                MessageBox.Show("Unknown commMode " + Convert.ToString(receivedByte[0]) + " detected");
            }
            commMode = Convert.ToInt16(receivedByte[0]);
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
            //Clear fingerprint and Deivce
            DialogResult dialogResult = MessageBox.Show("This will clear the device. Do you wish to countinue?", "Warning", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                string end = "\n";
                byte[] endBYTE = Encoding.ASCII.GetBytes(end);
                char startCode = '#';
                byte startCodeByte = Convert.ToByte(startCode);

                byte[] StoreFingerprint = { startCodeByte, 0x07 };

                byte[] FinalBytes = Combine(StoreFingerprint, endBYTE);

                port.Write(FinalBytes, 0, FinalBytes.Length);
                MessageBox.Show("Please Scan Fingerprint");

                string str = port.ReadLine();
                if (str == "1")
                {
                    MessageBox.Show("Device Clear Success. Please Restart Device and Scan New Fingerprint.");
                }
                else if (str == "0")
                {
                    MessageBox.Show("Device Clear Failure");
                }

            }
            else if (dialogResult == DialogResult.No)
            {
                port.Close();
                this.Close();
            }
        }
    }
}
