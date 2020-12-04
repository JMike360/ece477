using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SkeletonKeyGUIFinal
{
    interface IRsaCrypto
    {
        void initGuiCsp();
        void sendGuiPublicKey(SerialPort outPort);
        void recvEspPublicKey(SerialPort inPort);
        byte[] encryptPacketFromGui(byte[] plaintext);
        byte[] decryptPacketFromEsp(byte[] ciphertext);
    }
}
