using System.Reflection;
using System.Runtime.InteropServices;

namespace SweetEditor
{
    internal static class Program
    {
        [STAThread]
        static void Main()
        {
			NativeLibrary.SetDllImportResolver(Assembly.GetExecutingAssembly(), DllImportResolver);

			ApplicationConfiguration.Initialize();
            Application.Run(new Form1());
        }

		private static IntPtr DllImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath) {
			if (libraryName == "sweeteditor.dll") {
				string path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..\\..\\..\\..\\..\\cmake-build-debug-visual-studio\\x64-Debug\\bin", libraryName);
				if (File.Exists(path)) {
					return NativeLibrary.Load(path);
				}
			}
			return IntPtr.Zero;
		}
	}
}