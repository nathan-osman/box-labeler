/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef SHEET_H
#define SHEET_H

#include <QFont>
#include <QPaintDevice>
#include <QPainter>
#include <QRectF>
#include <QString>
#include <QVector>

#include "cell.h"

/**
 * @brief Sheet containing cells
 */
class Sheet
{
public:

    Sheet();

    QString headerText;
    QString footerText;

    QFont font;

    enum {
        Portrait,
        Landscape
    } orientation;

    int hSpacing;
    int vSpacing;

    Cell &cell(int row, int col);
    void setRows(int rows);
    void setCols(int cols);

    void draw(QPaintDevice *device, const QRectF &rect);

private:

    void fitText(QPainter &painter,
                 const QRectF &rect,
                 const QString &text) const;

    int mColCount;
    QVector<QVector<Cell>> mCells;
};

#endif // SHEET_H
