# HiveFX
 play cartoon in mobile phone



## Note

目前手机纹理最大载入纹理为：`15664 x 3528` ;

手机最大载入图片分辨率必须小于 `17920 x 4032` ;

`#include <glad/glad.h>` 替换成 `#include <GLES3/gl3.h>`



## TO DO

1. 将 HiveFX PC 端移植到 Android Studio 上；
2. 优化序列帧播放纹理载入算法；



## 测试流程

测试时间：5 min

测试工具：SmartPerf （帧率平均、最高、最低）、Android Debug Bridge（总功耗、每帧平均功耗）

1. `adb devices` 查看所连接的手机列表。
2. `adb shell dumpsys batterystats --reset`  重置电池数据。
3. `adb bugreport [path/]bugreport.zip`  将改段时间的手机的应用程序及耗电量传输到电脑中。
