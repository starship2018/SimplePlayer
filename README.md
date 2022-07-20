# SimplePlayer
FFmpeg4.4 + SDL2.0.22 + QT5.12.0  Compiler : MSVC2017

环境搭建：  
1.百度下载FFmpeg4.X  
2.百度下载SDL2.X  
3.将目录中的SDL2.dll拷贝至可执行文件目录中（SDL2.dll是修改源码重新生成的，目的是为了解决窗口拖放的卡死BUG，若有更好的解决方案也可以使用SDL中的DLL）  
4.将下载的FFmpeg和SDL放置在源码根目录中，具体的路径配置参考.pro文件，对文件夹名需要做适当的调整  

说明：  
1.作为一个FFmpeg学习小项目，本项目参考了夏曹骏老师的视频资料，并加以改造升级，使用SDL替换OpenGL。代码中存在的诸多问题，欢迎提出！  
2.后期有时间会加以升级，工作较忙... 

功能说明：  
1.Open打开本地文件  
2.双击屏幕全屏播放，再次双击退回原始大小  
3.窗口支持自由拖拽移动和拉伸缩放  

![contents](https://github.com/starship2018/SimplePlayer/blob/main/Snipaste_2022-07-20_16-37-41.png)
