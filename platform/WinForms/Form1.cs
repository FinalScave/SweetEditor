using System.Diagnostics;
using System.Runtime.InteropServices;

namespace SweetEditor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

			Document document = new Document("AAAAA\nBBB");
            Debug.WriteLine("new Document");

            editorControl1.LoadDocument(document);
			Debug.WriteLine("LoadDocument");
		}
    }
}
