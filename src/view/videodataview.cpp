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
#include <QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QContextMenuEvent>
#include <QMenu>
#include <model/videomodel.hpp>

VideoDataView::VideoDataView(QWidget* parent) :
    QTreeView { parent },
    _contextMenu { new QMenu { this } }
{
    // Delete
    _deleteAction = new QAction {
        style()->standardIcon(QStyle::SP_TrashIcon),
        tr("Delete"),
        this
    };
    
    _deleteAction->setShortcut(tr("Delete"));
    
    connect(_deleteAction, &QAction::triggered, [=]()
    {
        for (auto index : selectionModel()->selectedIndexes()) {
            model()->removeRow(index.row(), index.parent());
        }
    });
    
    addAction(_deleteAction);
    _contextMenu->addAction(_deleteAction);
    
    // Copy
    _copyAction = new QAction {
        style()->standardIcon(QStyle::SP_FileIcon),
        tr("Copy data to clipboard"),
        this
    };
    
    _copyAction->setShortcut(tr("Ctrl+C"));
    
    connect(_copyAction, &QAction::triggered, this, &VideoDataView::copySelectionToClipboard);
    
    addAction(_copyAction);
    _contextMenu->addAction(_copyAction);
}

void VideoDataView::contextMenuEvent(QContextMenuEvent* e)
{
    if (!proxyModel()) {
        return;
    }
    
    auto index = indexAt(e->pos());
    auto sourceIndex = proxyModel()->mapToSource(index);
    auto path = VideoModel::indexPath(sourceIndex);
    
    if (path.isEmpty()) {
        return;
    }
    
    // Delete
    QString deleteString;
    switch (path.at(0).column) {
    case VideoModel::DistancesColumn:
        deleteString = tr("Delete distance");
        break;
    case VideoModel::TrajectoriesColumn:
        deleteString = tr("Delete trajectory");
        break;
    case VideoModel::AnglesColumn:
        deleteString = tr("Delete angle");
        break;
    default:
        deleteString = tr("Delete");
    }
    _deleteAction->setText(deleteString);
    
    _contextMenu->popup(e->globalPos());
}

void VideoDataView::copySelectionToClipboard()
{
    for (auto index : selectionModel()->selectedRows()) {
        auto path = VideoModel::indexPath(proxyModel()->mapToSource(index));
        QString clipboardText;
        
        if (path.size() == 2 && path.at(0).column == VideoModel::TrajectoriesColumn) {
            clipboardText += index.data().toString() + "\n";
            clipboardText +=
                tr("Frame") + "\t" +
                tr("Position (x)") + "\t" +
                tr("Position (y)") + "\t" +
                tr("Speed (abs)") + "\t" +
                tr("Speed (x)") + "\t" +
                tr("Speed (y)") + "\t" +
                tr("Acceleration (abs)") + "\t" +
                tr("Acceleration (x)") + "\t" +
                tr("Acceleration (y)") + "\n";
            
            for (int i = 0; i < model()->rowCount(index); ++i) {
                auto speed = model()->index(
                    i, VideoModel::LSpeedColumn, index
                ).data(VideoModel::SourceDataRole).toPointF();
                auto absSpeed = sqrt(speed.x() * speed.x() + speed.y() * speed.y());
                auto acceleration =
                    model()->index(
                        i, VideoModel::LAccelerationColumn, index
                    ).data(VideoModel::SourceDataRole).toPointF();
                auto absAcceleration = sqrt(
                    acceleration.x() * acceleration.x() +
                    acceleration.y() * acceleration.y()
                );
                
                clipboardText +=
                    QString::number(
                        model()->index(
                            i, VideoModel::LFrameColumn, index
                        ).data(VideoModel::SourceDataRole).toInt()
                    ) + "\t" + QString::number(
                        model()->index(
                            i, VideoModel::PositionColumn, index
                        ).data(VideoModel::SourceDataRole).toPointF().x()
                    ) + "\t" + QString::number(
                        model()->index(
                            i, VideoModel::PositionColumn, index
                        ).data(VideoModel::SourceDataRole).toPointF().y()
                    ) + "\t" + QString::number(
                        absSpeed
                    ) + "\t" + QString::number(
                        speed.x()
                    ) + "\t" + QString::number(
                        speed.y()
                    ) + "\t" + QString::number(
                        absAcceleration
                    ) + "\t" + QString::number(
                        acceleration.x()
                    ) + "\t" + QString::number(
                        acceleration.y()
                    ) + "\n";
            }
        }
        
        QApplication::clipboard()->setText(clipboardText);
    }
}

void VideoDataView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (!proxyModel()) {
        return QTreeView::selectionChanged(selected, deselected);
    }
    
    auto enableActions = true;
    for (auto index : selected.indexes()) {
        const auto path = VideoModel::indexPath(proxyModel()->mapToSource(index));
        const auto indexIsValid =
            path.size() == 2 && (
                path.at(0).column == VideoModel::DistancesColumn ||
                path.at(0).column == VideoModel::TrajectoriesColumn ||
                path.at(0).column == VideoModel::AnglesColumn
            );
        
        if (!indexIsValid) {
            enableActions = false;
            break;
        }
    }
    
    _deleteAction->setEnabled(enableActions);
    _copyAction->setEnabled(enableActions);
    
    QTreeView::selectionChanged(selected, deselected);
}

QAbstractProxyModel* VideoDataView::proxyModel()
{
    return dynamic_cast<QAbstractProxyModel *>(model());
}

#include "videodataview.moc"
