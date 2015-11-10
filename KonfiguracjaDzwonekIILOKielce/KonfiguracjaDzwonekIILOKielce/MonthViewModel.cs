using MVVMUtils;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using System.Windows.Controls;
using System.Windows.Input;
using System.Diagnostics;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class MonthViewModel : ObservableObject
    {
        public MonthViewModel(int month, int year)
        {
            m_setProfilesCommand = new RelayCommand<int>(SetProfiles);
            m_selectionChangedCommand = new RelayCommand<SelectionChangedEventArgs>(SelectionChanged);

            m_month = month;
            m_year = year;

            DateTime dt = new DateTime(year, month, 1);
            int dummies = 0;
            switch(dt.DayOfWeek)
            {
                case DayOfWeek.Monday: dummies = 0; break;
                case DayOfWeek.Tuesday: dummies = 1; break;
                case DayOfWeek.Wednesday: dummies = 2; break;
                case DayOfWeek.Thursday: dummies = 3; break;
                case DayOfWeek.Friday: dummies = 4; break;
                case DayOfWeek.Saturday: dummies = 5; break;
                case DayOfWeek.Sunday: dummies = 6; break;
            }

            while(dummies-- > 0)
            {
                DayViewModel day = new DayViewModel(1, month, year);
                day.IsDummy = true;
                m_days.Add(day);
            }

            int daysInMonth = DateTime.DaysInMonth(year, month);
            for (int i = 1; i <= daysInMonth; i++)
            {
                m_days.Add(new DayViewModel(i, month, year));
            }
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

        public string Name
        {
            get
            {
                return CultureInfo.CurrentCulture.DateTimeFormat.GetMonthName(m_month) + " " + m_year;
            }
        }

        private ObservableCollection<DayViewModel> m_days = new ObservableCollection<DayViewModel>();
        public ObservableCollection<DayViewModel> Days
        {
            get { return m_days; }
        }

        private List<DayViewModel> m_selectedDays = new List<DayViewModel>();

        private RelayCommand<int> m_setProfilesCommand;
        public ICommand SetProfilesCommand
        {
            get { return m_setProfilesCommand; }
        }

        private RelayCommand<SelectionChangedEventArgs> m_selectionChangedCommand;
        public ICommand SelectionChangedCommand
        {
            get { return m_selectionChangedCommand; }
        }

        private void SetProfiles(int e)
        {
            foreach (var item in m_selectedDays) item.Profile = e;
        }
        
        private void SelectionChanged(SelectionChangedEventArgs e)
        {
            foreach (var item in e.RemovedItems)
            {
                DayViewModel day = (DayViewModel)item;
                if (day.IsDummy) continue;
                m_selectedDays.Remove(day);
            }

            foreach (var item in e.AddedItems)
            {
                DayViewModel day = (DayViewModel)item;
                if (day.IsDummy)
                {
                    // ugly hack
                    //ListView lv = e.Source as ListView;
                    //if (lv == null) continue;
                    //if (lv.SelectedItems.Contains(day)) lv.SelectedItems.Remove(day);

                    continue;
                }
                m_selectedDays.Add(day);
            }
        }
    }
}
