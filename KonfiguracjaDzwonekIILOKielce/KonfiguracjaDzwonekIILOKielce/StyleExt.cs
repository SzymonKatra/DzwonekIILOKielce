using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace KonfiguracjaDzwonekIILOKielce
{
    public static class StyleExt
    {
        public static void Merge(this Style style, Style other)
        {
            if (style == null) throw new ArgumentNullException("style");
            if (other == null) throw new ArgumentNullException("other");

            if (other.BasedOn != null) style.Merge(other.BasedOn);

            foreach (var setter in other.Setters) style.Setters.Add(setter);
            foreach (var trigger in other.Triggers) style.Triggers.Add(trigger);
            foreach (var key in other.Resources.Keys) style.Resources[key] = other.Resources[key];
        }
    }
}
