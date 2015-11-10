using MVVMUtils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class ObservableValue<T> : ObservableObject
    {
        public ObservableValue()
            : this(default(T))
        {
        }
        public ObservableValue(T value)
        {
            m_value = value;
        }

        private T m_value;
        public T Value
        {
            get { return m_value; }
            set { m_value = value; RaisePropertyChanged("Value"); }
        }
    }
}
