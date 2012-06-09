
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
			this.uxTestTree = new System.Windows.Forms.TreeView();
			this.SuspendLayout();
			// 
			// uxTestTree
			// 
			this.uxTestTree.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.uxTestTree.Location = new System.Drawing.Point(12, 12);
			this.uxTestTree.Name = "uxTestTree";
			this.uxTestTree.Size = new System.Drawing.Size(382, 458);
			this.uxTestTree.TabIndex = 0;
			// 
			// ReportForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(832, 482);
			this.Controls.Add(this.uxTestTree);
			this.Name = "ReportForm";
			this.Text = "CryMono Unit Test Report";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TreeView uxTestTree;
	}
}