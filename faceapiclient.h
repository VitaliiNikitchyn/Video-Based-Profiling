#ifndef FACEAPICLIENT_H
#define FACEAPICLIENT_H

#include <QObject>
#include <QHash>

class QNetworkAccessManager;
class QNetworkReply;
class Person;
class FaceApiClient : public QObject
{
    Q_OBJECT
public:
    explicit FaceApiClient(QObject *parent = nullptr);
    ~FaceApiClient();
    void faceDetect(QByteArray requestBody);
    void faceGroup(QHash<QString, qint32> allPersonHash, int availableLabel);

signals:
    void faceDetected(QList<Person*> list);
    void groupedFaces(QList<Person*>, QList<int> addedLabels);

public slots:
    void onDetectFaceFinished(QNetworkReply *reply);
    void onGroupFaceFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *faceDetectmanager;
    QNetworkAccessManager *faceGroupManager;

    QHash<QString, Person*> newPersonHash;
    QHash<QString, qint32> allPersonHash;
    int availableLabel;
};

#endif // FACEAPICLIENT_H
