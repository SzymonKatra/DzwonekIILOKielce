using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;

namespace KonfiguracjaDzwonekIILOKielce
{
    public static class Device
    {
        public const ushort VALID_CHECKSUM = 0x14D4;
        public const ushort PROFILE_BELLS_COUNT = 40;

        public const byte DAYS_COUNT = 32;
        public const byte PROFILES_COUNT = 4;
        public const byte MONTHS_COUNT = 12;
        public const byte RESERVED_VALUE = 0xFF;

        public const int METADATA_RESERVED = 0x6;
        public const int SETTINGS_RESERVED = 0x38;
        public const int PROFILE_RESERVED = 0x17;
        public const int MONTH_RESERVED = 0x8;
        public const int EEPROM_RESERVED = 0x18;

        [StructLayout(LayoutKind.Sequential)]
        public struct EEPROM
        {
            public Metadata Metadata;
            public Settings Settings;
            public Profile[] Profiles;
            public Month[] Months;

            public static EEPROM Create()
            {
                EEPROM result = new EEPROM();
                result.Metadata = Metadata.Create();
                result.Settings = Settings.Create();
                result.Profiles = new Profile[PROFILES_COUNT];
                for (int i = 0; i < PROFILES_COUNT; i++) result.Profiles[i] = Profile.Create();
                result.Months = new Month[MONTHS_COUNT];
                for (int i = 0; i < MONTHS_COUNT; i++) result.Months[i] = Month.Create();
                return result;
            }

            public void Pack(Stream stream)
            {
                this.Metadata.Pack(stream);
                this.Settings.Pack(stream);
                for (int i = 0; i < this.Profiles.Length; i++) this.Profiles[i].Pack(stream);
                for (int i = 0; i < this.Months.Length; i++) this.Months[i].Pack(stream);

                using (BinaryWriter writer = new BinaryWriter(stream, Encoding.UTF8, true))
                {
                    writer.WriteMany(RESERVED_VALUE, EEPROM_RESERVED);
                }
            }

            public void Unpack(Stream stream)
            {
                this.Metadata.Unpack(stream);
                this.Settings.Unpack(stream);
                for (int i = 0; i < this.Profiles.Length; i++) this.Profiles[i].Unpack(stream);
                for (int i = 0; i < this.Months.Length; i++) this.Months[i].Unpack(stream);

                stream.Seek(EEPROM_RESERVED, SeekOrigin.Current);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Metadata
        {
            public ushort Checksum;

            public static Metadata Create()
            {
                Metadata result = new Metadata();
                result.Checksum = VALID_CHECKSUM;
                return result;
            }

            public void Pack(Stream stream)
            {
                using (BinaryWriter writer = new BinaryWriter(stream, Encoding.UTF8, true))
                {
                    writer.Write(this.Checksum);
                    writer.WriteMany(RESERVED_VALUE, METADATA_RESERVED);
                }
            }

            public void Unpack(Stream stream)
            {
                using (BinaryReader reader = new BinaryReader(stream, Encoding.UTF8, true))
                {
                    this.Checksum = reader.ReadUInt16();
                }

                stream.Seek(METADATA_RESERVED, SeekOrigin.Current);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Settings
        {
            public byte BellLengthForLesson;
            public byte BellLengthForBreak;
            public byte SchoolYear;
            public byte AutoTimeTransition;
            public ushort Dcf77SynchronizationTime;
            public ushort Dcf77SynchronizationMaxLength;

            public static Settings Create()
            {
                Settings result = new Settings();
                return result;
            }

            public void Pack(Stream stream)
            {
                using (BinaryWriter writer = new BinaryWriter(stream, Encoding.UTF8, true))
                {
                    writer.Write(this.BellLengthForLesson);
                    writer.Write(this.BellLengthForBreak);
                    writer.Write(this.SchoolYear);
                    writer.Write(this.AutoTimeTransition);
                    writer.Write(this.Dcf77SynchronizationTime);
                    writer.Write(this.Dcf77SynchronizationMaxLength);
                    writer.WriteMany(RESERVED_VALUE, SETTINGS_RESERVED);
                }
            }

            public void Unpack(Stream stream)
            {
                using (BinaryReader reader = new BinaryReader(stream, Encoding.UTF8, true))
                {
                    this.BellLengthForLesson = reader.ReadByte();
                    this.BellLengthForBreak = reader.ReadByte();
                    this.SchoolYear = reader.ReadByte();
                    this.AutoTimeTransition = reader.ReadByte();
                    this.Dcf77SynchronizationTime = reader.ReadUInt16();
                    this.Dcf77SynchronizationMaxLength = reader.ReadUInt16();
                }

                stream.Seek(SETTINGS_RESERVED, SeekOrigin.Current);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Profile
        {
            public byte Count;
            public ushort[] Bells;
            public ulong BellTypes;

            public static Profile Create()
            {
                Profile result = new Profile();
                result.Bells = new ushort[PROFILE_BELLS_COUNT];
                return result;
            }

            public void Pack(Stream stream)
            {
                using (BinaryWriter writer = new BinaryWriter(stream, Encoding.UTF8, true))
                {
                    writer.Write(this.Count);
                    for (int i = 0; i < this.Bells.Length; i++) writer.Write(this.Bells[i]);
                    writer.Write(this.BellTypes);
                    writer.WriteMany(RESERVED_VALUE, PROFILE_RESERVED);
                }
            }

            public void Unpack(Stream stream)
            {
                using (BinaryReader reader = new BinaryReader(stream, Encoding.UTF8, true))
                {
                    this.Count = reader.ReadByte();
                    for (int i = 0; i < this.Bells.Length; i++) this.Bells[i] = reader.ReadUInt16();
                    this.BellTypes = reader.ReadUInt64();
                }

                stream.Seek(PROFILE_RESERVED, SeekOrigin.Current);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Month
        {
            public byte[] Days;

            public static Month Create()
            {
                Month result = new Month();
                result.Days = new byte[DAYS_COUNT];
                return result;
            }

            public void Pack(Stream stream)
            {
                using (BinaryWriter writer = new BinaryWriter(stream, Encoding.UTF8, true))
                {
                    for (int i = 0; i < this.Days.Length; i++) writer.Write(this.Days[i]);
                    writer.WriteMany(RESERVED_VALUE, MONTH_RESERVED);
                }
            }

            public void Unpack(Stream stream)
            {
                using (BinaryReader reader = new BinaryReader(stream, Encoding.UTF8, true))
                {
                    for (int i = 0; i < this.Days.Length; i++) this.Days[i] = reader.ReadByte();
                }

                stream.Seek(MONTH_RESERVED, SeekOrigin.Current);
            }
        }
    }
}