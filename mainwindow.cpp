#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "person.h"
#include "persondetailsdialog.h"
#include "faceapiclient.h"

#include <QDebug>
#include <QFile>
#include <QTimer>

#include <QBuffer>
//#include <QCloseEvent>


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

    //recognizer = cv::face::LBPHFaceRecognizer::create();
    recognizer = cv::face::FisherFaceRecognizer::create();
    recognizer->read("./recognizerModel");
    //init_trainingset();
    //recognizer->train(images, labels);

    imageTimer = new QTimer(this);
    imageTimer->setInterval(20);
    connect(imageTimer, SIGNAL(timeout()), this, SLOT(processFrame()));


    ui->listWidget->setIconSize(QSize(75, 75));


    //load from file list of persons
    Person *person1 = new Person();
    person1->setName("Vitalii");
    person1->setAge(22.2);
    person1->setGender("male");
    person1->setLabel(1);
    person1->setFacePath("./images/main.png");

    Person *person2 = new Person();
    //person2->setName("Max");
    person2->setAge(20.5);
    person2->setGender("male");
    person2->setLabel(2);
    person2->setFacePath("./training_set/max/f1.png");

    QList<Person*> list;
    list.append(person1);
    list.append(person2);

    for (int i = 0; i < list.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(list[i]->getName());
        item->setIcon(QIcon(list[i]->getFacePath()));
        item->setData(Qt::UserRole, QVariant::fromValue(list[i]));
        ui->listWidget->addItem(item);
    }
}


MainWindow::~MainWindow()
{
    //recognizer->save("./recognizerModel");
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
        cv::resize(img, img, cv::Size(80,80));
        int label = list.at(1).toInt();

        images.push_back(img);
        labels.push_back(label);
    }
}

std::string MainWindow::composeName(double confidence, int predictedLabel)
{
    std::string res = "";
    if (confidence < 350) {
        res = std::to_string(confidence) + " " + std::to_string(predictedLabel);
    } else {
        res = "Unrecognized";
    }
    return res;
}

void MainWindow::on_start_video_btn_clicked()
{    
    if(!imageTimer->isActive()) {
        imageTimer->start();
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(imageTimer->isActive()) {
        imageTimer->stop();
        cv::destroyWindow("face_recognizer");
    }
}


void MainWindow::on_addPersonBtn_clicked()
{    
    //cv::Mat img;
    capture.read(img);
    //cv::cvtColor(cameraFrame, cameraFrame, CV_BGR2RGB);
    //int size = img.total() * img.elemSize();
    //QByteArray arr((const char*) img.data, size);

    //convert from cv::Mat to QByteArray
    QImage qImage = QImage((uchar*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).copy();
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    qImage.save(&buffer, "PNG");

    faceApi->faceDetect(arr);

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

        cv::Mat face_resized;
        cv::resize(face, face_resized, cv::Size(80, 80), 1.0, 1.0, cv::INTER_CUBIC);

        int predictedLabel = -1;
        double confidence = 0.0;
        recognizer->predict(face_resized, predictedLabel, confidence);

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
    PersonDetailsDialog *dialog = new PersonDetailsDialog(person, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    //update item
}

void MainWindow::onDetectedFaces(QList<Person*> list)
{
    for (int i = 0; i < list.size(); i++) {
        Person *person = list[i];
        cv::Rect rect(person->getRect().x(), person->getRect().y(), person->getRect().width(), person->getRect().height());
        cv::Mat face = img(rect);
        cv::imshow("screenshot" + std::to_string(i), face);
    }

    qDeleteAll(list); //remove all objects from memory
    list.clear();
}
