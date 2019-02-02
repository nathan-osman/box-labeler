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

#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QFont>
#include <QLineEdit>
#include <QPaintDevice>
#include <QPainter>
#include <QRectF>
#include <QTableWidget>
#include <QWidget>

/**
 * @brief Widget for displaying and editing page data
 */
class PageWidget : public QWidget
{
    Q_OBJECT

public:

    PageWidget();

    /**
     * @brief Draw the page
     * @param device use this device for drawing the page
     * @param font use this font for drawing text
     * @param rect dimensions of the page
     * @param spacing the amount of spacing between cells
     */
    void draw(QPaintDevice *device, const QFont &font, const QRectF &rect, int spacing);

signals:

    /**
     * @brief Indicate that the widget has changed
     */
    void changed();

private:

    void fitText(QPainter &painter,
                 const QFont &font,
                 const QRectF &rect,
                 const QString &text);

    QLineEdit *mHeaderEdit;
    QLineEdit *mFooterEdit;

    QTableWidget *mTableWidget;
};

#endif // PAGEWIDGET_H
