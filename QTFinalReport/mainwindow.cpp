#include "mainwindow.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent), drawing(false) {
    setFixedSize(900, 600); // 畫布大小
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

    // 初始化文件監視定時器
    fileCheckTimer = new QTimer(this);
    connect(fileCheckTimer, &QTimer::timeout, this, &MainWindow::monitorResultFile);

    // 初始化計時器
    questionTimer = new QTimer(this);
    connect(questionTimer, &QTimer::timeout, this, &MainWindow::updateTimer);
    // 初始化倒計時顯示
    timeLabel = new QLabel("倒數時間：30", this);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet("font-size: 30px; color: white;");
    timeLabel->hide(); // 開始遊戲前隱藏

    // 初始化題目池
    questionPool = {"airplane", "bus", "cat", "clock", "fish", "flower", "key", "spider", "star", "tree"};
    currentQuestionIndex = 0;

    // 初始化畫布
    canvas = new Canvas(this);
    canvas->hide(); // 一開始隱藏畫布

    // 初始化主要佈局
    mainLayout = new QVBoxLayout();
    controls = new QHBoxLayout();

    // 顯示題目的標籤
    questionLabel = new QLabel("題目：", this);
    questionLabel->setAlignment(Qt::AlignCenter);

    // 設置字體大小並加粗
    QFont font = questionLabel->font();
    font.setPointSize(30);  // 設置字體大小為 20
    font.setBold(true);     // 設置字體加粗
    questionLabel->setFont(font);

    questionLabel->hide(); // 開始遊戲前隱藏


    // 調色盤按鈕
    auto *colorButton = new QPushButton("調色盤");
    colorButton->setStyleSheet("QPushButton {"
                              "border: 2px solid white;"   // 白色邊框
                              "background-color: yellow;"  // 黃色背景
                              "color: black;"              // 黑色文字
                              "font-size: 20px;"           // 文字大小
                              "padding: 5px 10px;"         // 按鈕內邊距
                              "border-radius: 5px;"        // 圓角邊框
                              "}");
    connect(colorButton, &QPushButton::clicked, this, &MainWindow::chooseColor);
    controls->addWidget(colorButton);
    colorButton->hide(); // 開始遊戲前隱藏

    // 粗細調節滑塊
    auto *sizeLabel = new QLabel("粗細: 5");
    auto *sizeSlider = new QSlider(Qt::Horizontal);
    sizeSlider->setRange(1, 30);
    sizeSlider->setValue(5);

    // 設置字體大小並加粗
    QFont fontsize = sizeLabel->font();
    fontsize.setPointSize(18);  // 設置字體大小為 18
    sizeLabel->setFont(fontsize);

    connect(sizeSlider, &QSlider::valueChanged, [this, sizeLabel](int value) {
        sizeLabel->setText("粗細: " + QString::number(value));
        canvas->setBrushSize(value);
    });

    controls->addWidget(sizeLabel);
    controls->addWidget(sizeSlider);

    // 開始遊戲前隱藏滑塊和顯示的標籤
    sizeLabel->hide();
    sizeSlider->hide();


    // 保存按鈕
    auto *saveButton = new QPushButton("保存");
    saveButton->setStyleSheet("QPushButton {"
                              "border: 2px solid white;"   // 白色邊框
                              "background-color: yellow;"  // 黃色背景
                              "color: black;"              // 黑色文字
                              "font-size: 20px;"           // 文字大小
                              "padding: 5px 10px;"         // 按鈕內邊距
                              "border-radius: 5px;"        // 圓角邊框
                              "}");
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveCanvas);
    controls->addWidget(saveButton);
    saveButton->hide();

    // 清除畫布按鈕
    auto *clearButton = new QPushButton("清除畫布");
    clearButton->setStyleSheet("QPushButton {"
                               "border: 2px solid white;"   // 白色邊框
                               "background-color: yellow;"  // 黃色背景
                               "color: black;"              // 黑色文字
                               "font-size: 20px;"           // 文字大小
                               "padding: 5px 10px;"         // 按鈕內邊距
                               "border-radius: 5px;"        // 圓角邊框
                               "}");
    connect(clearButton, &QPushButton::clicked, canvas, &Canvas::clearCanvas);
    controls->addWidget(clearButton);
    clearButton->hide();

    // 開始遊戲按鈕
    startButton = new QPushButton("開始遊戲", this);
    startButton->setStyleSheet("QPushButton {"
                               "border: 2px solid white;"   // 白色邊框
                               "background-color: yellow;"  // 黃色背景
                               "color: black;"              // 黑色文字
                               "font-size: 80px;"           // 文字大小
                               "padding: 5px 10px;"         // 按鈕內邊距
                               "border-radius: 5px;"        // 圓角邊框
                               "}");
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);


    // 佈局
    auto *centralWidget = new QWidget();
    auto *layout = new QVBoxLayout();
     layout->addWidget(timeLabel); // 添加倒計時顯示
    layout->addWidget(questionLabel);
    layout->addWidget(canvas, 0, Qt::AlignCenter);
    layout->addLayout(controls);
    layout->addWidget(startButton, 0, Qt::AlignCenter);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setWindowTitle("小畫家");
    resize(800, 600);
}


