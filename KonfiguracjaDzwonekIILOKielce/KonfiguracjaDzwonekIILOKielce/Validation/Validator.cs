﻿using MVVMUtils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KonfiguracjaDzwonekIILOKielce.Validation
{
    public abstract class Validator<T> : ObservableObject, IDataErrorInfo, IValidator
    {
        protected struct ValidatorResult
        {
            public string Error;
            public bool IsValid
            {
                get { return string.IsNullOrEmpty(Error); }
            }
            public T ValidResult;

            public static ValidatorResult CreateValid(T result)
            {
                ValidatorResult x = new ValidatorResult();
                x.Error = string.Empty;
                x.ValidResult = result;
                return x;
            }
            public static ValidatorResult CreateInvalid(string error)
            {
                ValidatorResult x = new ValidatorResult();
                x.Error = error;
                return x;
            }
        }

        private bool m_isValid;
        public bool IsValid
        {
            get { return m_isValid; }
            private set
            {
                bool old = m_isValid;
                m_isValid = value;
                if (old != m_isValid) OnIsValidChanged();
                RaisePropertyChanged(nameof(IsValid));
            }
        }

        private string m_error;
        public string Error
        {
            get { return m_error; }
            private set
            {
                m_error = value;
                RaisePropertyChanged(nameof(Error));
            }
        }

        private T m_validValue;
        public T ValidValue
        {
            get { return m_validValue; }
            set
            {
                m_validValue = value;
                RaisePropertyChanged(nameof(ValidValue));
                m_input = ToValidForm();
                RaisePropertyChanged(nameof(Input));
                IsValid = true;
            }
        }

        private object m_input;
        public object Input
        {
            get { return m_input; }
            set
            {
                m_input = value;
                RaisePropertyChanged(nameof(Input));

                ValidatorResult result = Validate();

                bool oldIsValid = m_isValid;

                if (result.IsValid)
                {
                    Error = string.Empty;
                    T oldResult = m_validValue;
                    m_validValue = result.ValidResult;
                    OnValidValueChanged(new ValidatorValidValueChangedEventArgs<T>(oldResult, result.ValidResult));
                }
                else
                {
                    Error = result.Error;
                }

                IsValid = result.IsValid;
            }
        }

        public Validator()
        {
            ValidValue = default(T);
        }

        public event EventHandler IsValidChanged;
        public event EventHandler<ValidatorValidValueChangedEventArgs<T>> ValidValueChanged;

        protected abstract ValidatorResult Validate();
        protected virtual object ToValidForm()
        {
            if (ValidValue == null) return string.Empty;
            return ValidValue.ToString();
        }

        protected virtual void OnIsValidChanged()
        {
            EventHandler handler = IsValidChanged;
            if (handler != null) handler(this, EventArgs.Empty);
        }
        protected virtual void OnValidValueChanged(ValidatorValidValueChangedEventArgs<T> e)
        {
            EventHandler<ValidatorValidValueChangedEventArgs<T>> handler = ValidValueChanged;
            if (handler != null) handler(this, e);
        }

        public string this[string columnName]
        {
            get
            {
                if (columnName != nameof(Input)) return string.Empty;
                return Error;
            }
        }
    }
}
