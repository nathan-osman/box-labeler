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
    connect(mSheetWidget, &SheetWidget::changed, [this, graphicsPixmapItem]() {

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
    });

    // Create the vertical line
    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);

    // Create the print button
    QPushButton *printButton = new QPushButton(tr("&Print"));
    printButton->setIcon(QIcon(":/img/print.png"));
    connect(printButton, &QPushButton::clicked, this, &MainWindow::onPrintClicked);

    // Create the clear button
    QPushButton *clearButton = new QPushButton(tr("&Clear"));
    clearButton->setIcon(QIcon(":/img/clear.png"));
    connect(clearButton, &QPushButton::clicked, mSheetWidget, &SheetWidget::clear);

    // Create the about button
    QPushButton *aboutButton = new QPushButton(tr("&About..."));
    aboutButton->setIcon(QIcon(":/img/about.png"));
    connect(aboutButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(
            this,
            tr("About"),
            tr("Box Labeler\n\nCopyright 2019 - Nathan Osman")
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
    vboxLayout->addWidget(clearButton);
    vboxLayout->addStretch();
    vboxLayout->addWidget(mQueueWidget);
    vboxLayout->addWidget(aboutButton);

    // Create the layout
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setSpacing(16);
    hboxLayout->addWidget(splitter, 1);
    hboxLayout->addWidget(frame);
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

void MainWindow::onPrintClicked()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        mQueueWidget->addTask(
            new PrintTask(printer.printerName(), mSheetWidget->sheet())
        );
    }
}
