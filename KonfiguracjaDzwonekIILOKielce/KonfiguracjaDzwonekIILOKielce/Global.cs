using MVVMUtils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace KonfiguracjaDzwonekIILOKielce
{
    public static class Global
    {
        public static readonly string ConfigurationFileName = "dzwonek.zse";

        static Global()
        {
            s_dialogs = new DialogService();
        }

        private static DialogService s_dialogs;
        public static DialogService Dialogs
        {
            get { return s_dialogs; }
        }

        public static string GetStringResource(object key)
        {
            object result = Application.Current.TryFindResource(key);
            return (result == null ? key + " NOT FOUND - RESOURCE ERROR" : (string)result);
        }

        public static string GetProfileName(int id)
        {
            if (id == 0xFF) return Global.GetStringResource("StringProfileNone");
            return Global.GetStringResource("StringProfile" + id.ToString());
        }

        public static MessageBoxSuperButton MessageBox(object viewModel, string text, MessageBoxSuperPredefinedButtons buttons)
        {
            return MessageBoxSuper.ShowBox(Dialogs.GetWindow(viewModel), text, GetStringResource("StringAppName"), buttons);
        }
        public static MessageBoxSuperButton MessageBoxNoOwner(string text, MessageBoxSuperPredefinedButtons buttons)
        {
            return MessageBoxSuper.ShowBox(null, text, GetStringResource("StringAppName"), buttons);
        }
    }
}
