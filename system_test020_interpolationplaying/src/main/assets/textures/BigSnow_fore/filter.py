import os
import shutil

# 定义源文件夹和目标文件夹路径
source_folder = r"e:\source\c++\HiveFX\system_test020_interpolationplaying\src\main\assets\textures\BigSnow_fore"
target_folder = r"e:\source\c++\HiveFX\system_test020_interpolationplaying\src\main\assets\textures\BigSnow_fore_64"

# 确保目标文件夹存在
os.makedirs(target_folder, exist_ok=True)

# 获取源文件夹中的所有文件
files = os.listdir(source_folder)

# 筛选出文件名中数字部分为奇数的文件
odd_files = [
    f for f in files
    if f.startswith("frame_") and f.endswith(".png") and f.split('_')[-1].split('.')[0].isdigit() and int(f.split('_')[-1].split('.')[0]) % 2 == 1
]

# 确保只有64张图片
odd_files = odd_files[:64]

# 复制并重命名文件
for i, file_name in enumerate(odd_files, start=1):
    source_path = os.path.join(source_folder, file_name)
    target_name = f"frame_{i:03}.png"  # 生成类似 frame_001.png 的文件名
    target_path = os.path.join(target_folder, target_name)  # 假设目标文件格式为 .png
    shutil.copy(source_path, target_path)

print(f"完成！已将 {len(odd_files)} 张图片复制到 {target_folder} 并重新命名。")