MainWindow::~MainWindow() = default;


void MainWindow::startGame() {
    qDebug() << "startGame called";
    // 隱藏開始遊戲按鈕
    startButton->hide();

    // 隨機排列6個題目
    std::random_device rd; // 用於生成隨機種子
    std::mt19937 g(rd()); // Mersenne Twister 隨機數生成器
    std::shuffle(questionPool.begin(), questionPool.end(), g);
    questionQueue = questionPool.mid(0, 6); // 取前6個題目

    // 顯示畫布
    canvas->show();

    // 顯式顯示每個控件
    for (int i = 0; i < controls->count(); ++i) {
        QWidget *widget = controls->itemAt(i)->widget();
        if (widget) {
            widget->show();
        }
    }

    // 更新佈局
    update();

    // 初始化索引並顯示第一題
    currentQuestionIndex = 0;
    showNextQuestion();
}



void MainWindow::showNextQuestion() {
    // 如果所有題目完成，顯示總結
    if (currentQuestionIndex >= questionQueue.size()) {
        QMessageBox::information(this, "遊戲結束", "所有題目完成！");
        showSummary(); // 顯示總結視窗
        return;
    }


    // 更新當前題目
    currentQuestion = questionQueue[currentQuestionIndex];
    questionLabel->setText("題目：" + currentQuestion);
    questionLabel->show();

    // 清空畫布
    canvas->clearCanvas();
    canvas->show();

    // 重置倒計時
    remainingTime = questionTimeLimit;
    timeLabel->setText(QString("倒數時間：%1").arg(remainingTime));
    timeLabel->show();

    // 啟動計時器
    questionTimer->start(1000); // 每秒觸發一次

    // 增加索引
    currentQuestionIndex++;
}

void MainWindow::onTimeOut() {
    // 停止計時器
    questionTimer->stop();

    // 自動保存並跳轉到下一題
    saveCanvas();
    QMessageBox::information(this, "時間到", "已超過時間！");
}

void MainWindow::updateTimer() {
    remainingTime--;
    timeLabel->setText(QString("倒數時間：%1").arg(remainingTime));

    if (remainingTime <= 0) {
        // 停止計時器
        questionTimer->stop();

        // 自動保存並跳轉到下一題
        QMessageBox::information(this, "時間到", "已超過時間！");
        saveCanvas();
    }
}


void MainWindow::chooseColor() {
    QColor color = QColorDialog::getColor(Qt::black, this, "選擇顏色");
    if (color.isValid()) {
        canvas->setBrushColor(color);
    }
}

