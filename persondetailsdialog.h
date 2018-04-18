#ifndef PERSONDETAILSDIALOG_H
#define PERSONDETAILSDIALOG_H

#include <QDialog>

namespace Ui {
class PersonDetailsDialog;
}

class Person;
class PersonDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PersonDetailsDialog(Person *person, QWidget *parent = 0);
    ~PersonDetailsDialog();

private slots:
    void on_saveBtn_clicked();

private:
    Ui::PersonDetailsDialog *ui;
    Person *person;
};

#endif // PERSONDETAILSDIALOG_H
