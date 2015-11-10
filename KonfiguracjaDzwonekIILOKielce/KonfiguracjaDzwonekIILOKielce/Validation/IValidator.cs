﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KonfiguracjaDzwonekIILOKielce.Validation
{
    public interface IValidator
    {
        bool IsValid { get; }
        string Error { get; }
        event EventHandler IsValidChanged;
    }
}
