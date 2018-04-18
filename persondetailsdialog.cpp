#include "persondetailsdialog.h"
#include "ui_persondetailsdialog.h"
#include "person.h"

PersonDetailsDialog::PersonDetailsDialog(Person *person, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PersonDetailsDialog)
{
    ui->setupUi(this);
    ui->imageLabel->setPixmap(QPixmap(person->getFacePath()).scaled(135, 150));
    ui->nameLineEdit->setText(person->getName());
    ui->ageSpinBox->setValue(person->getAge());
    ui->genderLineEdit->setText(person->getGender());

    this->person = person;
}

PersonDetailsDialog::~PersonDetailsDialog()
{
    delete ui;
}

void PersonDetailsDialog::on_saveBtn_clicked()
{
    person->setName(ui->nameLineEdit->text());
    person->setAge(ui->ageSpinBox->value());
}
