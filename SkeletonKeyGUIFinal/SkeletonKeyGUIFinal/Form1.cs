using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SkeletonKeyGUIFinal
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            customizeDesign();
        }
        private void customizeDesign()
        {
            panelUserSubmenu.Visible = false;
            panelConnectSkelKeySubMenu.Visible = false;
            panelPassManagerSubMenu.Visible = false;
        }

        //Hide SubMenus
        private void HideSubMenu()
        {
            if (panelUserSubmenu.Visible == true)
                panelUserSubmenu.Visible = false;
            if (panelConnectSkelKeySubMenu.Visible == true)
                panelConnectSkelKeySubMenu.Visible = false;
            if (panelPassManagerSubMenu.Visible == true)
                panelPassManagerSubMenu.Visible = false;
        }

        //Function to Show Menus during click event
        private void showSubMenu(Panel subMenu)
        {
            if (subMenu.Visible == false)
            {
                HideSubMenu();
                subMenu.Visible = true;
            }
            else
                subMenu.Visible = false;
        }

        #region User Menu
        //User Button
        private void buttonUser_Click(object sender, EventArgs e)
        {
            showSubMenu(panelUserSubmenu);
        }

        //SubButtons under User button
        private void button2_Click(object sender, EventArgs e)
        {
            openChildForm(new Form2());
            //...Insert Code
            HideSubMenu();
        }
        private void button3_Click(object sender, EventArgs e)
        {
            openChildForm(new Form3());
            //... Insert Code
            HideSubMenu();
        }
#endregion

        #region Connect Device Menu
        //Connect Device button
        private void buttonConnectSkelKey_Click(object sender, EventArgs e)
        {
            showSubMenu(panelConnectSkelKeySubMenu);
        }

        //SubButtons Under Connent Device Button
        private void button5_Click(object sender, EventArgs e)
        {
            //...Insert Code
            HideSubMenu();
        }
        private void button4_Click(object sender, EventArgs e)
        {
            //...Insert Code
            openChildForm(new Form6());
            HideSubMenu();
        }
#endregion

        #region Password Manager Menu
        //Password Manager Button
        private void buttonPassManager_Click(object sender, EventArgs e)
        {
            showSubMenu(panelPassManagerSubMenu);
        }

        //Subuttons Under Password Manager Button
        private void button8_Click(object sender, EventArgs e)
        {
            openChildForm(new Form5());
            HideSubMenu();
        }
        private void button7_Click(object sender, EventArgs e)
        {
            openChildForm(new Form4());
            HideSubMenu();
        }

        #endregion

        #region Website Menu
        //Project Website Button
        private void buttonProjectWebsite_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("https://engineering.purdue.edu/477grp8/");
            HideSubMenu();
        }
        #endregion

        #region Help Menu 
        //Help Button 
        private void buttonHelp_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("https://engineering.purdue.edu/477grp8/Files/docs/12_UserManual.docx");
            HideSubMenu();
        }
        #endregion

        //This is to open seconardy forms after clicking a button
        private Form activeForm = null;
        private void openChildForm(Form childForm)
        {
            if (activeForm != null)
                activeForm.Close();
            activeForm = childForm;
            childForm.TopLevel = false;
            childForm.FormBorderStyle = FormBorderStyle.None;
            childForm.Dock = DockStyle.Fill;
            panelChildForm.Controls.Add(childForm);
            panelChildForm.Tag = childForm;
            childForm.BringToFront();
            childForm.Show(); 
        }

    }
}
