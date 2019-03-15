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

#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "multilinedelegate.h"
#include "sheetwidget.h"

const int DefaultRows = 1;
const int DefaultCols = 1;

const int DefaultHSpacing = 20;
const int DefaultVSpacing = 0;

const int DefaultBorder = 4;
const int DefaultMargin = 16;

SheetWidget::SheetWidget()
    : mHeaderEdit(new QLineEdit),
      mFooterEdit(new QLineEdit),
      mRowSpinBox(new QSpinBox),
      mColSpinBox(new QSpinBox),
      mHSpacingSpinBox(new QSpinBox),
      mVSpacingSpinBox(new QSpinBox),
      mComboBox(new QComboBox),
      mBorderSpinBox(new QSpinBox),
      mMarginSpinBox(new QSpinBox),
      mCopiesSpinBox(new QSpinBox)
{
    connect(mHeaderEdit, &QLineEdit::textChanged, [this](const QString &text) {
        mSheet.headerText = text;
        emit changed();
    });
    connect(mFooterEdit, &QLineEdit::textChanged, [this](const QString &text) {
        mSheet.footerText = text;
        emit changed();
    });

    // Create the table
    QTableWidget *tableWidget = new QTableWidget;
    tableWidget->setItemDelegate(new MultilineDelegate(this));
    tableWidget->horizontalHeader()->hide();
    tableWidget->verticalHeader()->hide();
    connect(tableWidget, &QTableWidget::cellChanged, [this, tableWidget](int row, int col) {
        mSheet.cell(row, col).setText(tableWidget->item(row, col)->text());
        tableWidget->resizeRowToContents(row);
        emit changed();
    });

    // Create the spinners for the table dimensions
    connect(mRowSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, tableWidget](int val) {
        mSheet.setRows(val);
        tableWidget->setRowCount(val);
        emit changed();
    });
    connect(mColSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, tableWidget](int val) {
        mSheet.setCols(val);
        tableWidget->setColumnCount(val);
        emit changed();
    });

    // Create the spinners for spacing
    connect(mHSpacingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int val) {
        mSheet.hSpacing = val;
        emit changed();
    });
    connect(mVSpacingSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int val) {
        mSheet.vSpacing = val;
        emit changed();
    });

    // Create the combo box for page orientation
    mComboBox->addItem(tr("Portrait"), Sheet::Portrait);
    mComboBox->addItem(tr("Landscape"), Sheet::Landscape);
    connect(mComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]() {
        mSheet.orientation = mComboBox->currentData().toInt();
        emit changed();
    });

    // Create the combo box for border and margin
    connect(mBorderSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int val) {
        mSheet.border = val;
        emit changed();
    });
    connect(mMarginSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int val) {
        mSheet.margin = val;
        emit changed();
    });

    // Same for copies
    connect(mCopiesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int val) {
        mSheet.copies = val;
    });

    // Add everything to the layout
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setMargin(0);
    gridLayout->addWidget(new QLabel(tr("Header:")), 0, 0, 1, 2);
    gridLayout->addWidget(mHeaderEdit, 1, 0, 1, 2);
    gridLayout->addWidget(new QLabel(tr("Footer:")), 2, 0, 1, 2);
    gridLayout->addWidget(mFooterEdit, 3, 0, 1, 2);
    gridLayout->addWidget(new QLabel(tr("Cells:")), 4, 0, 1, 2);
    gridLayout->addWidget(tableWidget, 5, 0, 1, 2);
    gridLayout->addWidget(new QLabel(tr("Rows:")), 6, 0, 1, 1);
    gridLayout->addWidget(mRowSpinBox, 7, 0, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Columns:")), 6, 1, 1, 1);
    gridLayout->addWidget(mColSpinBox, 7, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Horizontal Spacing:")), 8, 0, 1, 1);
    gridLayout->addWidget(mHSpacingSpinBox, 9, 0, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Vertical Spacing:")), 8, 1, 1, 1);
    gridLayout->addWidget(mVSpacingSpinBox, 9, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Orientation:")), 10, 0, 1, 2);
    gridLayout->addWidget(mComboBox, 11, 0, 1, 2);
    gridLayout->addWidget(new QLabel(tr("Border:")), 12, 0, 1, 1);
    gridLayout->addWidget(mBorderSpinBox, 13, 0, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Margin:")), 12, 1, 1, 1);
    gridLayout->addWidget(mMarginSpinBox, 13, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Copies:")), 14, 0, 1, 1);
    gridLayout->addWidget(mCopiesSpinBox, 15, 0, 1, 1);
    setLayout(gridLayout);

    // Reset everything
    clear();
}

Sheet &SheetWidget::sheet()
{
    return mSheet;
}

void SheetWidget::clear()
{
    mHeaderEdit->clear();
    mFooterEdit->clear();

    // Trick to clear the table
    mRowSpinBox->setValue(0);

    mRowSpinBox->setValue(DefaultRows);
    mColSpinBox->setValue(DefaultCols);

    mHSpacingSpinBox->setValue(DefaultHSpacing);
    mVSpacingSpinBox->setValue(DefaultVSpacing);

    mComboBox->setCurrentIndex(Sheet::Landscape);

    mBorderSpinBox->setValue(DefaultBorder);
    mMarginSpinBox->setValue(DefaultMargin);

    mCopiesSpinBox->setValue(1);
}
