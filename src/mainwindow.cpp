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
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QPageSize>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QSplitter>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "pagewidget.h"

struct PageType {
    QString name;
    QPrinter::Orientation orientation;
    QPageSize size;
};

const QList<PageType> gPageTypes = {
    PageType{ "Letter (landscape)", QPrinter::Landscape, QPageSize(QPageSize::Letter) },
    PageType{ "Letter (portrait)", QPrinter::Portrait, QPageSize(QPageSize::Letter) },
    PageType{ "Label", QPrinter::Landscape, QPageSize(QSizeF(4, 6), QPageSize::Inch) }
};

MainWindow::MainWindow()
{
    // Create the graphics scene and view
    QGraphicsScene *graphicsScene= new QGraphicsScene;
    graphicsScene->addItem(mGraphicsPixmapItem = new QGraphicsPixmapItem);
    QGraphicsView *graphicsView = new QGraphicsView(graphicsScene);
    graphicsView->setBackgroundBrush(QBrush(Qt::gray));

    // Create the new tab button
    QToolButton *newTabButton = new QToolButton;
    newTabButton->setText(tr("+"));
    connect(newTabButton, &QPushButton::clicked, [this]() {
        int newIndex = mTabWidget->count() - 1;
        PageWidget *pageWidget = new PageWidget;
        connect(pageWidget, &PageWidget::changed, this, &MainWindow::redraw);
        mTabWidget->insertTab(newIndex, pageWidget, tr("Page"));
        mTabWidget->setCurrentIndex(newIndex);
        toggleTools();
    });

    // Create the tab control
    mTabWidget = new QTabWidget;
    mTabWidget->setTabsClosable(true);
    mTabWidget->addTab(new QWidget, QString());
    mTabWidget->setTabEnabled(0, false);
    mTabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, newTabButton);
    connect(mTabWidget, &QTabWidget::currentChanged, this, &MainWindow::redraw);
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

    // Create the page type combo and populate it
    mPageTypeCombo = new QComboBox;
    for (auto i = gPageTypes.constBegin(); i != gPageTypes.constEnd(); ++i) {
        mPageTypeCombo->addItem(i->name);
    }
    connect(mPageTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::redraw);

    // Create the spacing button
    mSpacing = new QSpinBox;
    mSpacing->setValue(10);
    connect(mSpacing, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::redraw);

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
    toolsLayout->addWidget(new QLabel(tr("Page type:")));
    toolsLayout->addWidget(mPageTypeCombo);
    toolsLayout->addWidget(new QLabel(tr("Spacing:")));
    toolsLayout->addWidget(mSpacing);
    toolsLayout->addStretch(0);
    toolsLayout->addWidget(aboutButton);

    // Create the splitter
    QSplitter *splitter = new QSplitter;
    splitter->addWidget(graphicsView);
    splitter->addWidget(mTabWidget);

    // Create the layout that separates the tabs and buttons
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(splitter);
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
    resize(1024, 480);
    move(QApplication::desktop()->availableGeometry().center() - rect().center());

    // Simulate the addition of a new tab
    newTabButton->click();

    // Redraw the preview
    redraw();
}

void MainWindow::redraw()
{
    // Retrieve the current tab
    QWidget *widget = mTabWidget->currentWidget();
    if (!widget) {
        return;
    }

    // Calculate the page size in pixels at 36dpi
    PageType pageType = gPageTypes.value(mPageTypeCombo->currentIndex());
    QRect pageRect = pageType.size.rectPixels(36);
    if (pageType.orientation == QPrinter::Landscape) {
        pageRect = pageRect.transposed();
    }

    // Create an image of the specified size and render it
    QPixmap pixmap(pageRect.width(), pageRect.height());
    pixmap.fill();

    PageWidget *pageWidget = qobject_cast<PageWidget*>(widget);
    if (pageWidget) {
        pageWidget->draw(
            &pixmap,
            mFont,
            pageRect,
            mSpacing->value()
        );
    }

    // Show the image
    mGraphicsPixmapItem->setPixmap(pixmap);
}

void MainWindow::onPrintClicked()
{
    PageType pageType = gPageTypes.value(mPageTypeCombo->currentIndex());

    // Initialize the printer
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(pageType.size);
    printer.setOrientation(pageType.orientation);

    // Show the print dialog
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {

        // Print each page
        for (int n = 0; n < mTabWidget->count() - 1; ++n) {

            // Have the widget draw the page
            qobject_cast<PageWidget*>(mTabWidget->widget(n))->draw(
                &printer,
                mFont,
                printer.pageRect(),
                mSpacing->value()
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
