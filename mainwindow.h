#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>


namespace Ui {
class MainWindow;
}

class QTimer;
class QListWidgetItem;
class FaceApiClient;
class Person;
class PersonRepository;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_start_video_btn_clicked();

    void on_pushButton_clicked();

    void on_addPersonBtn_clicked();

    void processFrame();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void onFaceDetected(QList<Person*> list);
    void onFaceGrouped(QList<Person *> list, QList<int> addedLebels);

private:
    Ui::MainWindow *ui;
    const int FRAME_WIDTH = 500;//640; //480
    const int FRAME_HEIGHT = 340;//480; //320
    const QString personRepositoryPath = "./person.txt";

    cv::Ptr<cv::face::FaceRecognizer> recognizer;
    void init_recognizer();

    cv::CascadeClassifier face_cascade;
    cv::VideoCapture capture;
    QTimer *timer;
    FaceApiClient *faceApi;
    PersonRepository *personRepository;

    void addPersonToListWidget(Person* person);

    cv::Mat img;
};

#endif // MAINWINDOW_H
