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

#include <QApplication>
#include <QDesktopWidget>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QList>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QRect>
#include <QVBoxLayout>
#include <QWidget>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    // Initialize the default font
    mFont.setBold(true);
    mFont.setFamily("Corbel");

    // Create the text inputs and layout
    QVBoxLayout *textLayout = new QVBoxLayout;
    textLayout->addWidget(mStyle = new QTextEdit);
    textLayout->addWidget(mColor = new QTextEdit);
    textLayout->addWidget(mSize = new QTextEdit);
    textLayout->addStretch(0);

    // Create the print button
    QPushButton *printButton = new QPushButton(tr("Print"));
    connect(printButton, &QPushButton::clicked, this, &MainWindow::onPrintClicked);

    // Create the font button
    QPushButton *fontButton = new QPushButton(tr("Font"));
    connect(fontButton, &QPushButton::clicked, this, &MainWindow::onFontClicked);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(printButton);
    buttonLayout->addWidget(fontButton);
    buttonLayout->addStretch(0);

    // Create the central widget
    QWidget *widget = new QWidget;
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addLayout(textLayout);
    hboxLayout->addLayout(buttonLayout);
    widget->setLayout(hboxLayout);
    setCentralWidget(widget);

    // Set the window title and geometry
    setWindowTitle(tr("Box Labeler"));
    resize(640, 480);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    // Initialize the text inputs
    mStyle->setAcceptRichText(false);
    mColor->setAcceptRichText(false);
    mSize->setAcceptRichText(false);
}

void MainWindow::onPrintClicked()
{
    // Initialize the printer and set the correct page orientation
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Landscape);

    // Show the print dialog
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {

        // Create a rect one-third the page height
        QRectF rect = printer.pageRect();
        rect.setHeight(rect.height() / 3);

        // Prepare to draw the text
        QPainter painter;
        painter.begin(&printer);

        // Draw each block of text
        foreach (QTextEdit *textEdit, QList<QTextEdit*>({mStyle, mColor, mSize})) {

            // Grab the text value
            QString value = textEdit->toPlainText();
            fitText(painter,  rect, value);

            // Move the rect down by its own height
            rect.translate(0, rect.height());
        }

        painter.end();
    }
}

void MainWindow::onFontClicked()
{
    mFont = QFontDialog::getFont(nullptr, mFont);
}

void MainWindow::fitText(QPainter &painter, const QRectF &rect, QString &text)
{
    // Begin with a large point size that will likely exceed the bounding rect
    // and shrink the size until the text fits or the size is too small

    for (int fontSize = 400; fontSize > 0; fontSize -= 2) {

        // Try the font size
        mFont.setPointSize(fontSize);
        painter.setFont(mFont);
        QRectF requiredRect = painter.boundingRect(rect, 0, text);

        // Determine if the text fits
        if (requiredRect.width() <= rect.width() &&
                requiredRect.height() <= rect.height()) {
            break;
        }
    }

    // Draw the text
    painter.drawText(rect, 0, text);
}
