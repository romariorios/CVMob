/*
 *     CVMob - Motion capture program
 *     Copyright (C) 2014  The CVMob contributors
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

#include "videodataview.hpp"

#include <QAbstractProxyModel>
#include <QtGui/QContextMenuEvent>
#include <QMenu>
#include <model/videomodel.hpp>

#include <QDebug>

void VideoDataView::contextMenuEvent(QContextMenuEvent* e)
{
    auto proxyModel = dynamic_cast<QAbstractProxyModel *>(model());
    
    if (!proxyModel) {
        return;
    }
    
    auto index = indexAt(e->pos());
    auto sourceIndex = proxyModel->mapToSource(index);
    auto path = VideoModel::indexPath(sourceIndex);
    
    if (path.isEmpty()) {
        return;
    }
    
    auto contextMenu = new QMenu { this };
    connect(contextMenu, &QMenu::aboutToHide, contextMenu, &QObject::deleteLater);
    
    auto deleteAction = new QAction {
        style()->standardIcon(QStyle::SP_TrashIcon),
        tr("Delete"),
        contextMenu
    };
    connect(deleteAction, &QAction::triggered, [=]()
    {
        model()->removeRow(index.row(), index.parent());
    });
    
    if (path.size() == 2 &&
        (path.at(0).column == VideoModel::DistancesColumn ||
         path.at(0).column == VideoModel::TrajectoriesColumn ||
         path.at(0).column == VideoModel::AnglesColumn
        )) {
        contextMenu->addAction(deleteAction);
    }
    
    if (contextMenu->actions().isEmpty()) {
        return;
    }
    
    contextMenu->popup(e->globalPos());
}

#include "videodataview.moc"
