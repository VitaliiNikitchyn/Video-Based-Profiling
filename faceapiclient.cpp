#include "faceapiclient.h"
#include "person.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>
#include <QHashIterator>

FaceApiClient::FaceApiClient(QObject *parent) : QObject(parent)
{
    faceDetectmanager = new QNetworkAccessManager(this);
    faceGroupManager = new QNetworkAccessManager(this);

    connect(faceDetectmanager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(onDetectFaceFinished(QNetworkReply *)));
    connect(faceGroupManager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(onGroupFaceFinished(QNetworkReply *)));
}

FaceApiClient::~FaceApiClient()
{
    faceDetectmanager->deleteLater();
    faceGroupManager->deleteLater();
}

void FaceApiClient::faceDetect(QByteArray requestBody)
{    
    QNetworkRequest request(QUrl("https://westcentralus.api.cognitive.microsoft.com/face/v1.0/detect?returnFaceAttributes=age,gender"));
    request.setRawHeader("Ocp-Apim-Subscription-Key", "58dfaa3c653f49e2aeec90aab012114d");
    request.setRawHeader("Content-Type", "application/octet-stream");
    faceDetectmanager->post(request, requestBody);
}

void FaceApiClient::faceGroup(QHash<QString, qint32> allPersonHash, int availableLabel)
{
    QJsonArray arr;
    QList<QString> faceIDList = allPersonHash.keys();
    for (int i = 0; i < faceIDList.size(); i++) {
        arr.append(faceIDList.at(i));
    }
    QJsonObject obj{ {"faceIds", arr} };

    QNetworkRequest request(QUrl("https://westcentralus.api.cognitive.microsoft.com/face/v1.0/group"));
    request.setRawHeader("Ocp-Apim-Subscription-Key", "58dfaa3c653f49e2aeec90aab012114d");
    request.setRawHeader("Content-Type", "application/json");
    faceGroupManager->post(request, QJsonDocument(obj).toJson());

    this->allPersonHash = allPersonHash;
    this->availableLabel = availableLabel;
}

void FaceApiClient::onDetectFaceFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "reply error" << reply->errorString();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
        //TO DO: emit replyError signal
        return;
    }

    newPersonHash.clear();
    QList<Person*> newPersonList;
    QJsonArray jsonArr = QJsonDocument::fromJson(reply->readAll()).array();
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
        person->setLabel(-1); //undefined label
        person->setRect(rect);
        newPersonList.append(person);
        newPersonHash.insert(person->getFaceID(), person);
    }

    emit faceDetected(newPersonList);
    reply->deleteLater();
    reply = nullptr;
}

void FaceApiClient::onGroupFaceFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "onGroupFaceFinished reply error" << reply->errorString();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
        qDebug() << reply->readAll();
        return;
        //TO DO: emit replyError signal
    }
    QJsonObject jsonObj = QJsonDocument::fromJson(reply->readAll()).object();
    QJsonArray groups = jsonObj.value("groups").toArray();
    QJsonArray messyGroup = jsonObj.value("messyGroup").toArray();
    QList<int> addedLabels;

    for (int i = 0; i < groups.size(); i++) {
        qint32 label = -1;
        QList<QString> subgroup;
        QJsonArray group = groups.at(i).toArray();
        for (int j = 0; j < group.size(); j++) {
            QString faceID = group.at(j).toString();
            if (newPersonHash.contains(faceID)) {
                subgroup.append(faceID);
            } else if (label == -1) {
                label = allPersonHash.value(faceID);
            }
        }

        if (label == -1) { //similar not found in allPersonHash
            label = availableLabel;
            addedLabels.append(label);
            availableLabel++;
        }

        for (int j = 0; j < subgroup.size(); j++) {
            Person *person = newPersonHash.value(subgroup.at(j));
            person->setLabel(label);
        }
    }

    for (int i = 0; i < messyGroup.size(); i++) {
        QString faceID = messyGroup.at(i).toString();
        if (newPersonHash.contains(faceID)) {
            Person *person = newPersonHash.value(faceID);
            person->setLabel(availableLabel);
            addedLabels.append(availableLabel);
            availableLabel++;
        }
    }

    QHashIterator<QString, Person*> iter(newPersonHash);
    while(iter.hasNext()) {
        iter.next();
        qDebug() << "group: " << iter.key() << ": " << iter.value()->getLabel();
    }

    emit groupedFaces(newPersonHash.values(), addedLabels);
    reply->deleteLater();
    reply = nullptr;
}
