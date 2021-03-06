# 通道协议

## 预览非正常通道AbnormalPreview

### 通道格式

```json
{
    "AbnormalId": "",
    "AbnormalTaskName": "",
    "TargetTransform": "",
    "AdditiveTransform": "",
    "bWorldTransform": false,
    "bPreviewing": true,
    "CameraAddTrans": "",
    "PlayCommand": "",
    "PlayRate": 1.0
}
```

### 参数说明

| 参数名            | 值类型   | 说明                                    |
| ----------------- | -------- | --------------------------------------- |
| AbnormalId  | 字符串型 | 非正常任务Id                            |
| AbnormalTaskName  | 字符串型 | 非正常任务名                            |
| TargetTransform   | 字符串型 | 非正常生成的位置                        |
| AdditiveTransform | 字符串型 | 在线路中心位置基础上的偏移量            |
| bWorldTransform   | 布尔型   | 偏移量是否参照世界坐标系                |
| bPreviewing       | 布尔型   | 是否进行预览，用于控制场景视口          |
| CameraAddTrans    | 字符串型 | 摄像机的偏移量                          |
| PlayCommand       | 字符串型 | 控制动画效果的命令，Play/Pause/Stop/... |
| PlayRate          | 浮点型   | 动画播放的速率                          |

**参数详细说明：**

1. Transform的数据，顺序为(位置X,位置Y,位置Z;绕X轴旋转值,绕Y轴旋转值,绕Z轴旋转值;X轴向的缩放比例,Y轴向的缩放比例,Z轴向的缩放比例) 数值之间均由` `(一个空格)分隔。数据精度保留小数点后**6**位
2. 根据给定的**AbnormalTaskName**，三维场景程序会查找填表数据，查找到对应的非正常数据后，如果填表信息中设置的位置信息来源为*外部提供*时，那么通道中的**TargetTransform**就会起作用。场景程序会在**TargetTransform**给定的位置上生成一个*挂载点*。接着生成填表信息中的模型，将该模型挂载到*挂载点*上。这样一来，非正常模型就从属于挂载点，可以依据挂载点的坐标系做偏移也可以根据世界坐标系做偏移。此时会根据**bWorldTransform**来判断参照哪一个坐标系，将**AdditiveTransform**中的偏移量设置给模型。
3. **bPreviewing**参数设置为true时，在场景中会将视口跳转到一个距离非正常模型一定位置的地方。设置为false后，场景中的视口会恢复到预览前的视口位置。**CameraAddTrans**用来调整观察非正常模型的视口位置，需要三个数值，第一个数值控制摄像机的左右移动，第二个数值控制摄像机的远近移动，第三个数值控制摄像机的上下移动。
4. **PlayCommand**参数用来控制非正常动画效果。可选值有`Play/Pause/Stop/Replay`，分别用来控制非正常动画的`播放/暂停/跳转到动画结束状态并停止/重新播放`。**PlayRate**用于控制动画播放的速率。

### 使用示例

```json
{
    "AbnormalId": "AAA",
    "AbnormalTaskName": "HitStone",             
    "TargetTransform": "15.15315466 3.45215454 58.81635284 0.000000 0.000000 10.236502 1.0 1.0 1.0",              
    "AdditiveTransform": "3.00000000 0.000000 0.000000 90.000000 0.000000 0.000000 1.0 1.0 1.0",            
    "bWorldTransform": false,           
    "bPreviewing": true,                
    "CameraAddTrans": "-3.000000 -3.000000 2.000000",              
    "PlayCommand": "Play",                  
    "PlayRate": 1.0                     
}
```