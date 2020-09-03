using Ganss.XSS;
using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Sanitizer
{
    class Program
    {
        static void Main(string[] args)
        {
            DirectoryInfo dirty = new DirectoryInfo(@"../Stories-Dirty/");
            foreach (FileInfo file in dirty.GetFiles())
            {
                Console.WriteLine("Cleaning " + file.Name);
                Clean(file.Name);
                file.Delete();
            }
        }
        static void Clean(string name)
        {
            string html = File.ReadAllText("../Stories-Dirty/" + name);

            HtmlSanitizer sanitizer = new HtmlSanitizer();
            string cleanHtml = sanitizer.Sanitize(html);

            File.WriteAllText("../Stories/" + name, cleanHtml);
        }
    }
}
