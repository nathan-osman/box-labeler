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

#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

#include "multilinedelegate.h"
#include "sheetwidget.h"

const int DefaultRows = 2;
const int DefaultCols = 2;

SheetWidget::SheetWidget()
{
    // Create the header and footer line edits
    QLineEdit *headerEdit = new QLineEdit;
    QLineEdit *footerEdit = new QLineEdit;
    connect(headerEdit, &QLineEdit::textChanged, [this](const QString &text) {
        mSheet.headerText = text;
        emit changed();
    });
    connect(footerEdit, &QLineEdit::textChanged, [this](const QString &text) {
        mSheet.footerText = text;
        emit changed();
    });

    // Create the table
    QTableWidget *tableWidget = new QTableWidget;
    tableWidget->setItemDelegate(new MultilineDelegate(this));
    tableWidget->horizontalHeader()->hide();
    tableWidget->verticalHeader()->hide();
    connect(tableWidget, &QTableWidget::cellChanged, [this, tableWidget](int row) {
        tableWidget->resizeRowToContents(row);
        emit changed();
    });

    // Create the spinners for the table dimensions
    QSpinBox *rowSpinBox = new QSpinBox;
    QSpinBox *colSpinBox = new QSpinBox;
    connect(rowSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, tableWidget](int val) {
        tableWidget->setRowCount(val);
        emit changed();
    });
    connect(colSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, tableWidget](int val) {
        tableWidget->setColumnCount(val);
        emit changed();
    });
    rowSpinBox->setValue(DefaultRows);
    colSpinBox->setValue(DefaultCols);

    // Add everything to the layout
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->setMargin(0);
    vboxLayout->addWidget(new QLabel(tr("Header:")));
    vboxLayout->addWidget(headerEdit);
    vboxLayout->addWidget(new QLabel(tr("Footer:")));
    vboxLayout->addWidget(footerEdit);
    vboxLayout->addWidget(new QLabel(tr("Cells:")));
    vboxLayout->addWidget(tableWidget);
    vboxLayout->addWidget(new QLabel(tr("Rows:")));
    vboxLayout->addWidget(rowSpinBox);
    vboxLayout->addWidget(new QLabel(tr("Columns:")));
    vboxLayout->addWidget(colSpinBox);
    setLayout(vboxLayout);
}

const Sheet &SheetWidget::sheet() const
{
    return mSheet;
}
