import time
import os
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import subprocess

class ImageEventHandler(FileSystemEventHandler):
    def __init__(self, images_folder, script_path, python_path):
        self.images_folder = images_folder
        self.script_path = script_path
        self.python_path = python_path

    def on_created(self, event):
        # 只處理圖片檔案
        if event.src_path.lower().endswith(('.png', '.jpg', '.jpeg')):
            print(f"新圖片檔案檢測到: {event.src_path}")
            self.process_image()

    def process_image(self):
        # 執行辨識程式
        print("執行辨識程式...")
        subprocess.run([self.python_path, self.script_path], check=True)

def monitor_images_folder(images_folder, script_path, python_path):
    # 初始化監視器
    event_handler = ImageEventHandler(images_folder, script_path, python_path)
    observer = Observer()
    observer.schedule(event_handler, images_folder, recursive=False)

    # 開始監視
    print(f"開始監視資料夾: {images_folder}")
    observer.start()

    try:
        while True:
            time.sleep(1)  # 保持程式運行
    except KeyboardInterrupt:
        observer.stop()
        print("監視已停止。")

    observer.join()

if __name__ == "__main__":
    images_folder = "images"  # 資料夾路徑
    script_path = "lite.py"  # 辨識程式的路徑
    python_path = os.path.join("venv", "Scripts", "python")  # 虛擬環境中的 Python 解釋器路徑
    monitor_images_folder(images_folder, script_path, python_path)
