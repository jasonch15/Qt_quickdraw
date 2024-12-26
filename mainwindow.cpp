#include "mainwindow.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent), drawing(false) {
    setFixedSize(600, 400); // 畫布大小
    pixmap = QPixmap(size());
    pixmap.fill(Qt::white);
    brushColor = Qt::black;
    brushSize = 5;
}

void Canvas::setBrushColor(const QColor &color) {
    brushColor = color;
}

void Canvas::setBrushSize(int size) {
    brushSize = size;
}

void Canvas::setEraser() {
    brushColor = Qt::white;
}

QPixmap Canvas::getPixmap() const {
    return pixmap;
}

void Canvas::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        drawing = true;
        lastPos = event->pos();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    if (drawing && event->buttons() & Qt::LeftButton) {
        QPainter painter(&pixmap);
        QPen pen(brushColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.drawLine(lastPos, event->pos());
        lastPos = event->pos();
        update();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        drawing = false;
    }
}

void Canvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}

void Canvas::clearCanvas() {
    pixmap.fill(Qt::white); // 填充白色
    update(); // 更新畫布
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), resultFilePath("C:/Users/jason/Desktop/py_quickDraw_ndjson2img/py_quickDraw_ndjson2img/result.txt") {

    // 初始化定時器（暫不啟動）
    fileCheckTimer = new QTimer(this);
    connect(fileCheckTimer, &QTimer::timeout, this, &MainWindow::monitorResultFile);

    canvas = new Canvas(this);

    auto *layout = new QVBoxLayout();
    auto *controls = new QHBoxLayout();

    auto *colorButton = new QPushButton("調色盤");
    connect(colorButton, &QPushButton::clicked, this, &MainWindow::chooseColor);
    controls->addWidget(colorButton);

    auto *sizeLabel = new QLabel("粗細: 5");
    auto *sizeSlider = new QSlider(Qt::Horizontal);
    sizeSlider->setRange(1, 30);
    sizeSlider->setValue(5);
    connect(sizeSlider, &QSlider::valueChanged, [this, sizeLabel](int value) {
        sizeLabel->setText("粗細: " + QString::number(value));
        canvas->setBrushSize(value);
    });
    controls->addWidget(sizeLabel);
    controls->addWidget(sizeSlider);

    auto *eraserButton = new QPushButton("橡皮擦");
    connect(eraserButton, &QPushButton::clicked, canvas, &Canvas::setEraser);
    controls->addWidget(eraserButton);

    auto *clearButton = new QPushButton("清除畫布"); // 清除畫布按鈕
    connect(clearButton, &QPushButton::clicked, canvas, &Canvas::clearCanvas);
    controls->addWidget(clearButton);

    auto *saveButton = new QPushButton("保存");
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveCanvas);
    controls->addWidget(saveButton);

    layout->addLayout(controls);

    // 使用 QVBoxLayout 將畫布居中
    auto *canvasContainer = new QWidget();
    auto *canvasLayout = new QVBoxLayout();
    canvasLayout->addStretch(); // 增加彈性空間
    canvasLayout->addWidget(canvas, 0, Qt::AlignCenter); // 畫布居中
    canvasLayout->addStretch(); // 增加彈性空間
    canvasContainer->setLayout(canvasLayout);

    layout->addWidget(canvasContainer);

    auto *centralWidget = new QWidget();
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setWindowTitle("小畫家");
    resize(800, 600); // 窗口大小
}

MainWindow::~MainWindow() = default;

void MainWindow::chooseColor() {
    QColor color = QColorDialog::getColor(Qt::black, this, "選擇顏色");
    if (color.isValid()) {
        canvas->setBrushColor(color);
    }
}

// 保存圖片並啟動監視
void MainWindow::saveCanvas() {
    QString directory = "C:/Users/jason/Desktop/py_quickDraw_ndjson2img/py_quickDraw_ndjson2img/images";

    // 確保目錄存在
    QDir dir(directory);
    if (!dir.exists()) {
        dir.mkpath(directory);
    }

    // 生成唯一的文件名稱
   // QString fileName = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".png";
    QString fileName = "cat.png";
    QString filePath = directory + "/" + fileName;

    // 保存圖片
    if (canvas->getPixmap().save(filePath)) {
        QMessageBox::information(this, "保存成功", "圖片已保存到:\n" + filePath);

        // 啟動對結果文件的監視
        fileCheckTimer->start(1000);  // 每秒檢查一次
        qDebug() << "開始監視 result.txt";
    } else {
        QMessageBox::warning(this, "保存失敗", "無法保存圖片到指定路徑:\n" + filePath);
    }
}

// 監視 result.txt
void MainWindow::monitorResultFile() {
    QFile resultFile(resultFilePath);

    // 確保文件存在
    if (!resultFile.exists()) {
        return;  // 文件尚未生成，繼續監視
    }

    // 打開文件
    if (!resultFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "無法打開 result.txt";
        return;
    }

    QTextStream in(&resultFile);
    QString content = in.readAll();
    resultFile.close();

    // 檢查內容並顯示結果
    if (content.contains("Result: yes")) {
        QMessageBox::information(this, "辨識結果", "正確！");
    } else if (content.contains("Result: no")) {
        QMessageBox::critical(this, "辨識結果", "錯誤！");
    } else {
        return;  // 尚未生成完整結果，繼續監視
    }

    // 停止監視
    fileCheckTimer->stop();
    qDebug() << "監視已停止";
}
