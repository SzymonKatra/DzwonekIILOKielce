using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace KonfiguracjaDzwonekIILOKielce
{
    public static class BinaryWriterExt
    {
        public static void WriteMany(this BinaryWriter writer, byte value, int count)
        {
            while (count-- > 0) writer.Write(value);
        }
    }
}
