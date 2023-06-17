/******************************************************************************
** Copyright (C) 2023 Folhium project
** SPDX-License-Identifier: GPL-3.0-or-later
** https://spdx.org/licenses/GPL-3.0-or-later.html
*******************************************************************************
** Copyright (C) 2017 The Qt Company Ltd.
*******************************************************************************
** For more information, see the AUTHORS and CONTRIBUTORS files
******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class QLineEdit;
QT_END_NAMESPACE

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);

    void loadFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void documentWasModified();
    void find();
    void findNext();
    void findPrevious();
    void print();
    void cursorPositionChanged();
    void highlightCurrentLine();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *textEdit;
    QString curFile;
    //
    QLineEdit *findLineEdit;
    //
};

#endif
