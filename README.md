# windows安装字体不重启

本人在开发electron-vue的一个项目中需要将字体本地化，mac和linux中直接复制字体到默认字体库内即可使用，但是windows却不行，需要写入信息到注册表，并且还需要通过api发送已经更新了字体的消息，才能达到字体使用无需重启的目的，所以才有了这个项目。

项目使用NodeJs中的N-Api的方式，用C++的代码实现注册表写入和字体更新api消息推送。

安装字体无需重启；
卸载字体则需要重启才能生效，卸载字体也不会去删除字体文件，只会删除其注册表项目。

**要求：**
1. 只用于windows系统
2. 需要保证字体文件存在于windows的默认字体库内;
3. 文件名必须和字体文件内设置的字体名称一致(因为本人项目中的字体文件名称是md5值，所以和字体文件内设置的字体名是不一致的，不一致的情况很可能造成注册成功也无法使用字体)；
4. 字体文件必须是正常能使用的字体文件

## 开发环境

- Visual Studio 2015 with c++ installed
- Python 2.7

## 安装

``` javascript

npm install mfont

```

## 使用范例

``` javascript
# 引用

import MFont from 'mfont'

# 安装字体

// 情况1：字体文件内设置的字体名称 === 字体文件名称
// 使用方法：installFont（字体文件名称）
// 返回结果：String  成功返回1，失败返回0，使用getError方法获取错误信息
let temp1 = MFont.installFont('temp1.ttf')
if (temp1 === '1') {
    return '注册字体成功'
} else {
    return MFont.getError()
}

// 情况2：字体文件内设置的字体名称 !== 字体文件名称
// 使用方法：installFont（字体文件名称, 预注册的字体名）
// 返回结果：String  成功返回1，失败返回0，使用getError方法获取错误信息
let temp2 = MFont.installFont('temp2.ttf', 't2')
if (temp2 === '1') {
    return '注册字体成功'
} else {
    return MFont.getError()
}

# 卸载字体

// 情况1：字体文件内设置的字体名称 === 字体文件名称
// 使用方法：removeFont（字体文件名称）
// 返回结果：String  成功返回1，失败返回0，使用getError方法获取错误信息
let temp3 = MFont.removeFont('temp3.ttf')
if (temp3 === '1') {
    return '卸载字体成功'
} else {
    return MFont.getError()
}

// 情况2：字体文件内设置的字体名称 !== 字体文件名称
// 使用方法：removeFont（字体文件名称, 预注册的字体名）
// 返回结果：String  成功返回1，失败返回0，使用getError方法获取错误信息
let temp4 = MFont.removeFont('temp4.ttf', 't4')
if (temp4 === '1') {
    return '卸载字体成功'
} else {
    return MFont.getError()
}


```

---