// 保存圖片並啟動監視
void MainWindow::saveCanvas() {
    qDebug() << "saveCanvas called";
    QString directory = "C:/Users/jason/Desktop/py_quickDraw_ndjson2img/py_quickDraw_ndjson2img/images";

    // 確保目錄存在
    QDir dir(directory);
    if (!dir.exists()) {
        dir.mkpath(directory);
    }

    // 使用當前題目作為檔名
    QString fileName = currentQuestion + ".png";
    QString filePath = directory + "/" + fileName;

    // 保存圖片
    if (canvas->getPixmap().save(filePath)) {
        //QMessageBox::information(this, "保存成功", "圖片已保存到:\n" + filePath);
        // 停止計時器並隱藏倒計時
        questionTimer->stop();
        timeLabel->hide();

        // 創建進度條窗口
        QDialog *progressDialog = new QDialog(this);
        progressDialog->setWindowTitle("辨識中...");
        progressDialog->resize(300, 100);

        QVBoxLayout *layout = new QVBoxLayout(progressDialog);
        QLabel *label = new QLabel("正在辨識圖片，請稍候...", progressDialog);
        label->setAlignment(Qt::AlignCenter);
        QProgressBar *progressBar = new QProgressBar(progressDialog);
        progressBar->setRange(0, 0); // 無限進度模式
        layout->addWidget(label);
        layout->addWidget(progressBar);

        progressDialog->setLayout(layout);
        progressDialog->setModal(true);
        progressDialog->show();

        // 延遲5秒後開始監視
        QTimer::singleShot(10000, this, [this, progressDialog]() {
            progressDialog->accept();  // 關閉進度條窗口
            fileCheckTimer->start(1000);  // 每秒檢查一次
            qDebug() << "開始監視 result.txt";
        });
    } else {
        QMessageBox::warning(this, "保存失敗", "無法保存圖片到指定路徑:\n" + filePath);
    }
}


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
    QString lastLine;

    // 讀取最後一行
    while (!in.atEnd()) {
        lastLine = in.readLine();
    }
    resultFile.close();
    // 調試輸出
    qDebug() << "讀取到的最後一行：" << lastLine;

    // 檢查內容並顯示結果
    if (lastLine.contains("Result: yes")) {
        QMessageBox::information(this, "辨識結果", "正確！");
    } else if (lastLine.contains("Result: no")) {
        QMessageBox::critical(this, "辨識結果", "錯誤！");
    } else {
        qDebug() << "文件格式不正確，繼續監視...";
        return;  // 尚未生成完整結果，繼續監視
    }

    // 停止監視
    fileCheckTimer->stop();
    qDebug() << "監視已停止";

    // 顯示下一題
    showNextQuestion();
}


