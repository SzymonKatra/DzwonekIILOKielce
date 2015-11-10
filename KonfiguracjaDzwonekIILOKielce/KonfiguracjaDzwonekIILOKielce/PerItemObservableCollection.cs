using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KonfiguracjaDzwonekIILOKielce
{
    public class ItemPropertyInCollectionChangedEventArgs<T> : PropertyChangedEventArgs
                                                                where T : INotifyPropertyChanged
    {
        public ItemPropertyInCollectionChangedEventArgs(T item, PropertyChangedEventArgs e)
            : this(item, e.PropertyName)
        {
        }
        public ItemPropertyInCollectionChangedEventArgs(T item, string propertyName)
            : base(propertyName)
        {
            m_item = item;
        }

        private T m_item;
        public T Item
        {
            get { return m_item; }
            set { m_item = value; }
        }
    }

    /// <summary>
    /// Collection which can notify about changed propety of item in this collection via event in class
    /// </summary>
    /// <typeparam name="T">Type</typeparam>
    public class PerItemObservableCollection<T> : ObservableCollection<T>
                                                  where T : INotifyPropertyChanged
    {
        public event EventHandler<ItemPropertyInCollectionChangedEventArgs<T>> ItemPropertyInCollectionChanged;

        protected override void ClearItems()
        {
            foreach (T item in this) item.PropertyChanged -= item_PropertyChanged;
            base.ClearItems();
        }
        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            if (e.Action != NotifyCollectionChangedAction.Move)
            {
                if (e.OldItems != null)
                {
                    foreach (T item in e.OldItems) item.PropertyChanged -= item_PropertyChanged;
                }

                if (e.NewItems != null)
                {
                    foreach (T item in e.NewItems) item.PropertyChanged += item_PropertyChanged;
                }
            }

            base.OnCollectionChanged(e);
        }

        private void item_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            OnItemPropertyInCollectionChanged(new ItemPropertyInCollectionChangedEventArgs<T>((T)sender, e));
        }

        protected virtual void OnItemPropertyInCollectionChanged(ItemPropertyInCollectionChangedEventArgs<T> e)
        {
            EventHandler<ItemPropertyInCollectionChangedEventArgs<T>> handler = ItemPropertyInCollectionChanged;
            if (handler != null) handler(this, e);
        }
    }
}
