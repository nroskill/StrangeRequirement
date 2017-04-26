using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CreateGuid
{
    class Program
    {
        [STAThreadAttribute]
        static void Main(string[] args)
        {
            Clipboard.Clear();
            Clipboard.SetText(Guid.NewGuid().ToString());
        }
    }
}
