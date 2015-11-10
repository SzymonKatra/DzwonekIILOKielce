using MVVMUtils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class BellViewModel : ObservableObject
    {
        private DateTime m_min;
        public DateTime Min
        {
            get { return m_min; }
            set { m_min = value; RaisePropertyChanged(nameof(Min)); }
        }

        private DateTime m_max;
        public DateTime Max
        {
            get { return m_max; }
            set { m_max = value; RaisePropertyChanged(nameof(Max)); }
        }

        private DateTime m_value;
        public DateTime Value
        {
            get { return m_value; }
            set { m_value = value; RaisePropertyChanged(nameof(Value)); }
        }

        private BellType m_type;
        public BellType Type
        {
            get { return m_type; }
            set { m_type = value; RaisePropertyChanged(nameof(Type)); }
        }

        private int m_number;
        public int Number
        {
            get { return m_number; }
            set { m_number = value; RaisePropertyChanged(nameof(Number)); }
        }
    }
}
