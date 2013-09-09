/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "basejob.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <QImage>
#include <QSize>

#include <vector>

BaseJob::BaseJob(QObject *parent) :
    QThread(parent)
{}

QVector< QPointF > BaseJob::trackPoints(const QVector< QPointF >& startPoints,
                                        const QImage& startFrame,
                                        const QImage& endFrame,
                                        const QSize& windowSize)
{
    QImage i8startFrame(startFrame.convertToFormat(QImage::Format_Indexed8));
    QImage i8endFrame(endFrame.convertToFormat(QImage::Format_Indexed8));
    cv::Mat cvStartFrame(i8startFrame.height(), i8startFrame.width(), CV_8UC1, i8startFrame.scanLine(0));
    cv::Mat cvEndFrame(i8endFrame.height(), i8endFrame.width(), CV_8UC1, i8endFrame.scanLine(0));
    
    std::vector<cv::Point2f> cvStartPoints;
    
    for (const QPointF &startP : startPoints) {
        cv::Point2f cvP;
        cvP.x = startP.x();
        cvP.y = startP.y();
        cvStartPoints.push_back(cvP);
    }
    
    std::vector<cv::Point2f> cvEndPoints;
    std::vector<uchar> status;
    std::vector<float> err;
    Size cvWindowSize;

    cvWindowSize.height = windowSize.height();
    cvWindowSize.width = windowSize.width();

    cv::calcOpticalFlowPyrLK(cvStartFrame, cvEndFrame, cvStartPoints, cvEndPoints, status, err,
                            cvWindowSize,
                            // Magic numbers:
                            3, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS,
                                                20, 0.01),
                            0.5, 0);
    
    QVector<QPointF> endPoints;
    
    for (const cv::Point2f &cvEndP : cvEndPoints) {
        endPoints << QPointF(cvEndP.x, cvEndP.y);
    }
    
    return endPoints;
}
