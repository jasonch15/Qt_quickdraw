import numpy as np
import tensorflow as tf
from PIL import Image, ImageOps
import os
import shutil  # 用於移動檔案

# 禁用科學記號
np.set_printoptions(suppress=True)

# 載入 TFLite 模型
interpreter = tf.lite.Interpreter(model_path="model_unquant.tflite")
interpreter.allocate_tensors()

# 獲取模型的輸入與輸出細節
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# 單張圖片辨識
def predict_image(image_path):
    # 載入圖片並轉換為 RGB
    image = Image.open(image_path).convert("RGB")

    # 調整大小並裁剪
    size = (224, 224)
    image = ImageOps.fit(image, size, Image.Resampling.LANCZOS)

    # 正規化圖片數據
    image_array = np.asarray(image).astype(np.float32) / 127.5 - 1
    image_array = np.expand_dims(image_array, axis=0)

    # 設定模型輸入
    interpreter.set_tensor(input_details[0]['index'], image_array)

    # 運行推理
    interpreter.invoke()

    # 獲取模型輸出
    output_data = interpreter.get_tensor(output_details[0]['index'])
    index = np.argmax(output_data)
    confidence_score = output_data[0][index]

    return index, confidence_score

# 核心邏輯
def process_single_image(folder_path, class_names_file, result_file, result_folder):
    # 確保 result_folder 存在
    os.makedirs(result_folder, exist_ok=True)

    # 載入標籤，忽略數字部分
    class_names = [line.strip().split(" ", 1)[1].lower() for line in open(class_names_file, "r").readlines()]

    # 確認資料夾中是否只有一張圖片
    image_files = [f for f in os.listdir(folder_path) if f.lower().endswith(('.png', '.jpg', '.jpeg'))]
    if len(image_files) != 1:
        raise ValueError("images 資料夾中應該只有一張圖片。")

    # 取得圖片檔案
    image_file = image_files[0]
    image_path = os.path.join(folder_path, image_file)

    # 辨識圖片
    index, confidence_score = predict_image(image_path)
    class_name = class_names[index]  # 模型預測的類別

    # 比較檔名與辨識結果
    file_name_without_extension = os.path.splitext(image_file)[0].strip().lower()
    print(f"檔名: {file_name_without_extension}, 預測類別: {class_name}")  # 調試輸出
    result = "yes" if file_name_without_extension == class_name else "no"

    # 將結果追加寫入 result.txt
    with open(result_file, "a") as f:  # 使用 'a' 模式以追加內容
        f.write(f"Image: {image_file} | Predicted Class: {class_name} | Confidence: {confidence_score:.2f} | Result: {result}\n")
    print(f"結果已追加到 {result_file}：{result}")

    # 移動圖片到 result_folder
    shutil.move(image_path, os.path.join(result_folder, image_file))
    print(f"{image_file} 已移動到 {result_folder}。")

# 主程式
if __name__ == "__main__":
    image_folder = "images"  # 圖片資料夾路徑
    class_names_file = "labels.txt"  # 標籤檔案
    result_file = "result.txt"  # 結果輸出檔案
    result_folder = "resultfile"  # 辨識後圖片的儲存資料夾

    process_single_image(image_folder, class_names_file, result_file, result_folder)
