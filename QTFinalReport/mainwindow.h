#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <cstdlib>
#include <ctime> // 用於生成隨機數
#include <random>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QApplication>


class Canvas : public QWidget {
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    void setBrushColor(const QColor &color);
    void setBrushSize(int size);
    void setEraser();
    QPixmap getPixmap() const;
    void clearCanvas();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap pixmap;
    QColor brushColor;
    int brushSize;
    QPoint lastPos;
    bool drawing;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void chooseColor();
    void saveCanvas();
    void monitorResultFile();
    void startGame(); // 新增：啟動遊戲功能
    void showNextQuestion(); // 在這裡宣告函數
    void showSummary();
    void clearResults();
    void onTimeOut();
    void updateTimer();

private:
    Canvas *canvas;
    QString resultFilePath;
    QTimer *fileCheckTimer;
    QVBoxLayout *mainLayout;
    QHBoxLayout *controls;
    QPushButton *startButton;   // "開始遊戲" 按鈕
    QLabel *questionLabel;      // 顯示題目
    QStringList questionPool;   // 題目池
    QString currentQuestion;    // 當前題目
    QStringList questionQueue;   // 保存隨機排列的6個題目
    int currentQuestionIndex;    // 當前題目的索引
    QTimer *delayTimer; // 用於保存後延遲處理
    QLabel *timeLabel; // 用於顯示倒計時
    QTimer *questionTimer; // 每題計時器
    int remainingTime; // 剩餘時間（秒）
    const int questionTimeLimit = 30; // 每一題限時（秒）

};

#endif // MAINWINDOW_H
