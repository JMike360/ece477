using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO.Ports;
using System.Linq;
using System.Security.Permissions;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SkeletonKeyGUIFinal
{
    class CryptoPort
    {
        protected SerialPort port;
        protected RsaCrypto rsa;
        private bool isModeBt;
        private static List<String> lines = new List<string>();
        public int BytesToRead
        {
            get { return port.BytesToRead; }
        }
        public CryptoPort(SerialPort port)
        {
            this.port = port;
            isModeBt = false;
            rsa = new RsaCrypto();
        }

        public CryptoPort(SerialPort port, bool isModeBt) : this(port)
        {
            this.isModeBt = isModeBt;
        }

        public void setBluetoothMode(bool isSet)
        {
            if(!isModeBt && isSet)
            {
                RsaKeyExchange();
            }
            if(isModeBt && !isSet)
            {
                DisableRsa();
            }
            isModeBt = isSet;
        }

        private void RsaKeyExchange()
        {
            rsa.initGuiCsp();
            rsa.recvEspPublicKey(port);
            rsa.sendGuiPublicKey(port);
        }

        private void DisableRsa()
        {
            if (isModeBt)
            {
                rsa.deinitGuiCsp();
                byte[] toSend = { Convert.ToByte('#'), 0x0C };
                Write(toSend, 0, toSend.Length);
            }
        }

        public void Open()
        {
            port.Open();
        }

        public void DiscardInBuffer()
        {
            port.DiscardInBuffer();
        }

        public void Write(byte[] buffer, int offset, int count)
        {
            if (!isModeBt)
            {
                port.Write(buffer, offset, count);
            }
            else
            {
                System.Diagnostics.Trace.WriteLine("Sending...");
                System.Diagnostics.Trace.WriteLine("Plaintext:");
                foreach(var c in buffer)
                {
                    System.Diagnostics.Trace.Write(c.ToString("x2"));
                    System.Diagnostics.Trace.Write(" ");
                }
                System.Diagnostics.Trace.Write("\n");

                byte[] ciphertext = rsa.encryptPacketFromGui(buffer);
                System.Diagnostics.Trace.WriteLine("Ciphertext:");
                int i = 0;
                foreach (var c in ciphertext)
                {
                    if (i++ % 16 == 0)
                    {
                        System.Diagnostics.Trace.Write('\n');
                    }
                    System.Diagnostics.Trace.Write(c.ToString("x2"));
                    System.Diagnostics.Trace.Write(" ");
                }
                System.Diagnostics.Trace.Write("\n");
                port.Write(ciphertext, 0, ciphertext.Length);
            }
        }

        public void Write(string text)
        {
            if (!isModeBt)
            {
                port.Write(text);
            }
            else
            {
                System.Diagnostics.Trace.WriteLine("Sending...");
                byte[] bytes = Encoding.ASCII.GetBytes(text);
                byte[] ciphertext = rsa.encryptPacketFromGui(bytes);
                System.Diagnostics.Trace.WriteLine("Ciphertext:");
                int i = 0;
                foreach (var c in ciphertext)
                {
                    if(i++%16 == 0)
                    {
                        System.Diagnostics.Trace.Write('\n');
                    }
                    System.Diagnostics.Trace.Write(c.ToString("x2"));
                    System.Diagnostics.Trace.Write(" ");
                }
                System.Diagnostics.Trace.Write("\n");
                port.Write(ciphertext, 0, ciphertext.Length);
            }
        }

        public void Read(byte[] buffer, int offset, int count) 
        {
            if (!isModeBt)
            {
                port.Read(buffer, offset, count);
            }
            else
            {
                System.Diagnostics.Trace.WriteLine("Receiving...");
                port.Read(buffer, offset, count);
                if(buffer.Length == 0)
                {
                    System.Diagnostics.Trace.WriteLine("Did not receive any data");
                    return;
                }
                System.Diagnostics.Trace.WriteLine("Ciphertext:");
                int i = 0;
                foreach (var c in buffer)
                {
                    if (i++ % 16 == 0)
                    {
                        System.Diagnostics.Trace.Write('\n');
                    }
                    System.Diagnostics.Trace.Write(c.ToString("x2"));
                    System.Diagnostics.Trace.Write(" ");
                }
                System.Diagnostics.Trace.Write("\n");
                //List<byte> list = buffer.ToList();
                //list.Reverse();
                //buffer = list.ToArray();
                byte[] plaintext = rsa.decryptPacketFromEsp(buffer);
                Array.Clear(buffer, 0, buffer.Length);
                plaintext.CopyTo(buffer, 0);              
            }
        }

        public string ReadLine()
        {
            if (!isModeBt)
            {
                return port.ReadLine();
            }
            else
            {
                if(lines.Count == 0)
                {
                    int ms_wait = 0;
                    while(port.BytesToRead < 256)
                    {
                        Thread.Sleep(1);
                        if(++ms_wait > 30000)
                        {
                            break;
                        }
                    }

                    int length = port.BytesToRead;
                    byte[] buffer = new byte[length];
                    port.Read(buffer, 0, length);
                    System.Diagnostics.Trace.WriteLine("Receiving...");
                    if (buffer.Length == 0)
                    {
                        System.Diagnostics.Trace.WriteLine("Did not receive any data");
                        return "";
                    }
                    System.Diagnostics.Trace.WriteLine("Ciphertext:");
                    int i = 0;
                    foreach (var c in buffer)
                    {
                        if (i++ % 16 == 0)
                        {
                            System.Diagnostics.Trace.Write('\n');
                        }
                        System.Diagnostics.Trace.Write(c.ToString("x2"));
                        System.Diagnostics.Trace.Write(" ");
                    }
                    System.Diagnostics.Trace.Write("\n");
                    byte[] plaintext = rsa.decryptPacketFromEsp(buffer);
                    string converted = Encoding.UTF8.GetString(plaintext, 0, plaintext.Length);

                    lines = converted.Trim().Split('\n').ToList();
                }
                if (lines.Count > 0)
                {
                    string line = lines[0];
                    lines.RemoveAt(0);
                    return line;
                }
                else
                {
                    return "";
                }
            }
        }

        public void Close()
        {
            if (!isModeBt)
            {
                port.Close();
            }
            else
            {
                DisableRsa();
                port.Close();
            }
            
        }
    }
}
