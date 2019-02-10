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
#include <QBrush>
#include <QDesktopWidget>
#include <QFontDialog>
#include <QFrame>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QPageSize>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QRect>
#include <QSplitter>
#include <QVBoxLayout>

#include "config.h"
#include "mainwindow.h"
#include "printtask.h"
#include "queuewidget.h"
#include "sheetwidget.h"

MainWindow::MainWindow()
    : mSheetWidget(new SheetWidget),
      mQueueWidget(new QueueWidget)
{
    // Create the graphics scene and view
    QGraphicsPixmapItem *graphicsPixmapItem = new QGraphicsPixmapItem;
    QGraphicsScene *graphicsScene= new QGraphicsScene;
    graphicsScene->addItem(graphicsPixmapItem);
    QGraphicsView *graphicsView = new QGraphicsView(graphicsScene);
    graphicsView->setBackgroundBrush(QBrush(Qt::gray));

    // Draw the preview when the widget changes
    connect(mSheetWidget, &SheetWidget::changed, [this, graphicsScene, graphicsPixmapItem]() {

        // Create the rect (at 36 DPI)
        QRect pageRect = QPageSize(QPageSize::Letter).rectPixels(36);

        // Transpose dimensions for landscape
        if (mSheetWidget->sheet().orientation == Sheet::Landscape) {
            pageRect = pageRect.transposed();
        }

        // Create the pixmap
        QPixmap pixmap(pageRect.width(), pageRect.height());
        pixmap.fill();

        // Draw the contents and display it
        mSheetWidget->sheet().draw(&pixmap, pageRect);
        graphicsPixmapItem->setPixmap(pixmap);
        graphicsScene->setSceneRect(pageRect);
    });

    // Create the vertical line
    QFrame *vFrame = new QFrame;
    vFrame->setFrameShape(QFrame::VLine);
    vFrame->setFrameShadow(QFrame::Sunken);

    // Create the print button
    QPushButton *printButton = new QPushButton(tr("&Print"));
    printButton->setDefault(true);
    printButton->setIcon(QIcon(":/img/print.png"));
    connect(printButton, &QPushButton::clicked, this, &MainWindow::onPrintClicked);

    // Create the print and clear button
    QPushButton *printAndClearButton = new QPushButton(tr("Print &and Clear"));
    printAndClearButton->setIcon(QIcon(":/img/print.png"));
    connect(printAndClearButton, &QPushButton::clicked, [this]() {
        if (onPrintClicked()) {
            mSheetWidget->clear();
        }
    });

    // Create the clear button
    QPushButton *clearButton = new QPushButton(tr("&Clear"));
    clearButton->setIcon(QIcon(":/img/clear.png"));
    connect(clearButton, &QPushButton::clicked, mSheetWidget, &SheetWidget::clear);

    // Create the horizontal line
    QFrame *hFrame = new QFrame;
    hFrame->setFrameShape(QFrame::HLine);
    hFrame->setFrameShadow(QFrame::Sunken);

    // Create the Select Printer button
    QPushButton *selectPrinterButton = new QPushButton(tr("&Select Printer..."));
    selectPrinterButton->setIcon(QIcon(":/img/preferences.png"));
    connect(selectPrinterButton, &QPushButton::clicked, this, &MainWindow::onSelectPrinterClicked);

    // Create the Select Font button
    QPushButton *selectFontButton = new QPushButton(tr("Select &Font..."));
    selectFontButton->setIcon(QIcon(":/img/font.png"));
    connect(selectFontButton, &QPushButton::clicked, [this]() {
        mSheetWidget->sheet().font = QFontDialog::getFont(
            nullptr, mSheetWidget->sheet().font
        );
        emit mSheetWidget->changed();
    });

    // Create the about button
    QPushButton *aboutButton = new QPushButton(tr("&About..."));
    aboutButton->setIcon(QIcon(":/img/about.png"));
    connect(aboutButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(
            this,
            tr("About"),
            tr("Box Labeler %1\n\nCopyright 2019 - Nathan Osman").arg(PROJECT_VERSION)
        );
    });

    // TODO: splitter width

    // Create the splitter
    QSplitter *splitter = new QSplitter;
    splitter->setHandleWidth(16);
    splitter->addWidget(graphicsView);
    splitter->addWidget(mSheetWidget);

    // Create the vbox layout for the buttons
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(printButton);
    vboxLayout->addWidget(printAndClearButton);
    vboxLayout->addWidget(clearButton);
    vboxLayout->addWidget(hFrame);
    vboxLayout->addWidget(selectPrinterButton);
    vboxLayout->addWidget(selectFontButton);
    vboxLayout->addStretch();
    vboxLayout->addWidget(mQueueWidget);
    vboxLayout->addWidget(aboutButton);

    // Create the layout
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setSpacing(16);
    hboxLayout->addWidget(splitter, 1);
    hboxLayout->addWidget(vFrame);
    hboxLayout->addLayout(vboxLayout);

    // Create the central widget
    QWidget *widget = new QWidget;
    widget->setLayout(hboxLayout);
    setCentralWidget(widget);

    // Initialize window properties
    setStyleSheet("QPushButton { padding: 8px 16px; }");
    setWindowIcon(QIcon(":/img/box-labeler.png"));
    setWindowTitle(tr("Box Labeler"));
    resize(1024, 480);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    // Redraw the preview
    mSheetWidget->changed();
}

bool MainWindow::onSelectPrinterClicked()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        mPrinterName = printer.printerName();
        return true;
    }
    return false;
}

bool MainWindow::onPrintClicked()
{
    if (!mPrinterName.isEmpty() || onSelectPrinterClicked()) {
        mQueueWidget->addTask(
            new PrintTask(mPrinterName, mSheetWidget->sheet())
        );
        return true;
    }
    return false;
}
