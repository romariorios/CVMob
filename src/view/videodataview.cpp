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

#include "videodataview.hpp"

#include <QAbstractProxyModel>
#include <QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QContextMenuEvent>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMenu>
#include <model/videomodel.hpp>

VideoDataView::VideoDataView(QWidget* parent) :
    QTreeView{parent}
{
    // Delete
    _deleteAction.setShortcut(tr("Delete"));

    connect(&_deleteAction, &QAction::triggered, [=]()
    {
        for (auto index : selectionModel()->selectedRows()) {
            model()->removeRow(index.row(), index.parent());
        }
    });

    addAction(&_deleteAction);
    _contextMenu.addAction(&_deleteAction);

    // Copy
    _copyAction.setShortcut(tr("Ctrl+C"));

    connect(&_copyAction, &QAction::triggered, this, &VideoDataView::copySelectionToClipboard);

    addAction(&_copyAction);
    _contextMenu.addAction(&_copyAction);

    // Save
    _saveAction.setShortcut(tr("Ctrl+S"));

    connect(&_saveAction, &QAction::triggered, [this]()
    {
        auto fileName = QFileDialog::getSaveFileName(
            this,
            tr("Save trajectory data"),
            QDir::homePath());

        if (fileName.isEmpty())
            return;

        QFile file{fileName};
        file.open(QIODevice::WriteOnly);
        QTextStream stream{&file};
        for (auto index : selectionModel()->selectedRows())
            stream << trajectoryDataString(index) << "\n";
    });

    addAction(&_saveAction);
    _contextMenu.addAction(&_saveAction);
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
    case VideoModel::AllDistancesCol:
        deleteString = tr("Delete distance");
        break;
    case VideoModel::AllTrajectoriesCol:
        deleteString = tr("Delete trajectory");
        break;
    case VideoModel::AllAnglesCol:
        deleteString = tr("Delete angle");
        break;
    default:
        deleteString = tr("Delete");
    }
    _deleteAction.setText(deleteString);

    _contextMenu.popup(e->globalPos());
}

QString VideoDataView::trajectoryDataString(const QModelIndex &index)
{
    auto path = VideoModel::indexPath(proxyModel()->mapToSource(index));
    QString clipboardText;

    if (path.size() == 2 && path.at(0).column == VideoModel::AllTrajectoriesCol) {
        clipboardText += index.data().toString() + "\n";
        int section = 0;
        for (
            auto header = model()->headerData(section, Qt::Horizontal);
            header.isValid();
            ++section, header = model()->headerData(section, Qt::Horizontal)
        ) {
            clipboardText += header.toString() + "\t";
        }
        clipboardText += "\n";

        for (int i = 0; i < model()->rowCount(index); ++i) {
            int col = 0;
            for (
                auto clipData = model()->index(i, col, index).data();
                clipData.isValid();
                ++col, clipData = model()->index(i, col, index).data()
            ) {
                clipboardText += clipData.toString() + "\t";
            }
            clipboardText += "\n";
        }
    }

    return clipboardText;
}

void VideoDataView::copySelectionToClipboard()
{
    for (auto index : selectionModel()->selectedRows())
        QApplication::clipboard()->setText(trajectoryDataString(index));
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
                path.at(0).column == VideoModel::AllDistancesCol ||
                path.at(0).column == VideoModel::AllTrajectoriesCol ||
                path.at(0).column == VideoModel::AllAnglesCol
            );

        if (!indexIsValid) {
            enableActions = false;
            break;
        }
    }

    _deleteAction.setEnabled(enableActions);
    _copyAction.setEnabled(enableActions);

    QTreeView::selectionChanged(selected, deselected);
}

QAbstractProxyModel* VideoDataView::proxyModel()
{
    return dynamic_cast<QAbstractProxyModel *>(model());
}

#include "videodataview.moc"
