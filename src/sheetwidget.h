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

#ifndef SHEETWIDGET_H
#define SHEETWIDGET_H

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>

#include "sheet.h"

/**
 * @brief Widget for editing a sheet
 */
class SheetWidget : public QWidget
{
    Q_OBJECT

public:

    SheetWidget();

    Sheet &sheet();

signals:

    void changed();

public slots:

    void clear();

private:

    Sheet mSheet;

    QLineEdit *mHeaderEdit;
    QLineEdit *mFooterEdit;

    QSpinBox *mRowSpinBox;
    QSpinBox *mColSpinBox;

    QSpinBox *mHSpacingSpinBox;
    QSpinBox *mVSpacingSpinBox;

    QComboBox *mComboBox;

    QSpinBox *mBorderSpinBox;
    QSpinBox *mMarginSpinBox;
};

#endif // SHEETWIDGET_H
