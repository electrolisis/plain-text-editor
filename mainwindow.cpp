/******************************************************************************
** Copyright (C) 2023 Efektivus project
** SPDX-License-Identifier: GPL-3.0-or-later
** https://spdx.org/licenses/GPL-3.0-or-later.html
*******************************************************************************
** Copyright (C) 2017 The Qt Company Ltd.
*******************************************************************************
** For more information, see the AUTHORS and CONTRIBUTORS files
******************************************************************************/

#include <QtWidgets>
//
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif // QT_CONFIG(printdialog)
#include <QPrinter>
#endif // QT_CONFIG(printer)
#endif // QT_PRINTSUPPORT_LIB
//

#include "mainwindow.h"

TextEdit::TextEdit(QWidget *parent)
    : QMainWindow(parent)

{
    textEdit = new QPlainTextEdit(this);

    setCentralWidget(textEdit);

    createActions();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &TextEdit::documentWasModified);

    connect(textEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &TextEdit::cursorPositionChanged);

    connect(textEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &TextEdit::highlightCurrentLine);

#ifndef QT_NO_SESSIONMANAGER
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &TextEdit::commitData);
#endif

    setCurrentFile(QString());
    //
    QIcon windowIcon = QIcon::fromTheme("accessories-text-editor");
    setWindowIcon(windowIcon);
}

void TextEdit::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void TextEdit::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

///TODO improve it
void TextEdit::open()
{
    if (maybeSave()) {
        const QStringList filters({tr("Text files (*.txt)"),
                                   tr("Any file (*)")
                                  });
        QFileDialog fileDialog(this);
        fileDialog.setNameFilters(filters);
        if (fileDialog.exec() != QDialog::Accepted)
            return;
        const QString fileName = fileDialog.selectedFiles().first();

        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool TextEdit::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool TextEdit::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void TextEdit::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
    //
    QString str;
    if(textEdit->document()->isModified()) {
        str = QString(tr("The document is modified"));
    }else{
        str = QString(tr(""));
    }
    statusBar()->showMessage(str, 0);
    //
}

void TextEdit::createActions()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new");
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &TextEdit::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open");
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &TextEdit::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save");
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &TextEdit::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &TextEdit::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);

    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD

    const QIcon cutIcon = QIcon::fromTheme("edit-cut");
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);

    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy");
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste");
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

