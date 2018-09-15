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
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QRect>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    // Create the description label
    QLabel *label = new QLabel(tr(
        "Use the table below to create box labels.\n\n"
        "Enter the information for each label on a separate row. "
        "Use the Add / Insert buttons to add more rows as needed."
    ));
    label->setWordWrap(true);

    // Initialize the table widget
    mTableWidget = new QTableWidget;
    mTableWidget->setColumnCount(3);
    mTableWidget->setHorizontalHeaderLabels(
                QStringList() << tr("Style") << tr("Color") << tr("Sizes"));
    mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Initialize the default font
    mFont.setBold(true);
    mFont.setFamily("Corbel");

    // Create the print button
    QPushButton *printButton = new QPushButton(tr("&Print..."));
    connect(printButton, &QPushButton::clicked, this, &MainWindow::onPrintClicked);

    // Create the font button
    QPushButton *fontButton = new QPushButton(tr("&Font..."));
    connect(fontButton, &QPushButton::clicked, this, &MainWindow::onFontClicked);

    // Create the add button
    QPushButton *addButton = new QPushButton(tr("&Add Row"));
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);

    // Create the insert button
    QPushButton *insertButton = new QPushButton(tr("&Insert Row"));
    insertButton->setToolTip(tr("Insert a row before the currently selected one"));
    connect(insertButton, &QPushButton::clicked, this, &MainWindow::onInsertClicked);

    // Create the delete button
    QPushButton *deleteButton = new QPushButton(tr("&Delete Row"));
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);

    // Create the layout for the buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(printButton);
    buttonLayout->addWidget(fontButton);
    buttonLayout->addWidget(createHLine());
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(insertButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch(0);

    // Create the layout that separates the table and buttons
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(mTableWidget);
    hboxLayout->addLayout(buttonLayout);

    // Create the central widget and layout
    QWidget *widget = new QWidget;
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->setSpacing(10);
    vboxLayout->addWidget(label);
    vboxLayout->addWidget(createHLine());
    vboxLayout->addLayout(hboxLayout);
    widget->setLayout(vboxLayout);
    setCentralWidget(widget);

    // Set the window title and geometry
    setWindowIcon(QIcon(":/img/box-labeler.png"));
    setWindowTitle(tr("Box Labeler"));
    resize(640, 480);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    // Create one new row to begin with
    onAddClicked();
}

void MainWindow::onPrintClicked()
{
    // Initialize the printer and set the correct page orientation
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Landscape);

    // Show the print dialog
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {

        // Prepare to render the document
        QPainter painter;
        painter.begin(&printer);

        // Draw each page
        for (int i = 0; i < mTableWidget->rowCount(); ++i) {

            // Create a rect one-third the page height
            QRectF rect = printer.pageRect();
            rect.setHeight(rect.height() / 3);

            // Draw the three columns
            for (int j = 0; j < 3; ++j) {

                // Grab the text value
                QString value = mTableWidget->item(i, j)->text();
                fitText(painter,  rect, value);

                // Move the rect down by its own height
                rect.translate(0, rect.height());
            }

            // Advance to the next page if necessary
            if (i + 1 < mTableWidget->rowCount()) {
                printer.newPage();
            }
        }

        painter.end();
    }
}

void MainWindow::onFontClicked()
{
    mFont = QFontDialog::getFont(nullptr, mFont);
}

void MainWindow::onAddClicked()
{
    mTableWidget->setRowCount(mTableWidget->rowCount() + 1);
}

void MainWindow::onInsertClicked()
{
    mTableWidget->insertRow(mTableWidget->currentRow());
}

void MainWindow::onDeleteClicked()
{
    if (mTableWidget->rowCount()) {
        mTableWidget->removeRow(mTableWidget->currentRow());
    }
}

QWidget *MainWindow::createHLine()
{
    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    return frame;
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

        // If the text fits, draw it
        if (requiredRect.width() <= rect.width() &&
                requiredRect.height() <= rect.height()) {
            painter.drawText(rect, 0, text);
            break;
        }
    }
}
