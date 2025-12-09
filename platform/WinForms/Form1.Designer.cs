namespace SweetEditor
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

		#region Windows Form Designer generated code

		/// <summary>
		///  Required method for Designer support - do not modify
		///  the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			components = new System.ComponentModel.Container();
			editorControl1 = new EditorControl(components);
			SuspendLayout();
			// 
			// editorControl1
			// 
			editorControl1.Font = new Font("Consolas", 11F);
			editorControl1.Location = new Point(1, -1);
			editorControl1.Name = "editorControl1";
			editorControl1.Size = new Size(1827, 1174);
			editorControl1.TabIndex = 0;
			editorControl1.Text = "editorControl1";
			// 
			// Form1
			// 
			AutoScaleDimensions = new SizeF(11F, 24F);
			AutoScaleMode = AutoScaleMode.Font;
			ClientSize = new Size(1831, 1175);
			Controls.Add(editorControl1);
			Name = "Form1";
			Text = "Form1";
			ResumeLayout(false);
		}

		#endregion

		private EditorControl editorControl1;
	}
}
