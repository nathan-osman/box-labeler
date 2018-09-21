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
#include <QRect>
#include <QTabBar>
#include <QTableWidgetItem>
#include <QToolButton>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "multilinedelegate.h"

MainWindow::MainWindow()
{
    // Create the description label
    QLabel *label = new QLabel(tr(
        "Use the table below to create box labels.\n\n"
        "The page will be split evenly into the selected number of rows and columns."
    ));
    label->setWordWrap(true);

    // Create the new tab button
    QToolButton *newTabButton = new QToolButton;
    newTabButton->setText(tr("+"));
    connect(newTabButton, &QPushButton::clicked, this, &MainWindow::onNewTabClicked);

    // Create the tab control
    mTabWidget = new QTabWidget;
    mTabWidget->setTabsClosable(true);
    mTabWidget->addTab(new QWidget, QString());
    mTabWidget->setTabEnabled(0, false);
    mTabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, newTabButton);
    connect(mTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onCurrentChanged);
    connect(mTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

    // Initialize the default font
    mFont.setBold(true);
    mFont.setFamily("Calibri");

    // Create the print button
    mPrintButton = new QPushButton(tr("&Print..."));
    mPrintButton->setIcon(QIcon(":/img/print.png"));
    connect(mPrintButton, &QPushButton::clicked, this, &MainWindow::onPrintClicked);

    // Create the font button
    mFontButton = new QPushButton(tr("&Font..."));
    mFontButton->setIcon(QIcon(":/img/font.png"));
    connect(mFontButton, &QPushButton::clicked, this, &MainWindow::onFontClicked);

    // Create the row / col labels and spinboxes
    QLabel *rowLabel = new QLabel(tr("Rows:"));
    QLabel *colLabel = new QLabel(tr("Cols:"));
    mRowSpinBox = new QSpinBox;
    mColSpinBox = new QSpinBox;
    connect(mRowSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onRowValueChanged);
    connect(mColSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onColValueChanged);

    // Create the layout for the buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(mPrintButton);
    buttonLayout->addWidget(mFontButton);
    buttonLayout->addWidget(createHLine());
    buttonLayout->addWidget(rowLabel);
    buttonLayout->addWidget(mRowSpinBox);
    buttonLayout->addWidget(colLabel);
    buttonLayout->addWidget(mColSpinBox);
    buttonLayout->addStretch(0);

    // Create the layout that separates the table and buttons
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(mTabWidget);
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

    // Setup the window
    setStyleSheet("QPushButton { padding: 8px 16px; }");
    setWindowIcon(QIcon(":/img/box-labeler.png"));
    setWindowTitle(tr("Box Labeler"));
    resize(640, 480);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    // Create a new tab
    onNewTabClicked();
}

void MainWindow::onCurrentChanged()
{
    QTableWidget *tableWidget = curTableWidget();
    if (tableWidget) {
        mRowSpinBox->setValue(tableWidget->rowCount());
        mColSpinBox->setValue(tableWidget->columnCount());
    }
}

void MainWindow::onTabCloseRequested(int index)
{
    mTabWidget->removeTab(index);
    mTabWidget->setCurrentIndex(index - 1);

    toggleTools();
}

void MainWindow::onNewTabClicked()
{
    // Create the widget
    QTableWidget *tableWidget = new QTableWidget;
    tableWidget->setItemDelegate(new MultilineDelegate(this));
    tableWidget->setRowCount(3);
    tableWidget->setColumnCount(1);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setMinimumSectionSize(150);
    tableWidget->horizontalHeader()->hide();
    tableWidget->verticalHeader()->hide();
    connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged);

    // Add a new tab
    int newIndex = mTabWidget->count() - 1;
    mTabWidget->insertTab(newIndex, tableWidget, tr("Page"));
    mTabWidget->setCurrentIndex(newIndex);

    toggleTools();
}

void MainWindow::onCellChanged(int row)
{
    curTableWidget()->resizeRowToContents(row);
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

        // Print a page for each tab
        for (int n = 0; n < mTabWidget->count() - 1; ++n) {

            QTableWidget *tableWidget = qobject_cast<QTableWidget*>(
                        mTabWidget->widget(n));

            // Create a rect of the appropriate size
            QRectF rect = printer.pageRect();
            rect.setWidth(rect.width() / tableWidget->columnCount());
            rect.setHeight(rect.height() / tableWidget->rowCount());

            // Divide the page into evenly-sized cells and fit the text into them
            for (int i = 0; i < tableWidget->columnCount(); ++i) {
                for (int j = 0; j < tableWidget->rowCount(); ++j) {

                    // Draw the text value
                    QTableWidgetItem *item = tableWidget->item(j, i);
                    if (item) {
                        QString value = item->text();
                        fitText(painter,
                                rect.translated(i * rect.width(), j * rect.height()),
                                value);
                    }
                }
            }

            // Advance to the next page if there are more remaining
            if (n < mTabWidget->count() - 2) {
                printer.newPage();
            }
        }

        // Finish drawing
        painter.end();
    }
}

void MainWindow::onFontClicked()
{
    mFont = QFontDialog::getFont(nullptr, mFont);
}

void MainWindow::onRowValueChanged()
{
    curTableWidget()->setRowCount(mRowSpinBox->value());
}

void MainWindow::onColValueChanged()
{
    curTableWidget()->setColumnCount(mColSpinBox->value());
}

void MainWindow::toggleTools()
{
    bool enable = mTabWidget->count() > 1;

    mPrintButton->setEnabled(enable);
    mFontButton->setEnabled(enable);
    mRowSpinBox->setEnabled(enable);
    mColSpinBox->setEnabled(enable);
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
