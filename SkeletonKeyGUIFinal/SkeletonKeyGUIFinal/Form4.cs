using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics.Eventing.Reader;

namespace SkeletonKeyGUIFinal
{

    public partial class Form4 : Form
    {

        public Form4()
        {
            InitializeComponent();
            string readText = File.ReadAllText(@"C:\Users\TheSi\Documents\UserInfo.txt");
            string[] splittedText = readText.Split(',');
            for (int i = 4; i < ((splittedText.Length)); i+=4)
            {
                comboBox1.Items.Add(splittedText[i]);
            }

        }
        private void button1_Click(object sender, EventArgs e)
        {
            string getSelection = comboBox1.SelectedItem.ToString();
            string readText = File.ReadAllText(@"C:\Users\TheSi\Documents\UserInfo.txt");
            string[] splittedText = readText.Split(',');

            for (int i = 4; i < (splittedText.Length); i++)
            {
                if(splittedText[i] == getSelection)
                {
                    webBrowser1.Navigate(splittedText[i+1]);
                    MessageBox.Show($"Logging on!");
                    try
                    {
                        webBrowser1.Document.GetElementById("email").InnerText = splittedText[i + 2]; //id_userLoginId
                        webBrowser1.Document.GetElementById("ap_email").InnerText = splittedText[i + 2];
                        webBrowser1.Document.GetElementById("Email").InnerText = splittedText[i + 2];
                        webBrowser1.Document.GetElementById("id_userLoginId").InnerText = splittedText[i + 2]; 
                        webBrowser1.Document.GetElementById("login-username").InnerText = splittedText[i + 2];
                    }
                    catch (NullReferenceException)
                    {
                    }

                    try
                    {
                        webBrowser1.Document.GetElementById("next").InvokeMember("Click"); //submit
                        webBrowser1.Document.GetElementById("continue").InvokeMember("Click"); //submit
                    }
                    catch (NullReferenceException)
                    {
                    }

                    try
                    {
                        webBrowser1.Document.GetElementById("pass").InnerText = splittedText[i + 3]; //id_password
                        webBrowser1.Document.GetElementById("id_password").InnerText = splittedText[i + 3]; //id_password
                        webBrowser1.Document.GetElementById("login-password").InnerText = splittedText[i + 2];
                    }
                    catch (NullReferenceException)
                    {
                    }

                    try
                    {
                        webBrowser1.Document.GetElementById("u_0_b").InvokeMember("Click"); //submit
                    }
                    catch (NullReferenceException)
                    {
                    }

                    
                }
            }

        }
        
        private void button3_Click(object sender, EventArgs e)
        {
            this.Close();
        }




        #region

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void Form4_Load(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
        #endregion


    }
}
