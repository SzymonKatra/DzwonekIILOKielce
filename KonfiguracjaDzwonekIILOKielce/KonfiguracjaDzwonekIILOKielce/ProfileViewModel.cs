using KonfiguracjaDzwonekIILOKielce.Validation;
using MVVMUtils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class ProfileViewModel : ObservableObject
    {
        public ProfileViewModel(int id)
        {
            m_addBellCommand = new RelayCommand(AddBell, CanAddBell);
            m_clearBellsCommand = new RelayCommand(ClearBells);

            Bells.ItemPropertyInCollectionChanged += Bells_ItemPropertyInCollectionChanged;

            m_id = id;
        }

        private int m_id;
        public int Id
        {
            get { return m_id; }
        }

        public string Name
        {
            get { return Global.GetProfileName(m_id); }
        }

        private PerItemObservableCollection<BellViewModel> m_bells = new PerItemObservableCollection<BellViewModel>();
        public PerItemObservableCollection<BellViewModel> Bells
        {
            get { return m_bells; }
        }

        private RelayCommand m_addBellCommand;
        public ICommand AddBellCommand
        {
            get { return m_addBellCommand; }
        }

        private RelayCommand m_clearBellsCommand;
        public ICommand ClearBellsCommand
        {
            get { return m_clearBellsCommand; }
        }

        private void AddBell(object e)
        {
            if (m_bells.Count > 0)
            {
                BellViewModel last = m_bells[m_bells.Count - 1];
                last.Max = last.Value;
                BellViewModel bell = new BellViewModel();
                bell.Value = last.Value.AddMinutes(1);
                bell.Min = bell.Value;
                bell.Max = DateTime.MinValue.AddHours(23).AddMinutes(59);
                bell.Type = m_bells[m_bells.Count - 1].Type == BellType.ForBreak ? BellType.ForLesson : BellType.ForBreak;
                bell.Number = m_bells[m_bells.Count - 1].Number + 1;

                m_bells.Add(bell);
            }
            else
            {
                BellViewModel bell = new BellViewModel();
                bell.Value = DateTime.MinValue;
                bell.Min = bell.Value;
                bell.Max = DateTime.MinValue.AddHours(23).AddMinutes(59);
                bell.Type = BellType.ForLesson;
                bell.Number = 1;

                m_bells.Add(bell);
            }

            m_addBellCommand.RaiseCanExecuteChanged();
        }
        private bool CanAddBell(object e)
        {
            if (m_bells.Count > 0)
            {
                BellViewModel last = m_bells[m_bells.Count - 1];
                if (last.Value.Hour >= 23 && last.Value.Minute >= 59) return false;
            }

            return m_bells.Count < Device.PROFILE_BELLS_COUNT;
        }

        private void ClearBells(object e)
        {
            m_bells.Clear();
            m_addBellCommand.RaiseCanExecuteChanged();
        }

        private void Bells_ItemPropertyInCollectionChanged(object sender, ItemPropertyInCollectionChangedEventArgs<BellViewModel> e)
        {
            if (e.PropertyName != "Value") return;

            int index = m_bells.IndexOf(e.Item);
            if (index > 0)
            {
                m_bells[index - 1].Max = e.Item.Value.AddMinutes(-1);
            }
            if (index + 1 < m_bells.Count)
            {
                m_bells[index + 1].Min = e.Item.Value.AddMinutes(1);
            }

            m_addBellCommand.RaiseCanExecuteChanged();
        }
    }
}
