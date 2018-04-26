#include "person.h"
#include <QDebug>

Person::Person(QObject *parent) : QObject(parent)
{}

qint32 Person::getLabel() const
{
    return label;
}

void Person::setLabel(qint32 value)
{
    label = value;
}

QString Person::getName() const
{
    return name;
}

void Person::setName(const QString &value)
{
    name = value;
}

QString Person::getGender() const
{
    return gender;
}

void Person::setGender(const QString &value)
{
    gender = value;
}

float Person::getAge() const
{
    return age;
}

void Person::setAge(float value)
{
    age = value;
}

QString Person::getImgPath() const
{
    return imgPath;
}

void Person::setImgPath(const QString &value)
{
    imgPath = value;
}

QString Person::getFaceID() const
{
    return faceID;
}

void Person::setFaceID(const QString &value)
{
    faceID = value;
}

QRect Person::getRect() const
{
    return rect;
}

void Person::setRect(const QRect &value)
{
    rect = value;
}

QDataStream &operator<<(QDataStream &out, const Person *person)
{
    out << person->label;
    out << person->faceID;
    out << person->name;
    out << person->gender;
    out << person->age;
    out << person->imgPath;
    return out;
}

QDataStream &operator>>(QDataStream &in, Person *person)
{   
    in >> person->label;
    in >> person->faceID;
    in >> person->name;
    in >> person->gender;
    in >> person->age;
    in >> person->imgPath;
    return in;
}
/*
 * to create new Person inside this function:
QDataStream &operator>>(QDataStream &in, Person *&person)
{
    person = new Person();
    ...
}*/
