/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include "model.h"

#include <QFile>
#include <QTextStream>
#include <QVarLengthArray>

#include <QtOpenGL>
#include <QDebug>
#include <QRegExp>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

Model::Model()
{


}
void Model::loadFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    //如下两个变量存放模型的边界
    Point3d boundsMin( 1e9, 1e9, 1e9);
    Point3d boundsMax(-1e9,-1e9,-1e9);

    //读取obj格式的文件
    QTextStream in(&file);
    //now begin to read the verters

    Point3d pi;
    float currentZ = 0.0;
    float currentX = 0.0;
    float currentY = 0.0;
    QRegExp regZ("[\w\d\s]*Z([0-9\.]*[0-9]*)");
    QRegExp regL(";LAYER: *([0-9]*)");
    QRegExp regXY("G1 [\w\d\s]* *X([0-9\.]*[0-9]*) *Y([0-9\.]*[0-9]*)");
    bool isZ = false;
    bool isIn = false;
    while(!in.atEnd())
    {
        QString current_str = in.readLine();
        if(current_str.contains(regXY))
        {
            currentX = regXY.cap(1).toFloat();
            currentY = regXY.cap(2).toFloat();
            boundsMin.x = qMin(boundsMin.x,currentX);
            boundsMax.x = qMax(boundsMax.x, currentX);
            boundsMin.y = qMin(boundsMin.y,currentY);
            boundsMax.y = qMax(boundsMax.y, currentY);
            boundsMin.z = qMin(boundsMin.z,currentZ);
            boundsMax.z = qMax(boundsMax.z, currentZ);
             qDebug()<< QObject::tr("Xs=%1").arg(currentX);
            pi = Point3d(currentX,currentY,currentZ);
            m_points.push_back(pi);
        }
        if(current_str.contains(regL))
        {
            if((regL.cap(1))=="0")
                isIn = false;
            else
                isIn = true;

            isZ = true;

        }
        if(current_str.contains(regZ)&&(isZ))
        {

            isZ = false;
            QString str_Z = regZ.cap(1);
            currentZ = str_Z.toFloat();
        }

    }
    const Point3d bounds = boundsMax - boundsMin;
    const qreal scale = 100 / qMax(bounds.x, qMax(bounds.y, bounds.z));
        for(int j=0;j<m_points.size();j++)
        {
            m_points[j] = (m_points[j] - (boundsMin + bounds * 0.5)) * scale;
            qDebug()<< QObject::tr("X=%1").arg(m_points[j].x);
        }
}
//渲染模型
void Model::render()
{
glPushMatrix();
    glEnable(GL_DEPTH_TEST);
    glColor3d(10,10,190);

    glBegin(GL_LINE_STRIP);
    for(int j=0;j<m_points.size();j++)
    {
//            glVertex3f(m_points[j-1].x,m_points[j-1].y,m_points[j-1].z);
        glVertex3f(m_points[j].x,m_points[j].y,m_points[j].z);
//            glVertex3f(m_points[j+1].x,m_points[j+1].y,m_points[j+1].z);
    }
    glEnd();
    glPopMatrix();
//    glMatrixMode(GL_PROJECTION);


}

