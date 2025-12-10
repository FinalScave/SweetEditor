using System.Diagnostics;
using System.Runtime.InteropServices;

namespace SweetEditor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            //Thread.Sleep(5000);
			Document document = new Document("AAAAA\nBBB\nCCCC\nDDDD\nEEEEEEEE\nFFFFFFFFFFF");
            Debug.WriteLine("new Document");

            editorControl1.LoadDocument(document);
			Debug.WriteLine("LoadDocument");
		}
    }
}