#endif // !QT_NO_CLIPBOARD

    //
    const QIcon undoIcon = QIcon::fromTheme("edit-undo");
    QAction *undoAct = new QAction(undoIcon, tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo previous change (if any) "));
    connect(undoAct, &QAction::triggered, textEdit, &QPlainTextEdit::undo);
    editMenu->addAction(undoAct);
    editToolBar->addAction(undoAct);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo");
    QAction *redoAct = new QAction(redoIcon, tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo previous change (if any) "));
    connect(redoAct, &QAction::triggered, textEdit, &QPlainTextEdit::redo);
    editMenu->addAction(redoAct);
    editToolBar->addAction(redoAct);

    const QIcon printIcon = QIcon::fromTheme("printer");
    QAction *printAct = new QAction(printIcon, tr("&Print"), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setStatusTip(tr("Print "));
    connect(printAct, &QAction::triggered, this, &TextEdit::print);
    editMenu->addAction(printAct);
    editToolBar->addAction(printAct);

    //
    QToolBar *searchToolBar = addToolBar(tr("Search"));

    findLineEdit = new QLineEdit;
    findLineEdit->setPlaceholderText(tr("Find"));
    findLineEdit->setClearButtonEnabled(true);
    searchToolBar->addWidget(findLineEdit);

    const QIcon findIcon = QIcon::fromTheme("edit-find");
    QAction *findAct = new QAction(findIcon, tr("&Find all"), this);
    findAct->setStatusTip(tr("Find all"));
    connect(findAct, &QAction::triggered, this, &TextEdit::find);
    searchToolBar->addAction(findAct);

    const QIcon findNextIcon = QIcon::fromTheme("arrow-right");
    QAction *findNextAct = new QAction(findNextIcon, tr("&Find next"), this);
    findNextAct->setStatusTip(tr("Find next"));
    findNextAct->setShortcuts(QKeySequence::Find);
    connect(findLineEdit, &QLineEdit::returnPressed, this, &TextEdit::findNext);
    connect(findNextAct, &QAction::triggered, this, &TextEdit::findNext);
    searchToolBar->addAction(findNextAct);

    const QIcon findPreviousIcon = QIcon::fromTheme("arrow-left");
    QAction *findPreviousAct = new QAction(findPreviousIcon, tr("&Find previous"), this);
    findPreviousAct->setStatusTip(tr("Find previous"));
    connect(findPreviousAct, &QAction::triggered, this, &TextEdit::findPrevious);
    searchToolBar->addAction(findPreviousAct);
    //

    menuBar()->addSeparator();

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}

void TextEdit::createStatusBar()
{
    statusBar()->showMessage(tr(""));
    cursorPositionChanged();
}

void TextEdit::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void TextEdit::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool TextEdit::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void TextEdit::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    //statusBar()->showMessage(tr("File loaded"), 2000);
}

bool TextEdit::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void TextEdit::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

///TODO improve it
void TextEdit::find()
{
    QString searchString = findLineEdit->text();
    QString str;

    if (searchString.isEmpty()) {
        str = QString(tr("The search field is empty"));
        statusBar()->showMessage(str, 0);
    } else {
        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(QColor(255, 255, 0, 255));
        selection.format.setForeground(QColor(0,0,0));
        textEdit->extraSelections().clear();

        QTextDocument *document = textEdit->document();
        QTextCursor highlightCursor(document);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document->find(searchString, highlightCursor);
            selection.cursor = highlightCursor;
            extraSelections.append(selection);
        }

        textEdit->setExtraSelections(extraSelections);

        int wordsFound = extraSelections.size() -1;

        if (wordsFound > 0) {
            str = QString(tr("Word(s) found: %1")).arg(wordsFound);
            statusBar()->showMessage(str, 0);
        }else{
                str = QString(tr("Word not found"));
                statusBar()->showMessage(str, 0);
        }
    }

}

void TextEdit::findNext()
{
    QString searchString = findLineEdit->text();
    QString str;

    if (searchString.isEmpty()) {
        str = QString(tr("The search field is empty"));
        statusBar()->showMessage(str, 0);
    } else {
        bool found = textEdit->find(searchString);

        if (found == true) {
            str = QString(tr("Word found"));
            statusBar()->showMessage(str, 0);
        }else{
                str = QString(tr("Word not found"));
                statusBar()->showMessage(str, 0);
        }
    }
}

void TextEdit::findPrevious()
{
    QString searchString = findLineEdit->text();
    QString str;

    if (searchString.isEmpty()) {
        str = QString(tr("The search field is empty"));
        statusBar()->showMessage(str, 0);
    } else {
        bool found = textEdit->find(searchString, QTextDocument::FindBackward);

        if (found == true) {
            str = QString(tr("Word found"));
            statusBar()->showMessage(str, 0);
        }else{
                str = QString(tr("Word not found"));
                statusBar()->showMessage(str, 0);
        }
    }
}
//

//
void TextEdit::print()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
    QPrinter printDev;
#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printDev, this);
    if (dialog.exec() == QDialog::Rejected)
        return;
#endif // QT_CONFIG(printdialog)
    textEdit->print(&printDev);
#endif // QT_CONFIG(printer)
}
//

//
void TextEdit::cursorPositionChanged()
{
    QString str;
    int lineNumber = textEdit->textCursor().blockNumber() + 1;
    int columnNumber = textEdit->textCursor().columnNumber() + 1;
    str = QString(tr("Line: %1, Column: %2")).arg(lineNumber).arg(columnNumber);
    statusBar()->showMessage(str, 0);
}
//

void TextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!textEdit->isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(255, 255, 234, 255);

        selection.format.setBackground(lineColor);
        selection.format.setForeground(QColor(0,0,0));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textEdit->textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    textEdit->setExtraSelections(extraSelections);
}

QString TextEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

#ifndef QT_NO_SESSIONMANAGER
void TextEdit::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (textEdit->document()->isModified())
            save();
    }
}
#endif
