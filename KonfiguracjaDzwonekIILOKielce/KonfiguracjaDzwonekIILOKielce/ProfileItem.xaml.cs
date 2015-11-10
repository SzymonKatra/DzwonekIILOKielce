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

namespace KonfiguracjaDzwonekIILOKielce
{
    /// <summary>
    /// Interaction logic for ProfileItem.xaml
    /// </summary>
    public partial class ProfileItem : UserControl
    {
        public ProfileItem()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty DayVMProperty = DependencyProperty.Register("DayVM", typeof(DayViewModel), typeof(ProfileItem), new PropertyMetadata(null, new PropertyChangedCallback(DateChanged)));
        public DayViewModel DayVM
        {
            get { return (DayViewModel)GetValue(DayVMProperty); }
            set { SetValue(DayVMProperty, value); }
        }

        public static readonly DependencyProperty DayBrushProperty = DependencyProperty.Register("DayBrush", typeof(Brush), typeof(ProfileItem), new PropertyMetadata(Brushes.Black));
        public Brush DayBrush
        {
            get { return (Brush)GetValue(DayBrushProperty); }
            set { SetValue(DayBrushProperty, value); }
        }
    
        private static void DateChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            ProfileItem control = (ProfileItem)sender;
            DateTime dt = new DateTime(control.DayVM.Year, control.DayVM.Month, control.DayVM.Day);
            if(dt.DayOfWeek == DayOfWeek.Saturday)
            {
                control.DayBrush = Brushes.Gray;
            }
            else if (dt.DayOfWeek == DayOfWeek.Sunday)
            {
                control.DayBrush = Brushes.Red;
            }
            else
            {
                control.DayBrush = Brushes.Black;
            }
        }
    }
}
