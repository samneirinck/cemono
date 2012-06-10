
namespace CryEngine.Testing.Internals
{
	partial class ReportForm
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ReportForm));
			this.uxTestTree = new System.Windows.Forms.TreeView();
			this.uxTreeImages = new System.Windows.Forms.ImageList(this.components);
			this.SuspendLayout();
			// 
			// uxTestTree
			// 
			this.uxTestTree.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.uxTestTree.ImageIndex = 0;
			this.uxTestTree.ImageList = this.uxTreeImages;
			this.uxTestTree.Location = new System.Drawing.Point(12, 12);
			this.uxTestTree.Name = "uxTestTree";
			this.uxTestTree.SelectedImageIndex = 0;
			this.uxTestTree.Size = new System.Drawing.Size(382, 458);
			this.uxTestTree.TabIndex = 0;
			// 
			// uxTreeImages
			// 
			this.uxTreeImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("uxTreeImages.ImageStream")));
			this.uxTreeImages.TransparentColor = System.Drawing.Color.Transparent;
			this.uxTreeImages.Images.SetKeyName(0, "TestSuccess.png");
			this.uxTreeImages.Images.SetKeyName(1, "TestFailed.png");
			this.uxTreeImages.Images.SetKeyName(2, "TestIgnored.png");
			// 
			// ReportForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(832, 482);
			this.Controls.Add(this.uxTestTree);
			this.Name = "ReportForm";
			this.Text = "CryMono Unit Test Report";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.OnClose);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TreeView uxTestTree;
		private System.Windows.Forms.ImageList uxTreeImages;
	}
}