#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "person.h"
#include "personrepository.h"
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

    personRepository = new PersonRepository();
    personRepository->load(personRepositoryPath);

    faceApi = new FaceApiClient(this);
    connect(faceApi, SIGNAL(faceDetected(QList<Person*>)), this, SLOT(onFaceDetected(QList<Person*>)));
    connect(faceApi, SIGNAL(groupedFaces(QList<Person*>, QList<int>)), this, SLOT(onFaceGrouped(QList<Person*>, QList<int>)));

    if (!face_cascade.load("./haarcascade_frontalface_alt.xml")) {
        qDebug() << "-(!)Error loading face cascade";
    }

    capture.open(0);
    //set height and width of capture frame
    capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

    init_recognizer();

    timer = new QTimer(this);
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(processFrame()));


    //load from file list of persons
    /*
    Person *person1 = new Person();
    person1->setName("Vitalii");
    person1->setAge(22.5);
    person1->setGender("male");
    person1->setLabel(0);
    person1->setFaceID("c834ea1d-11ba-40cf-8327-3dc0af43dd61");
    person1->setImgPath("./images/main.png");
    personRepository->create(person1);
    personRepository->save(personRepositoryPath);
*/

    QList<Person*> personList = personRepository->getAll();
    ui->listWidget->setIconSize(QSize(75, 75));
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

void MainWindow::init_recognizer()
{
    QFile file("./Trainingsetpath.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "error openning Trainingsetpath";
        return;
    }
    std::vector<cv::Mat> images;
    std::vector<int> labels;

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
    recognizer = cv::face::LBPHFaceRecognizer::create();
    //recognizer = cv::face::FisherFaceRecognizer::create();
    //recognizer->read("./recognizerModel");
    recognizer->train(images, labels);
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
    ui->addPersonBtn->setEnabled(false);
    //cv::Mat img;
    capture.read(img);
    //convert from cv::Mat to QByteArray
    QImage qImage = QImage((uchar*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).copy();
    QByteArray binaryData;
    QBuffer buffer(&binaryData);
    buffer.open(QIODevice::WriteOnly);
    qImage.save(&buffer, "PNG");

    faceApi->faceDetect(binaryData);
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

        // Calculate the position for annotated text
        int pos_x = std::max(faces[i].tl().x - 10, 0);
        int pos_y = std::max(faces[i].tl().y - 10, 0);
        std::string box_text;

        int predictedLabel = -1;
        double confidence = 0.0;
        recognizer->predict(face, predictedLabel, confidence);

        if (confidence < 112) {
            //box_text = std::to_string(confidence) + " " + std::to_string(predictedLabel);
            box_text = personRepository->getNameByLabel(predictedLabel).toStdString();
            cv::putText(cameraFrame, box_text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
        } else {
            box_text = "Unrecognized";
            cv::putText(cameraFrame, box_text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,0,0), 2.0);
        }
        cv::rectangle(cameraFrame, faces[i], CV_RGB(0, 255,0), 1); //rect around a face
    }
    cv::imshow("face_recognizer", cameraFrame);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //get associated data from QListWidgetItem stored in QVariant
    Person *person = item->data(Qt::UserRole).value<Person*>();
    PersonDetailsDialog dialog(person, item, this);
    //dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog.exec();
    //update item
}

void MainWindow::onFaceDetected(QList<Person*> list)
{          
    for (int i = 0; i < list.size(); i++) {
        Person *person = list[i];
        personRepository->create(person);
        cv::Rect rect(person->getRect().x(), person->getRect().y(), person->getRect().width(), person->getRect().height());
        cv::rectangle(img, rect, cv::Scalar(0, 255, 0));
        qDebug() << "New: gender: " << person->getGender() << " age: " << person->getAge() << "id: " << person->getFaceID();
    }
    cv::imshow("screenshot", img);
    faceApi->faceGroup(personRepository->getAllIdAndLabel(), personRepository->getAvailableLabel());
}

void MainWindow::onFaceGrouped(QList<Person*> list, QList<int> addedLebels)
{
    ui->addPersonBtn->setEnabled(true);
    personRepository->setAvailableLabel(personRepository->getAvailableLabel() + addedLebels.size());

    std::vector<cv::Mat> images;
    std::vector<int> labels;
    for (int i = 0; i < list.size(); i++) {
        Person *person = list.at(i);
        cv::Rect rect(person->getRect().x(), person->getRect().y(), person->getRect().width(), person->getRect().height());
        cv::Mat face = img(rect);
        int trainNumb = 6;
        //save face img
        if (addedLebels.contains(person->getLabel())) {
            QString imgPath = "./Person/" + person->getFaceID() + ".jpg";
            person->setImgPath(imgPath);
            cv::imwrite(imgPath.toStdString(), face);

            //add to listWidget
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(person->getName());
            item->setIcon(QIcon(person->getImgPath()));
            item->setData(Qt::UserRole, QVariant::fromValue(person));
            ui->listWidget->addItem(item);
            trainNumb = 1;
        }
        cv::cvtColor(face, face, CV_RGB2GRAY);
        for (int j = 0; j < trainNumb; j++) {
            images.push_back(face);
            labels.push_back(person->getLabel());
        }
    }
    recognizer->update(images, labels);
}
