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
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "pagewidget.h"

MainWindow::MainWindow()
{
    // Create the new tab button
    QToolButton *newTabButton = new QToolButton;
    newTabButton->setText(tr("+"));
    connect(newTabButton, &QPushButton::clicked, [this]() {
        int newIndex = mTabWidget->count() - 1;
        mTabWidget->insertTab(newIndex, new PageWidget, tr("Page"));
        mTabWidget->setCurrentIndex(newIndex);
        toggleTools();
    });

    // Create the tab control
    mTabWidget = new QTabWidget;
    mTabWidget->setTabsClosable(true);
    mTabWidget->addTab(new QWidget, QString());
    mTabWidget->setTabEnabled(0, false);
    mTabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, newTabButton);
    connect(mTabWidget, &QTabWidget::tabCloseRequested, [this](int index) {
        mTabWidget->removeTab(index);
        mTabWidget->setCurrentIndex(index - 1);
        toggleTools();
    });

    // Initialize the default font
    mFont.setBold(true);
    mFont.setFamily("Calibri");

    // Create the print button
    mPrintButton = new QPushButton(tr("&Print..."));
    mPrintButton->setIcon(QIcon(":/img/print.png"));
    connect(mPrintButton, &QPushButton::clicked, this, &MainWindow::onPrintClicked);

    // Create the font button
    QPushButton *fontButton = new QPushButton(tr("&Font..."));
    fontButton->setIcon(QIcon(":/img/font.png"));
    connect(fontButton, &QPushButton::clicked, [this]() {
        mFont = QFontDialog::getFont(nullptr, mFont);
    });

    // Create the about button
    QPushButton *aboutButton = new QPushButton(tr("&About..."));
    aboutButton->setIcon(QIcon(":/img/about.png"));
    connect(aboutButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(
            this,
            tr("About"),
            tr("Box Labeler\n\nCopyright 2018 - Nathan Osman")
        );
    });

    // Create the layout for the tools
    QVBoxLayout *toolsLayout = new QVBoxLayout;
    toolsLayout->addWidget(mPrintButton);
    toolsLayout->addWidget(fontButton);
    toolsLayout->addWidget(createHLine());
    toolsLayout->addWidget(aboutButton);
    toolsLayout->addStretch(0);

    // Create the layout that separates the tabs and buttons
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(mTabWidget);
    hboxLayout->addLayout(toolsLayout);

    // Create the central widget and layout
    QWidget *widget = new QWidget;
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->setSpacing(10);
    vboxLayout->addLayout(hboxLayout);
    widget->setLayout(vboxLayout);
    setCentralWidget(widget);

    // Setup the window
    setStyleSheet("QPushButton { padding: 8px 16px; }");
    setWindowIcon(QIcon(":/img/box-labeler.png"));
    setWindowTitle(tr("Box Labeler"));
    resize(640, 480);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    // Simulate the addition of a new tab
    newTabButton->click();
}

void MainWindow::onPrintClicked()
{
    // Initialize the printer
    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation(QPrinter::Landscape);

    // Show the print dialog
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {

        // Print each page
        for (int n = 0; n < mTabWidget->count() - 1; ++n) {

            // Have the widget draw the page
            qobject_cast<PageWidget*>(mTabWidget->widget(n))->draw(
                &printer,
                mFont,
                printer.pageRect()
            );

            // Advance to the next page if applicable
            if (n < mTabWidget->count() - 2) {
                printer.newPage();
            }
        }
    }
}

void MainWindow::toggleTools()
{
    mPrintButton->setEnabled(mTabWidget->count() > 1);
}

QWidget *MainWindow::createHLine()
{
    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    return frame;
}
