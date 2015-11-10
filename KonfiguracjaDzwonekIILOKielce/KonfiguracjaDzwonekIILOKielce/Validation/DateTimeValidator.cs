using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KonfiguracjaDzwonekIILOKielce.Validation
{
    public class DateTimeValidator : Validator<DateTime>
    {
        private DateTime m_min;
        public DateTime Min
        {
            get { return m_min; }
            set { m_min = value; }
        }

        private DateTime m_max;
        public DateTime Max
        {
            get { return m_max; }
            set { m_max = value; }
        }

        protected override ValidatorResult Validate()
        {
            throw new NotImplementedException();
        }
        protected override object ToValidForm()
        {
            return ValidValue;
        }
    }
}
