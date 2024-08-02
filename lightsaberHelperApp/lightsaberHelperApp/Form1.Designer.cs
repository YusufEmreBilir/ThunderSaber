namespace lightsaberHelperApp
{
    partial class Form1
    {
        /// <summary>
        ///Gerekli tasarımcı değişkeni.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///Kullanılan tüm kaynakları temizleyin.
        /// </summary>
        ///<param name="disposing">yönetilen kaynaklar dispose edilmeliyse doğru; aksi halde yanlış.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer üretilen kod

        /// <summary>
        /// Tasarımcı desteği için gerekli metot - bu metodun 
        ///içeriğini kod düzenleyici ile değiştirmeyin.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label6 = new System.Windows.Forms.Label();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.conStatus = new System.Windows.Forms.Label();
            this.unstableBlade = new System.Windows.Forms.CheckBox();
            this.unstableDensityTB = new System.Windows.Forms.TrackBar();
            this.unstableDensity = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.bladeRedTB = new System.Windows.Forms.TrackBar();
            this.label3 = new System.Windows.Forms.Label();
            this.bladeGreenTB = new System.Windows.Forms.TrackBar();
            this.label4 = new System.Windows.Forms.Label();
            this.bladeBlueTB = new System.Windows.Forms.TrackBar();
            this.label5 = new System.Windows.Forms.Label();
            this.flickerFreqTB = new System.Windows.Forms.TrackBar();
            this.flickerBrtnsTB = new System.Windows.Forms.TrackBar();
            this.label7 = new System.Windows.Forms.Label();
            this.ignitionSpeedTB = new System.Windows.Forms.TrackBar();
            this.label8 = new System.Windows.Forms.Label();
            this.ledPerStepTB = new System.Windows.Forms.TrackBar();
            this.label9 = new System.Windows.Forms.Label();
            this.volumeTB = new System.Windows.Forms.TrackBar();
            this.label10 = new System.Windows.Forms.Label();
            this.trackBar10 = new System.Windows.Forms.TrackBar();
            this.label11 = new System.Windows.Forms.Label();
            this.idleMotorTB = new System.Windows.Forms.TrackBar();
            this.label12 = new System.Windows.Forms.Label();
            this.maxMotorTB = new System.Windows.Forms.TrackBar();
            this.uploadButton = new System.Windows.Forms.Button();
            this.presetSelector = new System.Windows.Forms.NumericUpDown();
            this.bladeColorVisualizer = new System.Windows.Forms.PictureBox();
            this.parameterGroup = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(this.unstableDensityTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeRedTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeGreenTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeBlueTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.flickerFreqTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.flickerBrtnsTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ignitionSpeedTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ledPerStepTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.volumeTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar10)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.idleMotorTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.maxMotorTB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.presetSelector)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeColorVisualizer)).BeginInit();
            this.SuspendLayout();
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label6.Location = new System.Drawing.Point(393, 145);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(88, 50);
            this.label6.TabIndex = 13;
            this.label6.Text = "Titreme\r\nparlaklığı";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // serialPort1
            // 
            this.serialPort1.PortName = "COM4";
            // 
            // conStatus
            // 
            this.conStatus.AutoSize = true;
            this.conStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.conStatus.Location = new System.Drawing.Point(12, 9);
            this.conStatus.Name = "conStatus";
            this.conStatus.Size = new System.Drawing.Size(226, 31);
            this.conStatus.TabIndex = 0;
            this.conStatus.Text = "Durum bekleniyor\r\n";
            // 
            // unstableBlade
            // 
            this.unstableBlade.AutoSize = true;
            this.unstableBlade.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.unstableBlade.Location = new System.Drawing.Point(18, 88);
            this.unstableBlade.Name = "unstableBlade";
            this.unstableBlade.Size = new System.Drawing.Size(213, 35);
            this.unstableBlade.TabIndex = 1;
            this.unstableBlade.Text = "dengesiz bıçak";
            this.unstableBlade.UseVisualStyleBackColor = true;
            this.unstableBlade.CheckedChanged += new System.EventHandler(this.unstableBlade_CheckedChanged);
            // 
            // unstableDensityTB
            // 
            this.unstableDensityTB.Location = new System.Drawing.Point(7, 156);
            this.unstableDensityTB.Maximum = 100;
            this.unstableDensityTB.Name = "unstableDensityTB";
            this.unstableDensityTB.Size = new System.Drawing.Size(104, 45);
            this.unstableDensityTB.TabIndex = 2;
            this.unstableDensityTB.Scroll += new System.EventHandler(this.unstableDensityTB_Scroll);
            // 
            // unstableDensity
            // 
            this.unstableDensity.AutoSize = true;
            this.unstableDensity.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.unstableDensity.Location = new System.Drawing.Point(117, 139);
            this.unstableDensity.Name = "unstableDensity";
            this.unstableDensity.Size = new System.Drawing.Size(154, 62);
            this.unstableDensity.TabIndex = 3;
            this.unstableDensity.Text = "Dengesizlik\r\nSeviyesi";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label2.Location = new System.Drawing.Point(117, 266);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(162, 31);
            this.label2.TabIndex = 5;
            this.label2.Text = "Kırmızı oranı";
            // 
            // bladeRedTB
            // 
            this.bladeRedTB.BackColor = System.Drawing.Color.Red;
            this.bladeRedTB.Location = new System.Drawing.Point(13, 266);
            this.bladeRedTB.Maximum = 255;
            this.bladeRedTB.Name = "bladeRedTB";
            this.bladeRedTB.Size = new System.Drawing.Size(104, 45);
            this.bladeRedTB.TabIndex = 4;
            this.bladeRedTB.Scroll += new System.EventHandler(this.bladeRedTB_Scroll);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label3.Location = new System.Drawing.Point(117, 317);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(140, 31);
            this.label3.TabIndex = 7;
            this.label3.Text = "Yeşil oranı";
            // 
            // bladeGreenTB
            // 
            this.bladeGreenTB.BackColor = System.Drawing.Color.Lime;
            this.bladeGreenTB.Location = new System.Drawing.Point(13, 317);
            this.bladeGreenTB.Maximum = 255;
            this.bladeGreenTB.Name = "bladeGreenTB";
            this.bladeGreenTB.Size = new System.Drawing.Size(104, 45);
            this.bladeGreenTB.TabIndex = 6;
            this.bladeGreenTB.Scroll += new System.EventHandler(this.bladeGreenTB_Scroll);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label4.Location = new System.Drawing.Point(117, 364);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(138, 31);
            this.label4.TabIndex = 9;
            this.label4.Text = "Mavi oranı";
            // 
            // bladeBlueTB
            // 
            this.bladeBlueTB.BackColor = System.Drawing.Color.Blue;
            this.bladeBlueTB.Location = new System.Drawing.Point(13, 364);
            this.bladeBlueTB.Maximum = 255;
            this.bladeBlueTB.Name = "bladeBlueTB";
            this.bladeBlueTB.Size = new System.Drawing.Size(104, 45);
            this.bladeBlueTB.TabIndex = 8;
            this.bladeBlueTB.Scroll += new System.EventHandler(this.bladeBlueTB_Scroll);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label5.Location = new System.Drawing.Point(393, 71);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(80, 50);
            this.label5.TabIndex = 11;
            this.label5.Text = "Titreme\r\nfrekansı";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // flickerFreqTB
            // 
            this.flickerFreqTB.Location = new System.Drawing.Point(283, 88);
            this.flickerFreqTB.Maximum = 255;
            this.flickerFreqTB.Minimum = 1;
            this.flickerFreqTB.Name = "flickerFreqTB";
            this.flickerFreqTB.Size = new System.Drawing.Size(104, 45);
            this.flickerFreqTB.TabIndex = 10;
            this.flickerFreqTB.Value = 1;
            this.flickerFreqTB.Scroll += new System.EventHandler(this.flickerFreqTB_Scroll);
            // 
            // flickerBrtnsTB
            // 
            this.flickerBrtnsTB.Location = new System.Drawing.Point(283, 156);
            this.flickerBrtnsTB.Maximum = 100;
            this.flickerBrtnsTB.Minimum = 25;
            this.flickerBrtnsTB.Name = "flickerBrtnsTB";
            this.flickerBrtnsTB.Size = new System.Drawing.Size(104, 45);
            this.flickerBrtnsTB.TabIndex = 12;
            this.flickerBrtnsTB.Value = 25;
            this.flickerBrtnsTB.Scroll += new System.EventHandler(this.flickerBrtnsTB_Scroll);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label7.Location = new System.Drawing.Point(393, 226);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(94, 50);
            this.label7.TabIndex = 15;
            this.label7.Text = "Ateşleme\r\nhızı";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ignitionSpeedTB
            // 
            this.ignitionSpeedTB.Location = new System.Drawing.Point(283, 226);
            this.ignitionSpeedTB.Name = "ignitionSpeedTB";
            this.ignitionSpeedTB.Size = new System.Drawing.Size(104, 45);
            this.ignitionSpeedTB.TabIndex = 14;
            this.ignitionSpeedTB.Scroll += new System.EventHandler(this.ignitionSpeedTB_Scroll);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label8.Location = new System.Drawing.Point(393, 303);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(129, 50);
            this.label8.TabIndex = 17;
            this.label8.Text = "Led ateşleme\r\nhızı";
            this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ledPerStepTB
            // 
            this.ledPerStepTB.Location = new System.Drawing.Point(283, 303);
            this.ledPerStepTB.Maximum = 5;
            this.ledPerStepTB.Name = "ledPerStepTB";
            this.ledPerStepTB.Size = new System.Drawing.Size(104, 45);
            this.ledPerStepTB.TabIndex = 16;
            this.ledPerStepTB.Scroll += new System.EventHandler(this.ledPerStepTB_Scroll);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label9.Location = new System.Drawing.Point(393, 364);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(122, 25);
            this.label9.TabIndex = 19;
            this.label9.Text = "Ses seviyesi";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // volumeTB
            // 
            this.volumeTB.Location = new System.Drawing.Point(283, 364);
            this.volumeTB.Maximum = 25;
            this.volumeTB.Minimum = 15;
            this.volumeTB.Name = "volumeTB";
            this.volumeTB.Size = new System.Drawing.Size(104, 45);
            this.volumeTB.TabIndex = 18;
            this.volumeTB.Value = 15;
            this.volumeTB.Scroll += new System.EventHandler(this.volumeTB_Scroll);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label10.Location = new System.Drawing.Point(642, 88);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(141, 31);
            this.label10.TabIndex = 21;
            this.label10.Text = "Ses paketi";
            // 
            // trackBar10
            // 
            this.trackBar10.Location = new System.Drawing.Point(531, 76);
            this.trackBar10.Name = "trackBar10";
            this.trackBar10.Size = new System.Drawing.Size(104, 45);
            this.trackBar10.TabIndex = 20;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label11.Location = new System.Drawing.Point(641, 135);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(149, 62);
            this.label11.TabIndex = 23;
            this.label11.Text = "Boşta\r\nmotor gücü";
            this.label11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // idleMotorTB
            // 
            this.idleMotorTB.Location = new System.Drawing.Point(531, 150);
            this.idleMotorTB.Maximum = 200;
            this.idleMotorTB.Minimum = 75;
            this.idleMotorTB.Name = "idleMotorTB";
            this.idleMotorTB.Size = new System.Drawing.Size(104, 45);
            this.idleMotorTB.TabIndex = 22;
            this.idleMotorTB.Value = 75;
            this.idleMotorTB.Scroll += new System.EventHandler(this.idleMotorTB_Scroll);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.label12.Location = new System.Drawing.Point(641, 212);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(149, 62);
            this.label12.TabIndex = 25;
            this.label12.Text = "Maximum\r\nmotor gücü";
            this.label12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // maxMotorTB
            // 
            this.maxMotorTB.Location = new System.Drawing.Point(531, 226);
            this.maxMotorTB.Maximum = 255;
            this.maxMotorTB.Minimum = 75;
            this.maxMotorTB.Name = "maxMotorTB";
            this.maxMotorTB.Size = new System.Drawing.Size(104, 45);
            this.maxMotorTB.TabIndex = 24;
            this.maxMotorTB.Value = 75;
            this.maxMotorTB.Scroll += new System.EventHandler(this.maxMotorTB_Scroll);
            // 
            // uploadButton
            // 
            this.uploadButton.Enabled = false;
            this.uploadButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(162)));
            this.uploadButton.Location = new System.Drawing.Point(559, 347);
            this.uploadButton.Name = "uploadButton";
            this.uploadButton.Size = new System.Drawing.Size(209, 48);
            this.uploadButton.TabIndex = 28;
            this.uploadButton.Text = "Ön ayar yuvasına\r\nkaydet";
            this.uploadButton.UseVisualStyleBackColor = true;
            this.uploadButton.Click += new System.EventHandler(this.uploadButton_Click);
            // 
            // presetSelector
            // 
            this.presetSelector.Location = new System.Drawing.Point(559, 316);
            this.presetSelector.Name = "presetSelector";
            this.presetSelector.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.presetSelector.Size = new System.Drawing.Size(86, 20);
            this.presetSelector.TabIndex = 29;
            this.presetSelector.ValueChanged += new System.EventHandler(this.presetSelector_ValueChanged);
            // 
            // bladeColorVisualizer
            // 
            this.bladeColorVisualizer.BackColor = System.Drawing.Color.Black;
            this.bladeColorVisualizer.Location = new System.Drawing.Point(13, 226);
            this.bladeColorVisualizer.Name = "bladeColorVisualizer";
            this.bladeColorVisualizer.Size = new System.Drawing.Size(190, 34);
            this.bladeColorVisualizer.TabIndex = 30;
            this.bladeColorVisualizer.TabStop = false;
            // 
            // parameterGroup
            // 
            this.parameterGroup.Location = new System.Drawing.Point(5, 55);
            this.parameterGroup.Name = "parameterGroup";
            this.parameterGroup.Size = new System.Drawing.Size(785, 364);
            this.parameterGroup.TabIndex = 31;
            this.parameterGroup.TabStop = false;
            this.parameterGroup.Text = "Parametreler";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.bladeColorVisualizer);
            this.Controls.Add(this.presetSelector);
            this.Controls.Add(this.uploadButton);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.maxMotorTB);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.idleMotorTB);
            this.Controls.Add(this.trackBar10);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.volumeTB);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.ledPerStepTB);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.ignitionSpeedTB);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.flickerBrtnsTB);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.flickerFreqTB);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.bladeBlueTB);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.bladeGreenTB);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.bladeRedTB);
            this.Controls.Add(this.unstableDensity);
            this.Controls.Add(this.unstableDensityTB);
            this.Controls.Add(this.unstableBlade);
            this.Controls.Add(this.conStatus);
            this.Controls.Add(this.parameterGroup);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.unstableDensityTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeRedTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeGreenTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeBlueTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.flickerFreqTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.flickerBrtnsTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ignitionSpeedTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ledPerStepTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.volumeTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar10)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.idleMotorTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.maxMotorTB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.presetSelector)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.bladeColorVisualizer)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.CheckBox unstableBlade;
        private System.Windows.Forms.TrackBar unstableDensityTB;
        private System.Windows.Forms.Label unstableDensity;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TrackBar bladeRedTB;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TrackBar bladeGreenTB;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TrackBar bladeBlueTB;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TrackBar flickerFreqTB;
        private System.Windows.Forms.TrackBar flickerBrtnsTB;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TrackBar ignitionSpeedTB;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TrackBar ledPerStepTB;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TrackBar volumeTB;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TrackBar trackBar10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TrackBar idleMotorTB;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TrackBar maxMotorTB;
        private System.Windows.Forms.Button uploadButton;
        private System.Windows.Forms.NumericUpDown presetSelector;
        private System.Windows.Forms.PictureBox bladeColorVisualizer;
        private System.Windows.Forms.Label conStatus;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox parameterGroup;
    }
}

