using System;
using System.Collections.Generic;
using System.Linq;
using System.Collections.ObjectModel;
using System.Text;
using System.Threading.Tasks;
using MVVMUtils;
using KonfiguracjaDzwonekIILOKielce.Validation;
using System.Windows;
using Xceed.Wpf.Toolkit;
using System.IO;
using System.Reflection;
using System.Windows.Input;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class MainViewModel : ObservableObject
    {
        public MainViewModel()
        {
            m_writeCommand = new RelayCommand(Write);
            m_readCommand = new RelayCommand(Read);
            m_restoreDefaultCommand = new RelayCommand(RestoreDefault);

            m_validators = new ValidatorCollection(null);
            m_profiles = new ObservableCollection<ProfileViewModel>();
            m_months = new ObservableCollection<MonthViewModel>();

            Initialize();
        }

        private ValidatorCollection m_validators;
        private string m_configurationPath;
        public string ConfigurationPath
        {
            get { return m_configurationPath; }
        }

        private int m_bellLengthForLesson;
        public int BellLengthForLesson
        {
            get { return m_bellLengthForLesson; }
            set { m_bellLengthForLesson = value;  RaisePropertyChanged(nameof(BellLengthForLesson)); }
        }

        private int m_bellLengthForBreak;
        public int BellLengthForBreak
        {
            get { return m_bellLengthForBreak; }
            set { m_bellLengthForBreak = value; RaisePropertyChanged(nameof(BellLengthForBreak)); }
        }

        private int m_schoolYear;
        public int SchoolYear
        {
            get { return m_schoolYear; }
            set
            {
                MessageBoxSuperButton mboxResult = Global.MessageBox(this, "Zmiana roku szkolnego spowoduje wyczyszczenie kalendarza" + Environment.NewLine + "Czy chcesz kontynuować?", MessageBoxSuperPredefinedButtons.YesNo);
                if (mboxResult != MessageBoxSuperButton.Yes) return;

                m_schoolYear = value;
                RaisePropertyChanged(nameof(SchoolYear));
                RaisePropertyChanged(nameof(SchoolYearSecond));

                RebuildYear();
            }
        }

        public int SchoolYearSecond
        {
            get { return m_schoolYear + 1; }
        }

        private DateTime m_synchronizationTime;
        public DateTime SynchronizationTime
        {
            get { return m_synchronizationTime; }
            set { m_synchronizationTime = value; RaisePropertyChanged(nameof(SynchronizationTime)); }
        }

        private int m_synchronizationLength;
        public int SynchronizationLength
        {
            get { return m_synchronizationLength; }
            set { m_synchronizationLength = value; RaisePropertyChanged(nameof(SynchronizationLength)); }
        }

        private bool m_autoTimeTransition;
        public bool AutoTimeTransition
        {
            get { return m_autoTimeTransition; }
            set { m_autoTimeTransition = value; RaisePropertyChanged(nameof(AutoTimeTransition)); }
        }

        private int m_alarmLength;
        public int AlarmLength
        {
            get { return m_alarmLength; }
            set { m_alarmLength = value;  RaisePropertyChanged(nameof(AlarmLength)); }
        }

        private int m_alarmDiscontinuousInterval;
        public int AlarmDiscontinuousInterval
        {
            get { return m_alarmDiscontinuousInterval; }
            set { m_alarmDiscontinuousInterval = value; RaisePropertyChanged(nameof(AlarmDiscontinuousInterval)); }
        }

        private ObservableCollection<ProfileViewModel> m_profiles;
        public ObservableCollection<ProfileViewModel> Profiles
        {
            get { return m_profiles; }
        }

        private ObservableCollection<MonthViewModel> m_months;
        public ObservableCollection<MonthViewModel> Months
        {
            get { return m_months; }
        }

        private RelayCommand m_writeCommand;
        public ICommand WriteCommand
        {
            get { return m_writeCommand; }
        }
        private RelayCommand m_readCommand;
        public ICommand ReadCommand
        {
            get { return m_readCommand; }
        }
        private RelayCommand m_restoreDefaultCommand;
        public ICommand RestoreDefaultCommand
        {
            get { return m_restoreDefaultCommand; }
        }

        public void Initialize()
        {
            m_configurationPath = string.Empty;

            string appRoot = Directory.GetDirectoryRoot(Assembly.GetExecutingAssembly().Location);

            string path = Path.Combine(appRoot, Global.ConfigurationFileName);
            if (File.Exists(path))
            {
                m_configurationPath = path;
            }
            else
            {
                string[] roots = Directory.GetLogicalDrives();
                foreach (var item in roots)
                {
                    path = Path.Combine(item, Global.ConfigurationFileName);
                    if (File.Exists(path))
                    {
                        m_configurationPath = path;
                    }
                }
            }

            if (string.IsNullOrEmpty(m_configurationPath))
            {
                m_configurationPath = Path.Combine(appRoot, Global.ConfigurationFileName);
                m_restoreDefaultCommand.Execute(new object());
            }
            else
            {
                m_readCommand.Execute(null);
            }
        }

        private void RebuildYear()
        {
            m_months.Clear();
            for (int i = 9; i <= 12; i++) m_months.Add(new MonthViewModel(i, m_schoolYear));
            for (int i = 1; i < 9; i++) m_months.Add(new MonthViewModel(i, m_schoolYear + 1));
        }

        private void RestoreDefault(object e)
        {
            if (e == null)
            {
                if (Global.MessageBox(this, Global.GetStringResource("StringDoYouReallyWantDefault"), MessageBoxSuperPredefinedButtons.YesNo) != MessageBoxSuperButton.Yes) return;
            }

            m_bellLengthForLesson = 10;
            m_bellLengthForBreak = 5;
            m_schoolYear = DateTime.Now.Year;
            m_synchronizationTime = new DateTime(1, 1, 1, 22, 0, 0); // 22:00
            m_synchronizationLength = 420; // 7 hours
            m_autoTimeTransition = true;
            m_alarmLength = 180; // 3 minutes;
            m_alarmDiscontinuousInterval = 1;

            m_profiles.Clear();
            for (int i = 0; i < Device.PROFILES_COUNT; i++) m_profiles.Add(new ProfileViewModel(i));

            RebuildYear();
        }
        private void Read(object e)
        {
            try
            {
                byte[] content = File.ReadAllBytes(m_configurationPath);
                Device.EEPROM eeprom = Device.EEPROM.Create();
                MemoryStream stream = new MemoryStream(content);
                eeprom.Unpack(stream);
                stream.Close();

                if (eeprom.Metadata.Checksum != Device.VALID_CHECKSUM) Global.MessageBoxNoOwner(string.Format(Global.GetStringResource("StringFileIsInvalid"), m_configurationPath), MessageBoxSuperPredefinedButtons.OK);

                m_bellLengthForLesson = eeprom.Settings.BellLengthForLesson;
                m_bellLengthForBreak = eeprom.Settings.BellLengthForBreak;
                m_schoolYear = (int)eeprom.Settings.SchoolYear + 1900;
                m_synchronizationTime = new DateTime(1, 1, 1, eeprom.Settings.Dcf77SynchronizationTime / 60, eeprom.Settings.Dcf77SynchronizationTime % 60, 0);
                m_synchronizationLength = eeprom.Settings.Dcf77SynchronizationMaxLength;
                m_autoTimeTransition = eeprom.Settings.AutoTimeTransition > 0 ? true : false;
                m_alarmLength = eeprom.Settings.AlarmLength;
                m_alarmDiscontinuousInterval = eeprom.Settings.AlarmDiscontinuousInterval;

                m_profiles.Clear();
                for (int i = 0; i < Device.PROFILES_COUNT; i++)
                {
                    Device.Profile eepromProfile = eeprom.Profiles[i];
                    ProfileViewModel profile = new ProfileViewModel(i);
                    m_profiles.Add(profile);
                    for (int bell = 0; bell < eepromProfile.Count; bell++)
                    {
                        profile.AddBellCommand.Execute(null);
                        profile.Bells[bell].Value = new DateTime(1, 1, 1, eepromProfile.Bells[bell] / 60, eepromProfile.Bells[bell] % 60, 0);
                        profile.Bells[bell].Type = (((ulong)1 << bell) & eepromProfile.BellTypes) > 0 ? BellType.ForBreak : BellType.ForLesson;
                    }
                }

                m_months.Clear();
                for (int i = 9; i <= 12; i++) m_months.Add(new MonthViewModel(i, m_schoolYear));
                for (int i = 1; i < 9; i++) m_months.Add(new MonthViewModel(i, m_schoolYear + 1));
                for (int i = 0; i < Device.MONTHS_COUNT; i++)
                {
                    MonthViewModel month = m_months[i];
                    Device.Month eepromMonth = eeprom.Months[month.Month - 1];
                    foreach (DayViewModel day in month.Days)
                    {
                        if (day.IsDummy) continue;
                        if (day.Day >= eepromMonth.Days.Length) continue;
                        day.Profile = eepromMonth.Days[day.Day];
                    }
                }
            }
            catch
            {
                Global.MessageBoxNoOwner(string.Format(Global.GetStringResource("StringFormatErrorWhileReading"), m_configurationPath), MessageBoxSuperPredefinedButtons.OK);
                Application.Current.Shutdown(1);
            }
        }
        private void Write(object e)
        {
            try
            {
                Device.EEPROM eeprom = Device.EEPROM.Create();
                eeprom.Metadata.Checksum = Device.VALID_CHECKSUM;
                eeprom.Settings.BellLengthForLesson = (byte)m_bellLengthForLesson;
                eeprom.Settings.BellLengthForBreak = (byte)m_bellLengthForBreak;
                eeprom.Settings.SchoolYear = (byte)(m_schoolYear - 1900);
                eeprom.Settings.Dcf77SynchronizationTime = (ushort)(m_synchronizationTime.Hour * 60 + m_synchronizationTime.Minute);
                eeprom.Settings.Dcf77SynchronizationMaxLength = (ushort)m_synchronizationLength;
                eeprom.Settings.AutoTimeTransition = (byte)(m_autoTimeTransition ? 1 : 0);
                eeprom.Settings.AlarmLength = (ushort)m_alarmLength;
                eeprom.Settings.AlarmDiscontinuousInterval = (byte)m_alarmDiscontinuousInterval;

                for (int i = 0; i < Device.PROFILES_COUNT; i++)
                {
                    ProfileViewModel profile = m_profiles[i];
                    Device.Profile eepromProfile = eeprom.Profiles[profile.Id];

                    eepromProfile.Count = (byte)profile.Bells.Count;
                    ulong types = 0;
                    for (int bell = 0; bell < profile.Bells.Count; bell++)
                    {
                        BellViewModel bellVm = profile.Bells[bell];
                        eepromProfile.Bells[bell] = (ushort)(bellVm.Value.Hour * 60 + bellVm.Value.Minute);
                        if (bellVm.Type == BellType.ForBreak) types |= (ulong)1 << bell;
                    }
                    eepromProfile.BellTypes = types;

                    eeprom.Profiles[profile.Id] = eepromProfile;
                }

                for (int i = 0; i < Device.MONTHS_COUNT; i++)
                {
                    MonthViewModel month = m_months[i];
                    Device.Month eepromMonth = eeprom.Months[month.Month - 1];

                    foreach (DayViewModel day in month.Days)
                    {
                        if (day.IsDummy) continue;
                        if (day.Day >= eepromMonth.Days.Length) continue;
                        eepromMonth.Days[day.Day] = (byte)day.Profile;
                    }

                    eeprom.Months[month.Month - 1] = eepromMonth;
                }

                MemoryStream stream = new MemoryStream();
                eeprom.Pack(stream);
                stream.Close();
                byte[] content = stream.ToArray();

                File.WriteAllBytes(m_configurationPath, content);

                Global.MessageBox(this, Global.GetStringResource("StringSaved"), MessageBoxSuperPredefinedButtons.OK);
            }
            catch
            {
                Global.MessageBox(this, string.Format(Global.GetStringResource("StringFormatErrorWhileWriting"), m_configurationPath), MessageBoxSuperPredefinedButtons.OK);
            }
        }
    }
}
