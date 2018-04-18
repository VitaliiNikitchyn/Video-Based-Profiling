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

    void onDetectedFaces(QList<Person*> list);

private:
    Ui::MainWindow *ui;

    const int FRAME_WIDTH = 640;
    const int FRAME_HEIGHT = 480;    

    cv::Ptr<cv::face::FaceRecognizer> recognizer;
    std::vector<cv::Mat> images;
    std::vector<int> labels;
    void init_trainingset();
    std::string composeName(double confidence, int predictedLabel);

    cv::CascadeClassifier face_cascade;
    cv::VideoCapture capture;
    QTimer *imageTimer;
    FaceApiClient *faceApi;

    cv::Mat img;
};

#endif // MAINWINDOW_H
