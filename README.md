## Answer-Card-Recognition
本项目是用图像处理的方法对常见的答题卡进行检测、识别，得到答题卡的结果。
This project is about Answer Card, using image process methods to detect and recognize the Answer card ,based on OpenCV 3.x.

最近发现很多人对这个项目有兴趣，那大家一起来完善一下吧，欢迎提Issue和PR来讨论改进。

## 2021年4月29日 面向对象重构
1. 使用cmake进行工程管理，对于包含cmake的IDE，只需修改answer_sheet_recognition\CmakeLists.txt文件的set(OpenCV_DIR "F:/cpp/opencv/build")

2. 为了将算法独立，分离出paperChecker类

3. 分离出axis类，用于表示坐标

