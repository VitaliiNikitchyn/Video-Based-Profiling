#include "personrepository.h"
#include "person.h"
#include <QDataStream>
#include <QFile>
#include <QHash>

PersonRepository::PersonRepository(QObject *parent) : QObject(parent)
{
    availableLabel = 0;
}

QList<Person *> PersonRepository::getAll()
{
    return this->personList;
}

QHash<QString, qint32> PersonRepository::getAllIdAndLabel()
{
    QHash<QString, qint32> qHash;
    for (int i = 0; i < personList.size(); i++) {
        qHash.insert(personList.at(i)->getFaceID(), personList.at(i)->getLabel());
    }
    return qHash;
}

void PersonRepository::create(Person *p)
{
    this->personList.append(p);
}

QString PersonRepository::getNameByLabel(int label)
{
    QString name;
    for (int i = 0; i < personList.size(); i++)
        if (personList.at(i)->getLabel() == label)
            name = personList.at(i)->getName();
    return name;
    //return personList.at(label)->getName();
}

int PersonRepository::getAvailableLabel() const
{
    return availableLabel;
}

void PersonRepository::setAvailableLabel(int value)
{
    availableLabel = value;
}

void PersonRepository::save(QString filePath)
{
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_10);

    for (int i = 0; i < personList.size(); i++) {
        out << personList[i];
    }
    file.flush();
    file.close();
}

void PersonRepository::load(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug("cannot load persons. Error file opening");
        return;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_10);
    while (!file.atEnd()) {
        Person *person = new Person();
        in >> person;
        personList.append(person);

        //calculate next available label for FaceRecognizer
        if (person->getLabel() == availableLabel) {
            availableLabel++;
        }
    }
    file.close();
}
