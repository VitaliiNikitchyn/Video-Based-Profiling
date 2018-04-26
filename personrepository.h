#ifndef PERSONREPOSITORY_H
#define PERSONREPOSITORY_H

#include <QObject>

class Person;
class PersonRepository : public QObject
{
    Q_OBJECT
public:
    explicit PersonRepository(QObject *parent = nullptr);
    void save(QString filePath);
    void load(QString filePath);

    QList<Person*> getAll();
    QHash<QString, qint32> getAllIdAndLabel();
    void create(Person *p);
    QString getNameByLabel(int label);

    int getAvailableLabel() const;
    void setAvailableLabel(int value);

private:
    QList<Person*> personList;
    int availableLabel;

signals:

public slots:
};

#endif // PERSONREPOSITORY_H
