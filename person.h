#ifndef PERSONE_H
#define PERSONE_H

#include <QObject>
#include <QRect>

class Person : public QObject
{
    Q_OBJECT
public:
    explicit Person(QObject *parent = nullptr);

    int getLabel() const;
    void setLabel(int value);

    QString getName() const;
    void setName(const QString &value);

    QString getGender() const;
    void setGender(const QString &value);

    float getAge() const;
    void setAge(float value);

    QString getFacePath() const;
    void setFacePath(const QString &value);

    QString getFaceID() const;
    void setFaceID(const QString &value);

    QRect getRect() const;
    void setRect(const QRect &value);

private:
    int label;
    QString faceID;
    QString name;
    QString gender;
    double age;
    QString facePath;
    QRect rect;

signals:

public slots:
};

#endif // PERSONE_H
