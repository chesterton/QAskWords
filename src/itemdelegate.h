/***************************************************************************
 *   Copyright (C) 2012 by Miguel Escudero Gestal
 *   miguelescudero@users.sourceforge.net
 *
 *   This file is part of QAskWords
 *
 *   QAskWords is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAskWords is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

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