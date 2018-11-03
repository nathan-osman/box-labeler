/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QSpinBox>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "multilinedelegate.h"
#include "pagewidget.h"

PageWidget::PageWidget()
{
    // Create the grid containing the header and footer controls
    QGridLayout *headerFooterLayout = new QGridLayout;
    headerFooterLayout->addWidget(new QLabel(tr("Header:")), 0, 0);
    headerFooterLayout->addWidget(new QLabel(tr("Footer:")), 1, 0);
    headerFooterLayout->addWidget(mHeaderEdit = new QLineEdit, 0, 1);
    headerFooterLayout->addWidget(mFooterEdit = new QLineEdit, 1, 1);

    connect(mHeaderEdit, &QLineEdit::textChanged, this, &PageWidget::changed);
    connect(mFooterEdit, &QLineEdit::textChanged, this, &PageWidget::changed);

    // Create the spinners for controlling table size

    QSpinBox *rowSpinBox = new QSpinBox;
    rowSpinBox->setValue(1);
    connect(rowSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int val) {
        mTableWidget->setRowCount(val);
        emit changed();
    });

    QSpinBox *colSpinBox = new QSpinBox;
    colSpinBox->setValue(1);
    connect(colSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int val) {
        mTableWidget->setColumnCount(val);
        emit changed();
    });

    // Create the layout for the controls
    QGridLayout *controlLayout = new QGridLayout;
    controlLayout->addWidget(new QLabel(tr("Rows:")), 0, 0);
    controlLayout->addWidget(rowSpinBox, 0, 1);
    controlLayout->addWidget(new QLabel(tr("Columns:")), 1, 0);
    controlLayout->addWidget(colSpinBox, 1, 1);

    // Create the layout containing the above layout, table view, and control layout
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addLayout(headerFooterLayout);
    vboxLayout->addWidget(mTableWidget = new QTableWidget);
    vboxLayout->addLayout(controlLayout);

    // Initialize the table widget
    mTableWidget->setItemDelegate(new MultilineDelegate(this));
    mTableWidget->setRowCount(rowSpinBox->value());
    mTableWidget->setColumnCount(colSpinBox->value());
    mTableWidget->horizontalHeader()->hide();
    mTableWidget->verticalHeader()->hide();

    // Resize the cell when its value changes
    connect(mTableWidget, &QTableWidget::cellChanged, [this](int row) {
        mTableWidget->resizeRowToContents(row);
        emit changed();
    });

    // Set the layout for the widget
    setLayout(vboxLayout);
}

void PageWidget::draw(QPaintDevice *device, const QFont &font, const QRectF &rect)
{
    bool hasHeader = !mHeaderEdit->text().isEmpty();
    bool hasFooter = !mFooterEdit->text().isEmpty();

    // Calculate the number of rows to divide the rect into
    int numRows = mTableWidget->rowCount() +
            (hasHeader ? 1 : 0) +
            (hasFooter ? 1 : 0);

    // Calculate cell width and height
    qreal offset = rect.top();
    qreal cellWidth = rect.width() / mTableWidget->columnCount();
    qreal cellHeight = rect.height() / numRows;

    // Begin painting
    QPainter painter;
    painter.begin(device);

    // Draw the header (if applicable)
    if (hasHeader) {
        fitText(
            painter,
            font,
            QRectF(rect.left(), rect.top(), rect.width(), cellHeight),
            mHeaderEdit->text()
        );
        offset += cellHeight;
    }

    // Draw each cell
    for (int i = 0; i < mTableWidget->columnCount(); ++i) {
        for (int j = 0; j < mTableWidget->rowCount(); ++j) {
            QTableWidgetItem *item = mTableWidget->item(j, i);
            if (item) {
                fitText(
                    painter,
                    font,
                    QRectF(
                        rect.left() + i * cellWidth,
                        rect.top() + offset + j * cellHeight,
                        cellWidth,
                        cellHeight
                    ),
                    item->text()
                );
            }
        }
    }

    // Draw the footer (if applicable)
    if (hasFooter) {
        fitText(
            painter,
            font,
            QRectF(rect.left(), rect.bottom() - cellHeight, rect.width(), cellHeight),
            mFooterEdit->text()
        );
    }

    // Finish painting
    painter.end();
}

void PageWidget::fitText(QPainter &painter,
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
