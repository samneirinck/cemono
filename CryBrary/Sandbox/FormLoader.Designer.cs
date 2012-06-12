namespace CryEngine.Sandbox
{
	partial class FormLoader
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if(disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.uxExtensionList = new System.Windows.Forms.ListBox();
			this.label1 = new System.Windows.Forms.Label();
			this.uxExtensionInfo = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.uxExtensionLoad = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// uxExtensionList
			// 
			this.uxExtensionList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.uxExtensionList.FormattingEnabled = true;
			this.uxExtensionList.Location = new System.Drawing.Point(15, 33);
			this.uxExtensionList.Name = "uxExtensionList";
			this.uxExtensionList.Size = new System.Drawing.Size(244, 277);
			this.uxExtensionList.TabIndex = 0;
			this.uxExtensionList.SelectedIndexChanged += new System.EventHandler(this.OnExtensionSelect);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 17);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(104, 13);
			this.label1.TabIndex = 1;
			this.label1.Text = "Available Extensions";
			// 
			// uxExtensionInfo
			// 
			this.uxExtensionInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.uxExtensionInfo.Location = new System.Drawing.Point(265, 33);
			this.uxExtensionInfo.Multiline = true;
			this.uxExtensionInfo.Name = "uxExtensionInfo";
			this.uxExtensionInfo.ReadOnly = true;
			this.uxExtensionInfo.Size = new System.Drawing.Size(314, 258);
			this.uxExtensionInfo.TabIndex = 2;
			// 
			// label2
			// 
			this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(262, 17);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(119, 13);
			this.label2.TabIndex = 3;
			this.label2.Text = "Selected Extension Info";
			// 
			// uxExtensionLoad
			// 
			this.uxExtensionLoad.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.uxExtensionLoad.Location = new System.Drawing.Point(265, 297);
			this.uxExtensionLoad.Name = "uxExtensionLoad";
			this.uxExtensionLoad.Size = new System.Drawing.Size(116, 23);
			this.uxExtensionLoad.TabIndex = 4;
			this.uxExtensionLoad.Text = "Load Extension";
			this.uxExtensionLoad.UseVisualStyleBackColor = true;
			// 
			// FormLoader
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(591, 332);
			this.Controls.Add(this.uxExtensionLoad);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.uxExtensionInfo);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.uxExtensionList);
			this.Name = "FormLoader";
			this.Text = "Sandbox Extension Manager";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox uxExtensionList;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox uxExtensionInfo;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button uxExtensionLoad;
	}
}