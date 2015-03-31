/*
 *     CVMob - Motion capture program
 *     Copyright (C) 2014, 2015  The CVMob contributors
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License along
 *     with this program; if not, write to the Free Software Foundation, Inc.,
 *     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef VIDEODATAVIEW_H
#define VIDEODATAVIEW_H

#include <QTreeView>

#include <QAction>
#include <QMenu>

class QAbstractProxyModel;

class VideoDataView : public QTreeView
{
    Q_OBJECT

public:
    explicit VideoDataView(QWidget* parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *e);

private:
    void copySelectionToClipboard();
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    QAbstractProxyModel *proxyModel();

    QMenu _contextMenu;
    QAction _deleteAction{
        style()->standardIcon(QStyle::SP_TrashIcon),
        tr("Delete"),
        this};
    QAction _copyAction{
        style()->standardIcon(QStyle::SP_FileIcon),
        tr("Copy data to clipboard"),
        this};
};

#endif // VIDEODATAVIEW_H
