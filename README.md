# 人脸识别考勤系统
基于 Qt6 + OpenCV4.5.2 + SeetaFace2 实现的本地人脸识别考勤上位机软件
实现摄像头采集、人脸检测、人脸比对、TCP网络通信、SQLite员工考勤记录。

## 技术栈
- Qt 6.10.3 MinGW64：GUI界面、TCP服务端/客户端、SQLite数据库、多线程
- OpenCV 4.5.2：摄像头视频采集、图像处理
- SeetaFace2：人脸检测、人脸关键点定位、人脸特征比对
- SQLite：存储员工信息与考勤记录

## 环境依赖
1. Qt6.10.3 MinGW64
2. OpenCV‑4.5.2 编译库
3. SeetaFace2 编译库

## 模型文件【重要，仓库不包含，需手动下载】
>模型文件体积较大，未上传仓库，请自行下载3个`.dat`模型放到程序运行目录：
|模型文件|作用|
| ---- | ---- |
|fd_2_00.dat|人脸检测模型|
|pd_2_00_pts5.dat|人脸5点关键点定位|
|fr_2_10.dat|人脸特征提取、人脸比对，比对阈值0.70|

额外资源：
`haarcascade_frontalface_alt2.xml` OpenCV人脸级联检测器，放到运行目录。

## 功能介绍
1. 摄像头实时画面采集，OpenCV人脸检测框选人脸
2. SeetaFace2做人脸特征比对，识别人脸ID
3. TCP通信，JSON格式报文收发，处理粘包（长度前缀协议）
4. SQLite数据库：员工信息查询，考勤打卡记录入库
5. UI展示摄像头画面、打卡信息、圆形头像显示

## 编译运行步骤
1. 使用Qt Creator打开项目，套件选择 `Desktop_Qt_6_10_3_MinGW_64_bit`
2. 配置OpenCV、SeetaFace2库路径
3. 将上述3个dat模型、haarcascade xml拷贝至exe同级目录
4. qmake构建，编译运行

## 项目模块说明
1. FaceAttendance：摄像头采集、人脸检测、TCP客户端
2. AttendenceWin：TCP服务端，接收人脸ID查询数据库，生成JSON考勤报文
3. 子线程处理人脸识别，避免UI阻塞
