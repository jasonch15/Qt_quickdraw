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
#include <QMessageBox> // 用於顯示訊息框
#include <QDir>        // 用於操作目錄
#include <QDateTime>   // 用於生成當前日期和時間
#include <QTimer>



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
    void monitorResultFile();   // 監視結果文件

private:
    Canvas *canvas;
    QString resultFilePath;     // 結果文件路徑
    QTimer *fileCheckTimer;     // 文件檢查定時器
};

#endif // MAINWINDOW_H
