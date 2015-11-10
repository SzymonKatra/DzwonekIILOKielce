using MVVMUtils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class DayViewModel : ObservableObject
    {
        public DayViewModel(int day, int month, int year)
        {
            m_day = day;
            m_month = month;
            m_year = year;

            DateTime dateTime = new DateTime(year, month, day);

            m_profile = (dateTime.DayOfWeek == DayOfWeek.Saturday || dateTime.DayOfWeek == DayOfWeek.Sunday ? 0xFF : 0);
        }

        private int m_day;
        public int Day
        {
            get { return m_day; }
        }

        private int m_month;
        public int Month
        {
            get { return m_month; }
        }

        private int m_year;
        public int Year
        {
            get { return m_year; }
        }

        private int m_profile;
        public int Profile
        {
            get { return m_profile; }
            set { m_profile = value; RaisePropertyChanged(nameof(Profile)); }
        }

        private bool m_isDummy = false;
        public bool IsDummy
        {
            get { return m_isDummy; }
            set { m_isDummy = value; RaisePropertyChanged(nameof(IsDummy)); RaisePropertyChanged(nameof(IsNotDummy)); }
        }

        public bool IsNotDummy
        {
            get { return !m_isDummy; }
            set { IsDummy = !value; }
        }
    }
}
