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

#include "sheet.h"

Sheet::Sheet()
    : rowCount(0),
      colCount(0),
      orientation(Portrait),
      hSpacing(0),
      vSpacing(0)
{
}

void Sheet::draw(QPaintDevice *device, const QRectF &rect)
{
    bool hasHeader = !headerText.isEmpty();
    bool hasFooter = !footerText.isEmpty();

    // Ensure non-zero rows and columns
    if (!rowCount || !colCount) {
        return;
    }

    // Calculate the number of rows being drawn
    int drawRowCount = rowCount + (hasHeader ? 1 : 0) + (hasFooter ? 1 : 0);

    // Calculate the cell width and height, taking spacing into account
    qreal vOffset = rect.top();
    qreal cellWidth = (rect.width() - hSpacing * (colCount - 1)) / colCount;
    qreal cellHeight = (rect.height() - vSpacing * (drawRowCount - 1)) / drawRowCount;

    // Begin painting
    QPainter painter;
    painter.begin(device);

    // Draw the header (if applicable)
    if (hasHeader) {
        fitText(
            painter,
            font,
            QRectF(rect.left(), rect.top(), rect.width(), cellHeight),
            headerText
        );
        vOffset += cellHeight;
    }

    // Draw each cell
    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < colCount; ++j) {
            const Cell &cell = cells.at(i + j * colCount);
            fitText(
                painter,
                font,
                QRectF(
                    rect.left() + j * (cellWidth + hSpacing),
                    rect.top() + i * (cellHeight + vSpacing) + vOffset,
                    cellWidth,
                    cellHeight
                ),
                cell.text()
            );
        }
    }

    // Draw the footer (if applicable)
    if (hasFooter) {
        fitText(
            painter,
            font,
            QRectF(rect.left(), rect.bottom() - cellHeight, rect.width(), cellHeight),
            footerText
        );
    }

    // Finish painting
    painter.end();
}

void Sheet::fitText(QPainter &painter,
                    const QFont &font,
                    const QRectF &rect,
                    const QString &text)
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
