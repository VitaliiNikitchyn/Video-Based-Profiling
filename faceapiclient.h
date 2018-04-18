#ifndef FACEAPICLIENT_H
#define FACEAPICLIENT_H

#include <QObject>

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

signals:
    void detectedFaces(QList<Person*> list);

public slots:
    void onDetectFaceFinished();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
};

#endif // FACEAPICLIENT_H