// 顯示總結
void MainWindow::showSummary() {
    // 創建新窗口顯示總結
    QDialog *summaryDialog = new QDialog(this);
    summaryDialog->setWindowTitle("答題總結");
    summaryDialog->resize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(summaryDialog);
    QGridLayout *gridLayout = new QGridLayout();

    // 打開 result.txt 文件
    QFile resultFile(resultFilePath);
    if (!resultFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "錯誤", "無法打開結果文件！");
        return;
    }

    QTextStream in(&resultFile);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    resultFile.close();

    // 確保只有 6 題結果
    int numQuestions = qMin(lines.size(), 6);

    for (int i = 0; i < numQuestions; ++i) {
        QString line = lines[i];
        QStringList parts = line.split("|");
        if (parts.size() < 4) continue;

        QString imageFile = parts[0].split(":").last().trimmed();
        QString predictedClass = parts[0].split(":").last().trimmed();  // 獲取 Predicted Class
        predictedClass.remove(".png");
        QString result = parts[3].split(":").last().trimmed();

        QString questionNumber = QString("第 %1 題").arg(i + 1);

        // 加載圖片
        QString imagePath = QString("C:/Users/jason/Desktop/py_quickDraw_ndjson2img/py_quickDraw_ndjson2img/resultfile/%1").arg(imageFile);
        QLabel *imageLabel = new QLabel();
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            imageLabel->setPixmap(pixmap.scaled(200, 150, Qt::KeepAspectRatio));
        } else {
            imageLabel->setText("無法加載圖片");
        }
        imageLabel->setAlignment(Qt::AlignCenter);  // 圖片居中顯示

        // 顯示題號標籤
        QLabel *questionLabel = new QLabel(questionNumber, summaryDialog);
        questionLabel->setAlignment(Qt::AlignCenter);
        questionLabel->setStyleSheet(
            "font-size: 16px; "
            "font-weight: bold; "
            "color: #4CAF50; "  // 顯示綠色
            "padding: 8px; "
            "border: 2px solid #4CAF50; "
            "border-radius: 5px; "
            "background-color: #e8f5e9;"  // 淺綠色背景
            );

        // 顯示題目名稱
        QLabel *titleLabel = new QLabel(QString("題目：%1").arg(predictedClass), summaryDialog);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(
            "font-size: 14px; "
            "font-weight: bold; "
            "color: White; "  // 顯示深灰色字體
            "padding: 5px;"
            );

        // 顯示答案標籤
        QString resultText = result == "yes" ? "正確" : "錯誤";
        QLabel *resultLabel = new QLabel(resultText, summaryDialog);
        resultLabel->setAlignment(Qt::AlignCenter);
        resultLabel->setStyleSheet(
            result == "yes" ?
                "font-size: 14px; "
                "color: white; "
                "background-color: green; "  // 正確顯示綠色背景
                "border-radius: 5px; "
                "padding: 8px;" :
                "font-size: 14px; "
                "color: white; "
                "background-color: red; "  // 錯誤顯示紅色背景
                "border-radius: 5px; "
                "padding: 8px;"
            );


        // 外圍布局調整
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->addWidget(questionLabel);
        vLayout->addWidget(titleLabel);
        vLayout->addWidget(imageLabel);
        vLayout->addWidget(resultLabel);
        vLayout->setSpacing(10);  // 控制元素之間的間距
        vLayout->setAlignment(Qt::AlignCenter);  // 垂直布局居中

        // 外圍容器
        QWidget *questionWidget = new QWidget();
        questionWidget->setLayout(vLayout);

        // 添加到 Grid Layout 中
        int row = i / 3;  // 行數
        int col = i % 3;  // 列數
        gridLayout->addWidget(questionWidget, row, col);
    }


    mainLayout->addLayout(gridLayout);

    // 添加按鈕
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *playAgainButton = new QPushButton("再玩一局", summaryDialog);
    playAgainButton->setStyleSheet("QPushButton {"
                                   "border: 2px solid white;"   // 白色邊框
                                   "background-color: yellow;"  // 黃色背景
                                   "color: black;"              // 黑色文字
                                   "font-size: 20px;"           // 文字大小
                                   "padding: 5px 10px;"         // 按鈕內邊距
                                   "border-radius: 5px;"        // 圓角邊框
                                   "}");
    QPushButton *exitButton = new QPushButton("結束遊戲", summaryDialog);
    exitButton->setStyleSheet("QPushButton {"
                              "border: 2px solid white;"   // 白色邊框
                              "background-color: yellow;"  // 黃色背景
                              "color: black;"              // 黑色文字
                              "font-size: 20px;"           // 文字大小
                              "padding: 5px 10px;"         // 按鈕內邊距
                              "border-radius: 5px;"        // 圓角邊框
                              "}");

    connect(playAgainButton, &QPushButton::clicked, this, [this, summaryDialog]() {
        clearResults();  // 清空結果文件與資料夾
        summaryDialog->accept();  // 關閉總結窗口
        startGame();  // 開始新遊戲
    });

    // 結束遊戲
    connect(exitButton, &QPushButton::clicked, this, [this]() {
        clearResults();  // 清空結果文件與資料夾
        QApplication::quit();  // 結束程式
    });

    buttonLayout->addWidget(playAgainButton);
    buttonLayout->addWidget(exitButton);

    mainLayout->addLayout(buttonLayout);

    summaryDialog->exec();
}


void MainWindow::clearResults() {
    // 清空 result.txt 文件
    QFile resultFile(resultFilePath);
    if (resultFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        resultFile.resize(0);  // 清空文件內容
        resultFile.close();
    }

    // 刪除 resultfile 資料夾中的所有文件
    QDir resultDir("C:/Users/jason/Desktop/py_quickDraw_ndjson2img/py_quickDraw_ndjson2img/resultfile");
    if (resultDir.exists()) {
        for (const QFileInfo &fileInfo : resultDir.entryInfoList(QDir::Files)) {
            QFile::remove(fileInfo.absoluteFilePath());
        }
    }
}


