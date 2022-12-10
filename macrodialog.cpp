#include "macrodialog.h"
#include "ui_macrodialog.h"

#include <QModelIndex>
#include <QSettings>

MacroDialog::MacroDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacroDialog)
{
    ui->setupUi(this);
    loadSettings();
}

MacroDialog::~MacroDialog()
{
    delete ui;
}

void MacroDialog::loadSettings()
{
    QSettings s("RPiKeyerTerm.ini", QSettings::IniFormat);
    QString label;
    for(int i = 1; i < 13; i++) {
         label = s.value("Macros/macro" + QString::number(i) + "Label").toString();
         if(label.isEmpty())
             label = "MAC" + QString::number(i);
         QListWidgetItem *item = ui->macroListWidget->item(i - 1);
         item->setText(label);
    }
}

void MacroDialog::on_macroListWidget_clicked(const QModelIndex &index)
{
    QSettings s("RPiKeyerTerm.ini", QSettings::IniFormat);
    int macnum = index.row() + 1;
    const QString text = s.value("Macros/macro" + QString::number(macnum) + "Text").toString();
    const QString label = s.value("Macros/macro" + QString::number(macnum) + "Label").toString();
    ui->macroTextEdit->setPlainText(text);
    ui->macroLabelLineEdit->setText(label);
    ui->saveMacroButton->setEnabled(true);
}

void MacroDialog::on_saveMacroButton_clicked()
{
    QSettings s("RPiKeyerTerm.ini", QSettings::IniFormat);
    QListWidgetItem *it = ui->macroListWidget->currentItem();
    int row = ui->macroListWidget->currentRow() + 1;
    const QString item = ui->macroLabelLineEdit->text();
    //qDebug()<<"Item:"<<item;
    s.setValue("Macros/macro" + QString::number(row) + "Text", ui->macroTextEdit->toPlainText());
    s.setValue("Macros/macro" + QString::number(row) + "Label", item);
    it->setText(item);
    emit loadMacros();
}


void MacroDialog::on_macroListWidget_currentRowChanged(int currentRow)
{
    QSettings s("RPiKeyerTerm.ini", QSettings::IniFormat);
    int macnum = currentRow + 1;
    const QString text = s.value("Macros/macro" + QString::number(macnum) + "Text").toString();
    const QString label = s.value("Macros/macro" + QString::number(macnum) + "Label").toString();
    ui->macroTextEdit->setPlainText(text);
    ui->macroLabelLineEdit->setText(label);
    ui->saveMacroButton->setEnabled(true);
}

void MacroDialog::on_macroDialogCloseButton_clicked()
{
    close();
}
