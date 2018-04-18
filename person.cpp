#include "person.h"

Person::Person(QObject *parent) : QObject(parent)
{

}

int Person::getLabel() const
{
    return label;
}

void Person::setLabel(int value)
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

QString Person::getFacePath() const
{
    return facePath;
}

void Person::setFacePath(const QString &value)
{
    facePath = value;
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
