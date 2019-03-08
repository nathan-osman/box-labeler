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

#include <QMarginsF>
#include <QPen>

#include "sheet.h"

Sheet::Sheet()
    : orientation(Portrait),
      hSpacing(0),
      vSpacing(0),
      border(0),
      margin(0),
      mColCount(0)
{
    font.setBold(true);
    font.setFamily("Calibri");
}

Cell &Sheet::cell(int row, int col)
{
    Q_ASSERT(row < mCells.count());
    auto &cellRow = mCells[row];
    if (cellRow.count() != mColCount) {
        cellRow.resize(mColCount);
    }
    return cellRow[col];
}

void Sheet::setRows(int rows)
{
    mCells.resize(rows);
}

void Sheet::setCols(int cols)
{
    mColCount = cols;
}

void Sheet::draw(QPaintDevice *device, const QSize &size)
{
    bool hasHeader = !headerText.isEmpty();
    bool hasFooter = !footerText.isEmpty();

    // Ensure non-zero rows and columns
    if (!mCells.count() || !mColCount) {
        return;
    }

    // Begin painting
    QPainter painter;
    painter.begin(device);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(0, 0, size.width(), size.height());
    painter.setViewport(0, 0, device->width(), device->height());

    // Draw the border
    if (border) {
        auto halfBorder = border / 2;
        painter.setPen(QPen(Qt::black, border, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        painter.drawRect(halfBorder, halfBorder, size.width() - border, size.height() - border);
    }

    // Create a rect for the client area
    QRectF clientRect(margin, margin, size.width() - margin * 2, size.height() - margin * 2);

    // Calculate the number of rows being drawn
    int rowCount = mCells.count() + (hasHeader ? 1 : 0) + (hasFooter ? 1 : 0);

    // Calculate the cell width and height, taking spacing into account
    qreal vOffset = 0;
    qreal cellWidth = (clientRect.width() - hSpacing * (mColCount - 1)) / mColCount;
    qreal cellHeight = (clientRect.height() - vSpacing * (rowCount - 1)) / rowCount;

    // Draw the header if applicable
    if (hasHeader) {
        fitText(
            painter,
            QRectF(
                clientRect.left(),
                clientRect.top(),
                clientRect.width(),
                cellHeight
            ),
            headerText
        );
        vOffset = cellHeight + vSpacing;
    }

    // Draw each cell
    for (auto i = 0; i < mCells.count(); ++i) {
        for (auto j = 0; j < mColCount; ++j) {
            auto &c = cell(i, j);
            fitText(
                painter,
                QRectF(
                    clientRect.left() + j * (cellWidth + hSpacing),
                    clientRect.top() + i * (cellHeight + vSpacing) + vOffset,
                    cellWidth,
                    cellHeight
                ),
                c.text()
            );
        }
    }

    // Draw the footer (if applicable)
    if (hasFooter) {
        fitText(
            painter,
            QRectF(
                clientRect.left(),
                clientRect.bottom() - cellHeight,
                clientRect.width(),
                cellHeight
            ),
            footerText
        );
    }

    // Finish painting
    painter.end();
}

void Sheet::fitText(QPainter &painter,
                    const QRectF &rect,
                    const QString &text) const
{
    QFont trialFont = font;

    // Beginning with a size identical to the height of the bounding rect,
    // slowly reduce the text size until it fits in the rect
    for (int fontSize = static_cast<int>(rect.height()); fontSize > 0; fontSize -= 2) {

        // Calculate the size of the rect at this font size
        trialFont.setPointSize(fontSize);
        painter.setFont(trialFont);
        QRectF requiredRect = painter.boundingRect(rect, 0, text);

        // If the text fits, draw it
        if (requiredRect.width() <= rect.width() &&
                requiredRect.height() <= rect.height()) {
            painter.drawText(rect, Qt::AlignVCenter, text);
            return;
        }
    }
}
