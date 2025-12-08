using System.Runtime.InteropServices;

namespace SweetEditor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            EditorControl editorControl = new EditorControl();
			components.Add(editorControl);

			Document document = new Document("AAAAA\nBBB");
            Console.WriteLine("new Document");

            editorControl.LoadDocument(document);
			Console.WriteLine("LoadDocument");

			string json = editorControl.BuildRenderModel();
			Console.WriteLine(json);
		}
    }
}
