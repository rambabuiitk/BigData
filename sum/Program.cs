using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace SampleProg
{
    class Program
    {
        static UInt64 total = 0;
        static void Main(string[] args)
        {
            BinaryWriter bw = new BinaryWriter(File.Open("test.txt", FileMode.Create));
            Random rn = new Random();
            
            for (UInt32 i = 1; i <= 500000000; ++i)
            {
                UInt32 num = (UInt32)rn.Next(0, 0xffff);
                bw.Write(num);
                total += num;
            }
            
            bw.Flush();
            bw.Close();
        }
    }
}
