/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "settings.hpp"
#include "ui_settings.h"

#include <QSettings>

Settings::Settings(QWidget* parent) :
    QDialog(parent),
    _ui(new Ui::Settings)
{
    _ui->setupUi(this);
    
    loadSettings();
    
    connect(_ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onButtonBoxCliked(QAbstractButton*)));
}

void Settings::saveSettings()
{
    QSettings settings;
    
    settings.setValue("video/respectFramerate", _ui->videoFramerateBox->isChecked());
    settings.setValue("video/antiAlias", _ui->videoAntiAliasBox->isChecked());
    settings.setValue("plot/period", _ui->plotPeriodBox->value());
}

void Settings::loadSettings()
{
    QSettings settings;
    
    _ui->videoFramerateBox->setChecked(settings.value("video/respectFramerate", false).toBool());
    _ui->videoAntiAliasBox->setChecked(settings.value("video/antiAlias", true).toBool());
    _ui->plotPeriodBox->setValue(settings.value("plot/period", 200).toInt());
    
}

void Settings::onButtonBoxCliked(QAbstractButton *button)
{
    switch (_ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::AcceptRole:
        close();
    case QDialogButtonBox::ApplyRole:
        saveSettings();
        emit settingsChanged();
        loadSettings();
        break;
    case QDialogButtonBox::RejectRole:
        close();
        loadSettings();
        break;
    default:
        close();
        loadSettings();
    }
}

Settings::~Settings()
{
    delete _ui;
}
