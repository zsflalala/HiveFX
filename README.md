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

3. `adb bugreport E:\Battery\test.zip`  将该段时间的手机的应用程序及耗电量传输到电脑中。
   
   1. 选定某一个设备：`adb -s <设备ID> shell dumpsys batterystats --reset`
   
      `adb -s 172.20.10.12:40133 bugreport E:\Battery\ZhiCheng.zip`
   
   2. `e.g.: adb -s 192.168.1.100:5555 shell dumpsys batterystats --reset`





<center><font size='2'> 表 6. 不同路径下的平均每帧绘制三角形数测试


|                 | 每帧平均绘制三角形数 |              |              |
| :-------------: | :------------------: | :----------: | :----------: |
|  **剔除算法**   |     **地面路径**     | **低空路径** | **高空路径** |
|  Unity2022无OC  |        37477         |    42002     |    50197     |
| Unity2022原生OC |         9313         |    15267     |    46646     |
|  Unity2022ACS   |         4249         |     5226     |     9977     |
| ContourLinesOC  |                      |              |              |
|   Unity6无OC    |        37177         |    42331     |    51317     |
|   Unity6PGUOC   |         2386         |     2225     |     2169     |

## 
