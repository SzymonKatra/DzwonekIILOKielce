using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class PacketWriter : BinaryWriter
    {
        private MemoryStream m_stream;

        public byte[] Data
        {
            get { return m_stream.ToArray(); }
        }

        public PacketWriter()
            : this(new MemoryStream())
        {
        }
        private PacketWriter(MemoryStream stream)
            : base(stream)
        {
            m_stream = stream;
        }

        public void WriteMany(byte value, int count)
        {
            while (count-- > 0) this.Write(value);
        }
    }
}
