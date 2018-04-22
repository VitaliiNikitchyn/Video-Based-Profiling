#include "faceapiclient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "person.h"

#include <QDebug>

FaceApiClient::FaceApiClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);    
}

FaceApiClient::~FaceApiClient()
{
    delete manager;
}

void FaceApiClient::faceDetect(QByteArray requestBody)
{
    QNetworkRequest request(QUrl("https://westcentralus.api.cognitive.microsoft.com/face/v1.0/detect?returnFaceAttributes=age,gender"));
    request.setRawHeader("Ocp-Apim-Subscription-Key", "58dfaa3c653f49e2aeec90aab012114d");
    request.setRawHeader("Content-Type", "application/octet-stream");

    onFaceDetectReply = manager->post(request, requestBody);
    connect(onFaceDetectReply, SIGNAL(finished()), this, SLOT(onDetectFaceFinished()));
}

void FaceApiClient::onDetectFaceFinished()
{
    if (onFaceDetectReply->error()) {
        qDebug() << "reply error" << onFaceDetectReply->errorString();
        qDebug() << onFaceDetectReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
        return;
    }
    QList<Person*> list;    
    QJsonArray jsonArr = QJsonDocument::fromJson(onFaceDetectReply->readAll()).array();
    for (int i = 0; i < jsonArr.size(); i++) {
        QJsonObject obj = jsonArr.at(i).toObject();
        QJsonObject faceRectangle = obj.value("faceRectangle").toObject();
        QJsonObject faceAttribute = obj.value("faceAttributes").toObject();

        QRect rect;
        rect.setTop(faceRectangle.value("top").toInt());
        rect.setLeft(faceRectangle.value("left").toInt());
        rect.setWidth(faceRectangle.value("width").toInt());
        rect.setHeight(faceRectangle.value("height").toInt());

        Person *person = new Person();
        person->setFaceID(obj.value("faceId").toString());
        person->setGender(faceAttribute.value("gender").toString());
        person->setAge(faceAttribute.value("age").toDouble());
        person->setRect(rect);
        list.append(person);        
    }

    emit detectedFaces(list);
    //onFaceDetectReply->deleteLater();
    //onFaceDetectReply = nullptr;
}
