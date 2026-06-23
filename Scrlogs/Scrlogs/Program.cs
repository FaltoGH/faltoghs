using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Scrlogs
{

    internal class Program
    {
        
        private static void Main(string[] args)
        {
            try
            {
                ScreenCapture screenCapture = new ScreenCapture();
                string myPictures = Environment.GetFolderPath(Environment.SpecialFolder.MyPictures, Environment.SpecialFolderOption.Create);
                string saveDirectory = Path.Combine(myPictures, "scrlogs");
                Directory.CreateDirectory(saveDirectory);

                while (true)
                {
                    Thread.Sleep(90000);
                    string[] files = Directory.GetFiles(saveDirectory);
                    
                    if (files.Length >= 10)
                    {
                        return;
                    }

                    string fullFilePath = Path.Combine(saveDirectory, DateTime.Now.ToString("yyyyMMddHHmmss") + ".png");
                    screenCapture.CaptureScreenToFile(fullFilePath, ImageFormat.Png);
                    Thread.Sleep(90000);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        
    }
}
