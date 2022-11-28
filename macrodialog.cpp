#include "macrodialog.h"
#include "ui_macrodialog.h"

#include <QModelIndex>
#include <QSettings>

MacroDialog::MacroDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MacroDialog)
{
    ui->setupUi(this);
}

MacroDialog::~MacroDialog()
{
    delete ui;
}

void MacroDialog::on_macroListWidget_clicked(const QModelIndex &index)
{
    QSettings s("RPiKeyerTerm.ini", QSettings::IniFormat);
    int macnum = index.row();
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
    int row = ui->macroListWidget->currentRow();
    const QString item = it->text();
    //qDebug()<<"Item:"<<item;
    s.setValue("Macros/macro" + QString::number(row) + "Text", ui->macroTextEdit->toPlainText());
    s.setValue("Macros/macro" + QString::number(row) + "Label", ui->macroLabelLineEdit->text());
    emit loadMacros();
}
