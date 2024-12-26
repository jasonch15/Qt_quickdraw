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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
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

void MainWindow::saveCanvas() {
    QString filePath = QFileDialog::getSaveFileName(this, "保存畫布", "", "PNG Files (*.png);;All Files (*)");
    if (!filePath.isEmpty()) {
        canvas->getPixmap().save(filePath);
    }
}
