using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO.Ports;
using System.Linq;
using System.Security.AccessControl;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SkeletonKeyGUIFinal
{
    class RsaCrypto : IRsaCrypto
    {
        private static RSACryptoServiceProvider guiRsaProvider;
        private static RSACryptoServiceProvider espRsaProvider;
        private static bool isInit = false;

        public byte[] decryptPacketFromEsp(byte[] ciphertext)
        {
            string expected = "Facebook,reachjohnm@gmail.com,www.facebook.com/login,\nNetflix,jmike360,www.netflix.com,\n";
            byte[] pt = Encoding.ASCII.GetBytes(expected);
            byte[] ct = guiRsaProvider.Encrypt(pt, false);
            System.Diagnostics.Trace.Write("Sample ciphertext:");
            int i = 0;
            foreach (var c in ct)
            {
                if ((i++ % 16) == 0)
                {
                    System.Diagnostics.Trace.Write("\n");
                }
                System.Diagnostics.Trace.Write(c.ToString("x2"));
                System.Diagnostics.Trace.Write(" ");
            }
            System.Diagnostics.Trace.Write("\n");
            byte[] decrypted = guiRsaProvider.Decrypt(ct, false);
            string decString = Encoding.ASCII.GetString(decrypted);
            System.Diagnostics.Trace.WriteLine("Decrypted sample:");
            System.Diagnostics.Trace.WriteLine(decString);
            byte[] plaintext = guiRsaProvider.Decrypt(ciphertext, false);
            try
            {
                byte[] plaintext = guiRsaProvider.Decrypt(ciphertext, false);
                return plaintext;
            }
            catch(System.Security.Cryptography.CryptographicException e)
            {
                MessageBox.Show("Failed to decrypt data sent from device.");
                byte[] ret = { };
                return ret;
            }
            
            
        }

        public byte[] encryptPacketFromGui(byte[] plaintext)
        {
            byte[] ciphertext = espRsaProvider.Encrypt(plaintext, false);
            return ciphertext;
        }

        public void initGuiCsp()
        {
            if (!isInit)
            {
                guiRsaProvider = new RSACryptoServiceProvider(2048);
                isInit = true;
            }
        }

        public void deinitGuiCsp()
        {
            isInit = false;
            guiRsaProvider = null;
        }

        public void recvEspPublicKey(SerialPort inPort)
        {
            //Grab packet from esp32 for parameters
            byte[] recv = new byte[266];
            inPort.Read(recv, 0, 266);

            //Now Create the CSP
            byte[] exponent = recv.Skip(0).Take(4).ToArray();
            List<byte> list = exponent.ToList();
            list.Reverse();
            exponent = list.ToArray();
            byte[] modulus = recv.Skip(9).Take(256).ToArray();
            list = modulus.ToList();
            list.Reverse();
            modulus = list.ToArray();

            System.Diagnostics.Trace.WriteLine("Received:");
            System.Diagnostics.Trace.WriteLine("Exponent:");
            foreach(var c in exponent)
            {
                System.Diagnostics.Trace.Write(c.ToString("x2"));
                System.Diagnostics.Trace.Write(" ");
            }
            System.Diagnostics.Trace.Write("\nModulus:");
            int i = 0;
            foreach(var c in modulus)
            {
                if((i++%16) == 0)
                {
                    System.Diagnostics.Trace.Write("\n");
                }
                System.Diagnostics.Trace.Write(c.ToString("x2"));
                System.Diagnostics.Trace.Write(" ");
            }
            System.Diagnostics.Trace.Write("\n");

            RSAParameters rsaParams = new RSAParameters();
            rsaParams.Modulus = modulus;
            rsaParams.Exponent = exponent;

            espRsaProvider = new RSACryptoServiceProvider();
            espRsaProvider.ImportParameters(rsaParams);

        }

        public void sendGuiPublicKey(SerialPort outPort)
        {
            RSAParameters rsaParams = guiRsaProvider.ExportParameters(false);
            byte[] exponent = new byte[8];
            rsaParams.Exponent.CopyTo(exponent,0);

            byte[] modulus = rsaParams.Modulus;
            List<byte> list = modulus.ToList();
            list.Reverse();
            modulus = list.ToArray();

            System.Diagnostics.Trace.WriteLine("Sizes:");
            System.Diagnostics.Trace.WriteLine(exponent.Length);
            System.Diagnostics.Trace.WriteLine(modulus.Length);
            System.Diagnostics.Trace.WriteLine("Sending:");
            //Send exponent to the esp32
            byte[] toSend = { Convert.ToByte('#'), Convert.ToByte(0x0b) };
            toSend = toSend.Concat(exponent).ToArray();
            toSend = toSend.Append(Convert.ToByte('\n')).ToArray();
            toSend = toSend.Concat(modulus).ToArray();
            toSend = toSend.Append(Convert.ToByte('\n')).ToArray();

            int i = 0;
            foreach(var c in toSend)
            {
                if((i++%16) == 0)
                {
                    System.Diagnostics.Trace.Write("\n");
                }
                System.Diagnostics.Trace.Write(c.ToString("x2"));
                System.Diagnostics.Trace.Write(" ");
            }
            System.Diagnostics.Trace.Write("\n");

            outPort.Write(toSend, 0, toSend.Length);
        }
    }
}
