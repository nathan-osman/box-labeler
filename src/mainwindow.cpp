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
#include <QHBoxLayout>
#include <QPushButton>
#include <QRect>
#include <QVBoxLayout>
#include <QWidget>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    // Create the text inputs and layout
    QVBoxLayout *textLayout = new QVBoxLayout;
    textLayout->addWidget(mStyle = new QTextEdit);
    textLayout->addWidget(mColor = new QTextEdit);
    textLayout->addWidget(mSize = new QTextEdit);
    textLayout->addStretch(0);

    // Create the print button
    QPushButton *pushButton = new QPushButton(tr("Print"));
    connect(pushButton, &QPushButton::clicked, this, &MainWindow::onClicked);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(pushButton);
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

void MainWindow::onClicked()
{
    //...
}
