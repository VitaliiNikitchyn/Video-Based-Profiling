#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "person.h"
#include "persondetailsdialog.h"
#include "faceapiclient.h"

#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);        
    faceApi = new FaceApiClient(this);
    connect(faceApi, SIGNAL(detectedFaces(QList<Person*>)), this, SLOT(onDetectedFaces(QList<Person*>)));

    if (!face_cascade.load("./haarcascade_frontalface_alt.xml")) {
        qDebug() << "-(!)Error loading face cascade";
    }

    capture.open(0);
    //set height and width of capture frame
    capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

    recognizer = cv::face::LBPHFaceRecognizer::create();
    //recognizer = cv::face::FisherFaceRecognizer::create();
    //recognizer->read("./recognizerModel");
    init_trainingset();
    recognizer->train(images, labels);

    timer = new QTimer(this);
    timer->setInterval(40);
    connect(timer, SIGNAL(timeout()), this, SLOT(processFrame()));

    ui->listWidget->setIconSize(QSize(75, 75));

/*
    //load from file list of persons
    Person *person1 = new Person();
    person1->setName("Vitalii");
    person1->setAge(22.5);
    person1->setGender("male");
    person1->setLabel(1);
    person1->setFacePath("./images/main.png");

    Person *person2 = new Person();
    person2->setName("Max");
    person2->setAge(20.5);
    person2->setGender("male");
    //person2->setLabel(2);
    person2->setFacePath("./training_set/max/f1.png");

    QList<Person*> list;
    list.append(person1);
    list.append(person2);
    Person::addPerson(list);*/

    personList = Person::loadPerson();
    for (int i = 0; i < personList.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(personList[i]->getName());
        item->setIcon(QIcon(personList[i]->getImgPath()));
        item->setData(Qt::UserRole, QVariant::fromValue(personList[i]));
        ui->listWidget->addItem(item);
    }
}


MainWindow::~MainWindow()
{
    //recognizer->save("./recognizerModel");
    //faceApi->deleteLater();
    delete ui;
}
/*
void MainWindow::closeEvent()
{
    capture.~VideoCapture();
    cv::destroyAllWindows();
}*/


void MainWindow::init_trainingset()
{
    QFile file("./Trainingsetpath.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "error openning Trainingsetpath";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> list = line.split(';');

        std::string path = list.at(0).toUtf8().constData();
        cv::Mat img = cv::imread(path, 0);
        //cv::resize(img, img, cv::Size(80,80));
        int label = list.at(1).toInt();

        images.push_back(img);
        labels.push_back(label);
    }
}

void MainWindow::on_start_video_btn_clicked()
{    
    if(!timer->isActive()) {
        timer->start();
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(timer->isActive()) {
        timer->stop();
        cv::destroyWindow("face_recognizer");
    }
}


void MainWindow::on_addPersonBtn_clicked()
{    
    //cv::Mat img;
    capture.read(img);
    //convert from cv::Mat to QByteArray
    QImage qImage = QImage((uchar*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).copy();
    QByteArray binaryData;
    QBuffer buffer(&binaryData);
    buffer.open(QIODevice::WriteOnly);
    qImage.save(&buffer, "PNG");

    faceApi->faceDetect(binaryData);

    //cv::imshow("screenshot", img);
    //cv::imshow("face", cameraFrame(faces[0]));
}

void MainWindow::processFrame()
{
    cv::Mat cameraFrame;
    capture.read(cameraFrame);
    cv::Mat img_grey;
    std::vector<cv::Rect> faces;

    cv::cvtColor(cameraFrame, img_grey, CV_BGR2GRAY);
    cv::equalizeHist(img_grey, img_grey);
    face_cascade.detectMultiScale(img_grey, faces);

    for(size_t i = 0; i < faces.size(); i++) {
        // Crop the face from the image.
        cv::Mat face = img_grey(faces[i]);
        //cv::Mat face_resized;
        //cv::resize(face, face_resized, cv::Size(80, 80), 1.0, 1.0, cv::INTER_CUBIC);

        int predictedLabel = -1;
        double confidence = 0.0;
        recognizer->predict(face, predictedLabel, confidence);

        std::string box_text = std::to_string(confidence) + " " + std::to_string(predictedLabel);
        // Calculate the position for annotated text
        int pos_x = std::max(faces[i].tl().x - 10, 0);
        int pos_y = std::max(faces[i].tl().y - 10, 0);
        // And now put it into the image:
        cv::putText(cameraFrame, box_text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
        cv::rectangle(cameraFrame, faces[i], CV_RGB(0, 255,0), 1); //rect around a face
    }
    cv::imshow("face_recognizer", cameraFrame);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //get associated data from QListWidgetItem stored in QVariant
    Person *person = item->data(Qt::UserRole).value<Person*>();
    PersonDetailsDialog dialog(person, this);
    //dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog.exec();
    //update item
}

void MainWindow::onDetectedFaces(QList<Person*> list)
{
    for (int i = 0; i < list.size(); i++) {
        personList.append(person);
        Person *person = list[i];
        cv::Rect rect(person->getRect().x(), person->getRect().y(), person->getRect().width(), person->getRect().height());
        cv::rectangle(img, rect, cv::Scalar(0, 255, 0));        

        cv::Mat face = img(rect);        
        //save Mat face to localFaceDB
        //person.setImgPath from previous step
        //add to listWidget

        //from global list get all faceIDs and post to faceAPI

        //cv::imshow("screenshot" + std::to_string(i), face);        
        qDebug() << "New person: gender: " << person->getGender() << "  age: " << person->getAge();
    }
    cv::imshow("screenshot", img);

    //qDeleteAll(list); //remove all objects from memory
    list.clear();
}

