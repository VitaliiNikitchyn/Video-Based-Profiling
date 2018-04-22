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
    void faceGroup();

signals:
    void detectedFaces(QList<Person*> list);
    void groupedFaces();

public slots:
    void onDetectFaceFinished();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *onFaceDetectReply;
    QNetworkReply *onFaceGroupReply;
};

#endif // FACEAPICLIENT_H
