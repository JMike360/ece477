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
using System.Security.Cryptography;

// RSA algorithm reference:
// https://docs.microsoft.com/en-us/dotnet/api/system.security.cryptography.rsacryptoserviceprovider?view=net-5.0

namespace SkeletonKeyGUIFinal
{
    public partial class Form3 : Form
    {
        bool isConnected = false;
        int commMode = -1;
        String[] ports;
        SerialPort port;
        RSAParameters my_rsa;
        RSAParameters client_rsa;

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
            port = new SerialPort(selectedPort, 115200, Parity.None, 8, StopBits.One); // important contection waht buard rate esp32 needs etc.
            port.Open();
            button1.Text = "Disconnect";
            enableControls();

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
                rsaKeyExchange();
            }
            commMode = Int32.Parse(str);
        }
        private void disconnectFromESP()
        {
            isConnected = false;
            port.Close();
            button1.Text = "Connect";
            disableControls();
        }
        private void rsaKeyExchange() {
            // step 1. GUI connect to ESP32 via unknown COM port
            // step 2. ESP32 informs GUI that COM port was for Bluetooth
            // step 3. ESP32 sends public key to GUI
            // step 4. GUI sends public key to ESP32

            // receive RSA public key from ESP32
            UnicodeEncoding ByteConverter = new UnicodeEncoding();
            client_rsa = new RSAParameters();
            client_rsa.Exponent = ByteConverter.GetBytes(port.ReadLine());
            client_rsa.Modulus = ByteConverter.GetBytes(port.ReadLine());

            // generate and sent RSA public key to ESP32
            RSACryptoServiceProvider rsa_provider = new RSACryptoServiceProvider();
            my_rsa = rsa_provider.ExportParameters(true);
            // public exponent:     8 bytes
            // public modulus:      512 bytes
            // two divider chars:   2 bytes
            // total send length:    522 bytes
            int send_len = 522;
            byte[] byteToSend = {Convert.ToByte('#'), Convert.ToByte(0xb), Convert.ToByte(send_len)};
            byte[] sepByte = Encoding.ASCII.GetBytes("\n");
            byteToSend = Combine(byteToSend, my_rsa.Exponent);
            byteToSend = Combine(byteToSend, sepByte);
            byteToSend = Combine(byteToSend, my_rsa.Modulus);
            byteToSend = Combine(byteToSend, sepByte);
            port.Write(byteToSend, 0, send_len);
            // port.ReadLine(); //read rsa param from ESP32
        }

        private void button4_Click(object sender, EventArgs e)
        {
            //Clear fingerprint and Deivce
            DialogResult dialogResult = MessageBox.Show("Sure", "This will clear the device. Do you wish to countinue", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                string end = "\n";
                byte[] endBYTE = Encoding.ASCII.GetBytes(end);
                char startCode = '#';
                byte startCodeByte = Convert.ToByte(startCode);

                byte[] StoreFingerprint = { startCodeByte, 0x07 };

                byte[] FinalBytes = Combine(StoreFingerprint, endBYTE);

                port.Write(FinalBytes, 0, 3);
                MessageBox.Show("Please Scan Fingerprint");

                string str = port.ReadLine();
                if (str == "1")
                {
                    MessageBox.Show("Device Clear Success. Please Restart Device and Scan New Fingerprint when Device Lights Green");
                }
                else if (str == "0")
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
