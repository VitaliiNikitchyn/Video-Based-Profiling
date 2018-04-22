#ifndef PERSONE_H
#define PERSONE_H

#include <QObject>
#include <QRect>
#include <QDataStream>

class Person : public QObject
{
    Q_OBJECT
public:
    explicit Person(QObject *parent = nullptr);


    qint32 getLabel() const;
    void setLabel(qint32 value);

    QString getName() const;
    void setName(const QString &value);

    QString getGender() const;
    void setGender(const QString &value);

    float getAge() const;
    void setAge(float value);

    QString getImgPath() const;
    void setImgPath(const QString &value);

    QString getFaceID() const;
    void setFaceID(const QString &value);

    QRect getRect() const;
    void setRect(const QRect &value);

    friend QDataStream &operator<<(QDataStream &out, const Person *person);
    friend QDataStream &operator>>(QDataStream &in, Person *person);

    static void addPerson(QList<Person*> list);
    static QList<Person*> loadPerson();

private:
    qint32 label;
    QString faceID;
    QString name;
    QString gender;
    double age;
    QString imgPath;
    QRect rect;


signals:

public slots:
};

#endif // PERSONE_H
