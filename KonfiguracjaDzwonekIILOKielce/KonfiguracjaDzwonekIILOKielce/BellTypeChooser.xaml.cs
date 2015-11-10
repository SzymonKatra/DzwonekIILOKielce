using MVVMUtils;
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
    /// Interaction logic for BellTypeChooser.xaml
    /// </summary>
    public partial class BellTypeChooser : UserControl
    {
        public static readonly DependencyProperty IsForLessonProperty = DependencyProperty.Register("IsForLesson", typeof(bool), typeof(BellTypeChooser), new PropertyMetadata(IsForLessonChanged));
        public bool IsForLesson
        {
            get { return (bool)GetValue(IsForLessonProperty); }
            set { SetValue(IsForLessonProperty, value); }
        }

        public static readonly DependencyProperty IsForBreakProperty = DependencyProperty.Register("IsForBreak", typeof(bool), typeof(BellTypeChooser), new PropertyMetadata(IsForBreakChanged));
        public bool IsForBreak
        {
            get { return (bool)GetValue(IsForBreakProperty); }
            set { SetValue(IsForBreakProperty, value); }
        }

        public static readonly DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(BellType), typeof(BellTypeChooser), new PropertyMetadata(ValueChanged));
        public BellType Value
        {
            get { return (BellType)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        public BellTypeChooser()
        {
            InitializeComponent();
        }

        private static void IsForBreakChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            BellTypeChooser control = (BellTypeChooser)sender;
            control.IsForLesson = !control.IsForBreak;

            control.Value = (control.IsForLesson ? BellType.ForLesson : BellType.ForBreak);
        }
        private static void IsForLessonChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            BellTypeChooser control = (BellTypeChooser)sender;
            control.IsForBreak = !control.IsForLesson;

            control.Value = (control.IsForLesson ? BellType.ForLesson : BellType.ForBreak);
        }

        private static void ValueChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            BellTypeChooser control = (BellTypeChooser)sender;
            if (control.Value == BellType.ForLesson)
            {
                control.IsForLesson = true;
                control.IsForBreak = false;
            }
            else if (control.Value == BellType.ForBreak)
            {
                control.IsForLesson = false;
                control.IsForBreak = true;
            }
        }
    }
}
