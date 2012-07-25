#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QItemDelegate>

class ItemDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  explicit ItemDelegate(QObject *parent = 0);

  virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    return QSize(QItemDelegate::sizeHint(option, index).width() * 2, 10);
  }
  
signals:
  
public slots:
  
};

#endif // ITEMDELEGATE_H