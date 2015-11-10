using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Markup;

namespace KonfiguracjaDzwonekIILOKielce
{
    [MarkupExtensionReturnType(typeof(Style))]
    public class MultiStyleExtension : MarkupExtension
    {
        private string[] m_stylesKeys;

        public MultiStyleExtension(string inputStyles)
        {
            if (inputStyles == null) throw new ArgumentException("inputStyles is null");
            m_stylesKeys = inputStyles.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
            if (m_stylesKeys.Length == 0) throw new ArgumentException("No styles specified");
        }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            Style result = new Style();

            foreach (var styleKey in m_stylesKeys)
            {
                Style style = (Style)new StaticResourceExtension(styleKey).ProvideValue(serviceProvider);

                if (style == null) new ArgumentException(styleKey + " not found");

                result.Merge(style);
            }

            return result;
        }
    }
}
