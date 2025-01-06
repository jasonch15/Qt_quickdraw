# Qt 與 AI 結合的圖像識別互動系統

## 專案簡介
### 專案名稱
# 基於 Qt 與 AI 的圖像識別互動系統
# 期末報告

### **1. 專案簡介**
- **專案名稱**：基於 Qt 與 AI 的圖像識別互動系統
- **靈感來源於 Quick, Draw!**：
  - 由Google開發的一款線上遊戲，玩法是由玩家按所給題目繪出圖形，同時由人工神經網絡來猜測圖形所代表的內容，猜中即為成功。
- **開發目標**：
  - 利用 Qt ，實現一個結合畫布繪製與 AI 圖像分類的桌面應用程式。


---

### **2. 技術流程**
![image](https://github.com/user-attachments/assets/12891281-a10a-4a59-9018-7f454fd62a57)

---
### **3. 技術實現**
- **系統架構**：
  - 前端 (Qt)：負責畫布繪製、互動與文件監控。
  - 後端 (Python + TFLite)：負責圖片識別與結果生成。
  - 整合:畫布數據轉換指定格式。使用文件共享方式將畫布資料傳送至 AI 推理模組。
  
- **開發環境與工具**：
  - Qt 6.8.1 (C++ 開發)
  - Python 3.9 (AI 模型與文件監控)
  - TensorFlow Lite 模型
  - Visual Studio Code 與 Qt Creator IDE
  
- **技術分工**：
  - Qt：畫布、界面、互動、計時與總結頁面。
  - AI 模型：圖像分類與結果生成。

---

### **5. 成果展示**
- **畫布繪製與工具實現**：
 - 使用 Canvas創建畫布並使用QPainter 進行繪圖。
- **文件監控與處理**：
 - 使用 QFile 實時監控 result.txt，讀取最新的辨識結果、提取內容進行解析，並通過 QTimer 定時檢測文件變化。
- **Layout**：
 - 使用 QGridLayout 排列圖片與文字
 - 根據 result.txt 的內容解析每一題結果，動態生成 QLabel 與 QPixma
 - 使用 QDialog 顯示總結頁面，支持動態布局。


  ![image](https://github.com/user-attachments/assets/ca01d648-0647-4983-9751-735a178caaeb)


---
### **8. 附件**
[視窗程式設計期末.pptx](/視窗程式設計期末.pptx)
