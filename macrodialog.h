#ifndef MACRODIALOG_H
#define MACRODIALOG_H

#include <QDialog>

namespace Ui {
class MacroDialog;
}

class MacroDialog : public QDialog
{
    Q_OBJECT
signals:
    void loadMacros();
public:
    explicit MacroDialog(QWidget *parent = nullptr);
    ~MacroDialog();

private slots:
    void on_saveMacroButton_clicked();
    void on_macroListWidget_clicked(const QModelIndex &index);

    void on_macroListWidget_currentRowChanged(int currentRow);

    void on_macroDialogCloseButton_clicked();

private:
    Ui::MacroDialog *ui;

    void loadSettings();
};

#endif // MACRODIALOG_H
