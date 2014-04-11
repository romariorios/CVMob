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

#include "playslider.hpp"

#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>

PlaySlider::PlaySlider(QWidget* parent) :
    QSlider(parent)
{}

void PlaySlider::mousePressEvent(QMouseEvent* e)
{
    // adapted from http://stackoverflow.com/questions/11132597/qslider-mouse-direct-jump
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    if (e->button() == Qt::LeftButton &&
        sr.contains(e->pos()) == false) {
        int newVal;

        if (orientation() == Qt::Vertical) {
            double halfHandleWidth = (0.5 * sr.width()) + 0.5; // Correct rounding
            int adaptedPosX = e->x();

            if (adaptedPosX < halfHandleWidth) {
                adaptedPosX = halfHandleWidth;
            }

            if (adaptedPosX > width() - halfHandleWidth) {
                adaptedPosX = width() - halfHandleWidth;
            }

            double newWidth = (width() - halfHandleWidth) - halfHandleWidth;
            double normalizedPosition = (adaptedPosX - halfHandleWidth) / newWidth;

            newVal = minimum() + ((maximum()-minimum()) * normalizedPosition);
        } else {
            newVal = minimum() + ((maximum()-minimum()) * e->x()) / width();
        }

        if (invertedAppearance() == true) {
            setSliderPosition(maximum() - newVal);
        } else {
            setSliderPosition(newVal);
        }

        e->accept();
    }

    QSlider::mousePressEvent(e);
}
