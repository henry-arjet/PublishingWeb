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

            CleanDir("../Stories-Dirty/", "../stories/");
            CleanDir("../bios-dirty/", "../bios/");
            //now for the bios

        }
        static void CleanDir(string pathDirty, string pathClean) {

            Console.WriteLine("Cleaning " + pathDirty);
            DirectoryInfo dirty = new DirectoryInfo(@pathDirty);
            foreach (FileInfo file in dirty.GetFiles())
            {
                Console.WriteLine("Cleaning " + file.Name);
                Clean(file.Name, pathDirty, pathClean);
                file.Delete();
            }
        }
        static void Clean(string name, string pathDirty, string pathClean)
        {
            string html = File.ReadAllText(pathDirty + name);

            HtmlSanitizer sanitizer = new HtmlSanitizer();
            string cleanHtml = sanitizer.Sanitize(html);

            File.WriteAllText(pathClean + name, cleanHtml);
        }
    }
}
