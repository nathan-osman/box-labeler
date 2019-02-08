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

#include <QFont>
#include <QHBoxLayout>

#include "printtask.h"
#include "queuewidget.h"

QueueWidget::QueueWidget()
    : mStatusLabel(new QLabel),
      mQueueLength(0)
{
    // Initialize the label
    QLabel *label = new QLabel(tr("Status:"));
    label->setStyleSheet("font-weight: bold;");

    // Create the layout
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setMargin(0);
    hboxLayout->addWidget(label);
    hboxLayout->addWidget(mStatusLabel, 1);
    setLayout(hboxLayout);

    // Start the thread
    mThread.start();

    // Update the label
    updateLabel();
}

QueueWidget::~QueueWidget()
{
    mThread.quit();
    mThread.wait();
}

void QueueWidget::addTask(PrintTask *task)
{
    // Update the queue length
    ++mQueueLength;
    updateLabel();

    task->moveToThread(&mThread);
    connect(task, &PrintTask::finished, this, [this, task]() {
        delete task;
        --mQueueLength;
        updateLabel();
    });
    QMetaObject::invokeMethod(task, &PrintTask::print, Qt::QueuedConnection);
}

void QueueWidget::updateLabel()
{
    mStatusLabel->setText(
        mQueueLength ? tr("%1 in queue").arg(mQueueLength) : tr("idle")
    );
}

