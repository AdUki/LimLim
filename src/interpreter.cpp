#include "interpreter.h"
#include "luacontrol.h"
#include "source.h"
#include "editor.h"
#include "console.h"
#include "global.h"
#include "ui_interpreter.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>

Interpreter::Interpreter(Console* console, QWidget *parent)
    : QDialog(parent), ui(new Ui::InterpreterForm)
{
    ui->setupUi(this);

    this->console = console;
    process = new QProcess(this);
    options.clear();

    ui->argsListView->setModel(&args);

    // TODO load lua path via conf file
    ui->luaPathEdit->setText("lua");

    connect(console, SIGNAL(emitInput(QByteArray)), this, SLOT(writeInput(QByteArray)));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(atFinish(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(started()), this, SLOT(atStart()));
}

Interpreter::~Interpreter()
{
    delete ui;
}

void Interpreter::debug(Source* source)
{
    if (source == NULL) return;

    QString remdebugPath = QString(APP_DIR_PATH).append("?.lua");
#ifdef Q_WS_WIN
    remdebugPath.replace('\\', "\\\\");
#endif
    QString query = QString("package.path = '").append(remdebugPath).append(";' .. package.path");
    options << "-e" << query;
    options << "-e" << "require 'remdebug.engine'.start()";

    run(source);
}

bool Interpreter::run(Source* source)
{
    // get file name from configuration
    if (!ui->updateCheckBox->isChecked()) {
        return runFile(ui->execPathEdit->text());
    }
    // if source NULL return
    if (source == NULL) return false;

    if (source->doesExist()) {
        fileName = source->getFileName();
        execute();
    } else {
        if (tempFile.open()) {
            tempFile.resize(0); // truncate file
            if (tempFile.write(source->text().toAscii()) == -1) {
                // failed to create temp file
                return false;
            }
            tempFile.close();
            fileName = tempFile.fileName();
            source->setTempFileName(fileName);
            execute();
        }
    }
    return true;
}

bool Interpreter::runFile(const QString &file)
{
    if (QFileInfo(file).exists()) {
        fileName = file;
        execute();
        return true;
    } else {
        QMessageBox::critical(this,
            tr("Error!"),
            tr("Could not open file:").append('\n').append(file),
            QMessageBox::Ok);
        return false;
    }
}


// TODO add lua 5.0 and 5.2 support
void Interpreter::execute()
{
    // TODO platform specific, lua mustn't be found

    console->open();
    console->writeSystem(QString("Starting program ")
        .append(fileName).append('\n'));

    options << "-e" << "io.stdout:setvbuf 'no'";
    process->start(luaPath, options << "--" << fileName << args.stringList());
}

void Interpreter::atFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    console->writeSystem(QString("Program exited whit code ")
        .append(QString::number(exitCode)).append('\n'));
    console->close();
    options.clear();
    if (ui->resetCheckBox->isChecked()) clearArgs();
    emit changedRunningState(false);
    emit finished();
}

void Interpreter::atStart()
{
    emit started();
    emit changedRunningState(true);
}

void Interpreter::addArgs(const QStringList& args)
{
    if (args.isEmpty()) return;
    this->args.setStringList(this->args.stringList() << args);
}

void Interpreter::addArg(const QString &arg)
{
    if (arg.isEmpty()) return;
    this->args.setStringList(args.stringList() << arg);
}

void Interpreter::clearArgs()
{
    args.setStringList(QStringList());
}

void Interpreter::addOptions(const QStringList &options)
{
    if (options.isEmpty()) return;
    this->options.append(options);
}

void Interpreter::addOption(const QString &option)
{
    if (option.isEmpty()) return;
    this->options.append(option);
}

void Interpreter::clearOptions()
{
    options.clear();
}

void Interpreter::kill()
{
    process->kill();
}

void Interpreter::writeInput(const QByteArray& input)
{
    process->write(input);
}

void Interpreter::readStandardOutput()
{
    console->writeOutput(process->readAllStandardOutput());
}

void Interpreter::readStandardError()
{
    console->writeError(process->readAllStandardError());
}

void Interpreter::on_luaPathButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    // TODO filter only executable files
    //dialog.setFilter(QDir::Executable);
    dialog.setModal(true);
    dialog.setWindowTitle(tr("Select Executable"));


    QString fileName;
    if (dialog.exec()) {
        fileName = dialog.selectedFiles().first();
        ui->luaPathEdit->setText(fileName);
    }
}

void Interpreter::on_execPathButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
            this, tr("Open Lua source"), "", tr("Lua Files (*.lua)"));
    if (!fileName.isEmpty()) ui->execPathEdit->setText(fileName);
}

void Interpreter::on_addArgButton_clicked()
{
    addArg(tr("New argument"));
    QModelIndex newArg = args.index(args.rowCount()-1,0);
    ui->argsListView->selectionModel()->select(newArg, QItemSelectionModel::Select);
    ui->argsListView->edit(newArg);
}

void Interpreter::on_delArgButton_clicked()
{
    if (args.rowCount() == 0) return;
    foreach (QModelIndex index,
             ui->argsListView->selectionModel()->selectedIndexes()) {
        args.removeRow(index.row(), index.parent());
    }
}

void Interpreter::on_luaPathEdit_textChanged(QString newPath)
{
    // TODO check if path valid
    luaPath = newPath;
}
