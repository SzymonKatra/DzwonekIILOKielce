using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Globalization;

namespace KonfiguracjaDzwonekIILOKielce
{
    /// <summary>
    /// Interaction logic for DayOfWeekTitle.xaml
    /// </summary>
    public partial class DayOfWeekTitle : UserControl
    {
        public DayOfWeekTitle()
        {
            InitializeComponent();
            DayOfWeekChanged(this, new DependencyPropertyChangedEventArgs(DayOfWeekProperty, 0, 0));
        }

        public static readonly DependencyProperty TextProperty = DependencyProperty.Register("Text", typeof(string), typeof(DayOfWeekTitle));
        public string Text
        {
            get { return (string)GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }

        public static readonly DependencyProperty BrushProperty = DependencyProperty.Register("Brush", typeof(Brush), typeof(DayOfWeekTitle));
        public Brush Brush
        {
            get { return (Brush)GetValue(BrushProperty); }
            set { SetValue(BrushProperty, value); }
        }

        public static readonly DependencyProperty DayOfWeekProperty = DependencyProperty.Register("DayOfWeek", typeof(DayOfWeek), typeof(DayOfWeekTitle), new PropertyMetadata(DayOfWeekChanged));
        public DayOfWeek DayOfWeek
        {
            get { return (DayOfWeek)GetValue(DayOfWeekProperty); }
            set { SetValue(DayOfWeekProperty, value); }
        }

        private static void DayOfWeekChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            DayOfWeekTitle control = (DayOfWeekTitle)sender;
            control.Text = CultureInfo.CurrentCulture.DateTimeFormat.DayNames[(int)control.DayOfWeek];
            if (control.DayOfWeek == DayOfWeek.Saturday)
            {
                control.Brush = Brushes.Gray;
            }
            else if (control.DayOfWeek == DayOfWeek.Sunday)
            {
                control.Brush = Brushes.Red;
            }
            else
            {
                control.Brush = Brushes.Black;
            }
        }
    }
}
