using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class PacketReader : BinaryReader
    {
        private MemoryStream m_stream;

        public PacketReader(byte[] data)
            : this(new MemoryStream(data))
        {
        }
        private PacketReader(MemoryStream stream)
            : base(stream)
        {
            m_stream = stream;
        }
    }
}
