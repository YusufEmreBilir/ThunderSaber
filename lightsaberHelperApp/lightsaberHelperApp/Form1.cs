using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Button;


namespace lightsaberHelperApp
{
    public partial class Form1 : Form
    {
        readonly byte COM_OFFER_CODE = 0xA1;
        readonly byte EXPECTED_RESPONSE_CODE = 0xA2;
        readonly byte COM_SUCCESSFULL = 0xAA;
        readonly byte COM_FAILED = 0xFF;

        bool connected = false;

        string[] searchThrobber = new string[3] //verimsiz ama basit :)
        {"Kılıç aranıyor.", "Kılıç aranıyor..", "Kılıç aranıyor..."};

        private BackgroundWorker backgroundWorker;

        public Form1()
        {
            InitializeComponent();
            try
            {
                serialPort1.Open();
                UpdateLabelText("Kılıç algılandı!");
            }catch { UpdateLabelText(searchThrobber[0]); }

            backgroundWorker = new BackgroundWorker();
            backgroundWorker.DoWork += new DoWorkEventHandler(DoWork);
            backgroundWorker.RunWorkerAsync();

            if (serialPort1.IsOpen)
            {
                ResetController();
            }
        }

        void DoWork(object sender, DoWorkEventArgs e)
        {
            while (true)
            {
                CheckConnection();

               
            }
        }

        void CheckConnection()
        {
            if (!serialPort1.IsOpen)
            {
                connected = false;
                for (int i = 0; i < searchThrobber.Length; i++)
                {
                    try
                    {
                        serialPort1.Open();
                        UpdateLabelText("Kılıç algılandı!");
                        return;
                    }

                    catch 
                    { 
                        UpdateLabelText(searchThrobber[i]); 
                    }
                    System.Threading.Thread.Sleep(50);
                }
                parameterGroup.Invoke((MethodInvoker)delegate { parameterGroup.Enabled = false; });
                uploadButton.Invoke((MethodInvoker)delegate { uploadButton.Enabled = false; });
                return;
            }

            if (connected) { return; }

            try
            {
                if (serialPort1.BytesToRead > 0 && (byte)serialPort1.ReadByte() == COM_OFFER_CODE)
                {
                    serialPort1.Write(new byte[] { EXPECTED_RESPONSE_CODE }, 0, 1);
                    System.Threading.Thread.Sleep(50);
                    if (serialPort1.IsOpen && serialPort1.BytesToRead > 0)
                    {
                        if ((byte)serialPort1.ReadByte() == COM_SUCCESSFULL)
                        {
                            UpdateLabelText("Kılıç hazır.Bağlantı başarılı.");
                            parameterGroup.Invoke((MethodInvoker)delegate { parameterGroup.Enabled = true; });
                            uploadButton.Invoke((MethodInvoker)delegate { uploadButton.Enabled = true; });
                            connected = true;
                        }
                        else if ((byte)serialPort1.ReadByte() == COM_FAILED)
                        {
                            UpdateLabelText("HATA: Uygulama hatası..");
                            ResetController();
                        }
                        else
                        {
                            UpdateLabelText("Bağlanıyor...");
                            ResetController();
                        }
                    }
                }
                else if (serialPort1.BytesToRead > 0)
                {
                    UpdateLabelText($"HATA: Kılıç'tan gelen kod yanlış,\ngelen byte: \n0x{serialPort1.ReadByte():X2}");
                    ResetController();
                }


                System.Threading.Thread.Sleep(50);
                    
                
            }
            catch 
            { 
                UpdateLabelText("HATA: bağlantı kesildi.");
                ResetController();
            }
        }

        void SendParameterData(byte parameterIndex, int value)
        {
            serialPort1.Write(new byte[] { parameterIndex, (byte)value }, 0, 2);
        }

        void ResetController()
        {
            connected = false;
            serialPort1.DtrEnable = false;
            System.Threading.Thread.Sleep(100);
            serialPort1.DtrEnable = true;
            System.Threading.Thread.Sleep(50);
        }

        private void UpdateLabelText(string text)
        {
            if (conStatus.InvokeRequired)
            {
                conStatus.Invoke((MethodInvoker)delegate
                {
                    conStatus.Text = text;
                });
            }
            else
            {
                conStatus.Text = text;
            }
        }

        void DisableAll()
        {
            conStatus.Invoke((MethodInvoker)delegate
            {
                uploadButton.Enabled = true;
            });
        }

        private void uploadButton_Click(object sender, EventArgs e)
        {

        }
        

        private void unstableBlade_CheckedChanged(object sender, EventArgs e)
        {
            SendParameterData(0x01, unstableBlade.Checked ? 1:0);
        }

        private void unstableDensityTB_Scroll(object sender, EventArgs e)
        {
            SendParameterData(0x02, unstableDensityTB.Value);
        }

        private void bladeRedTB_Scroll(object sender, EventArgs e)
        {
            bladeColorVisualizer.BackColor = Color.FromArgb
                (bladeRedTB.Value, bladeColorVisualizer.BackColor.G, bladeColorVisualizer.BackColor.B);

            SendParameterData(0x03, bladeRedTB.Value);
        }

        private void bladeGreenTB_Scroll(object sender, EventArgs e)
        {
            bladeColorVisualizer.BackColor = Color.FromArgb
                (bladeColorVisualizer.BackColor.R, bladeGreenTB.Value, bladeColorVisualizer.BackColor.B);

            SendParameterData(0x04, bladeGreenTB.Value); ;
        }

        private void bladeBlueTB_Scroll(object sender, EventArgs e)
        {
            bladeColorVisualizer.BackColor = Color.FromArgb
                (bladeColorVisualizer.BackColor.R, bladeColorVisualizer.BackColor.G, bladeBlueTB.Value);

            SendParameterData(0x05, bladeBlueTB.Value);
        }

        private void flickerFreqTB_Scroll(object sender, EventArgs e)
        {

        }

        private void flickerBrtnsTB_Scroll(object sender, EventArgs e)
        {

        }

        private void ignitionSpeedTB_Scroll(object sender, EventArgs e)
        {

        }

        private void ledPerStepTB_Scroll(object sender, EventArgs e)
        {

        }

        private void volumeTB_Scroll(object sender, EventArgs e)
        {

        }

        private void idleMotorTB_Scroll(object sender, EventArgs e)
        {
            maxMotorTB.Minimum = idleMotorTB.Value;
        }

        private void maxMotorTB_Scroll(object sender, EventArgs e)
        {

        }

        private void presetSelector_ValueChanged(object sender, EventArgs e)
        {

        }

    }
}
