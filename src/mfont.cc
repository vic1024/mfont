#include <napi.h>
#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
// #include <wingdi.h>
#include <Windows.h>
#include <tchar.h>
#include <experimental/filesystem>
#include <string>
#include <atlbase.h> // 注册表
#include <atlconv.h> // 类型转换
#include <atlstr.h>
#endif

using namespace Napi;
static std::string _ERROR = "";

#if defined(_WIN32) || defined(_WIN64)
static int _HASFONT = 0;
static std::string _FONTPATH = "C:\\Windows\\Fonts\\";
static std::string _REGPATH_1 = "HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
static std::string _REGPATH_2 = "HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes";
// static std::string _REGPATH_3 = "HKLM\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
// static std::string _REGPATH_4 = "HKLM\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes";

/**
  * --------------------------------- 注册表操作部分 ---------------------------------
  * 包含了注册表子项的增改、读取、删除，
  * 增改的部分不会自动添加父路径，所以要注意使用
**/

/**
  * -------- 查找注册表的根键 --------
  * 返回类型：HKEY
  * 返回结果：[ 注册表根键名称 ]
**/
HKEY FindRegeditHKEY(const std::string name) {
  HKEY result = HKEY_LOCAL_MACHINE;
  if ((name == "HKCR") || (name == "HKEY_CLASSES_ROOT")) {
    result = HKEY_CLASSES_ROOT;
  } else if ((name == "HKCU") || (name == "HKEY_CURRENT_USER")) {
    result = HKEY_CURRENT_USER;
  } else if ((name == "HKU") || (name == "HKEY_USERS")) {
    result = HKEY_USERS;
  } else if ((name == "HKCC") || (name == "HKEY_CURRENT_CONFIG")) {
    result = HKEY_CURRENT_CONFIG;
  } else if ((name == "HKLM") || (name == "HKEY_LOCAL_MACHINE")) {
    result = HKEY_LOCAL_MACHINE;
  }
  return result;
}

/**
  * -------- 查找注册表的数据类型 --------
  * 返回类型：DWORD
  * 返回结果：[ 注册表数据类型 ]
**/
DWORD FindRegeditDataType(const std::string name) {
  DWORD result = REG_SZ;
  if ((name == "bin") || (name == "binary") || (name == "REG_BINARY")) {
    result = REG_BINARY;
  } else if ((name == "dword") || (name == "REG_DWORD")) {
    result = REG_DWORD;
  } else if (name == "REG_DWORD_BIG_ENDIAN") {
    result = REG_DWORD_BIG_ENDIAN;
  } else if (name == "REG_EXPAND_SZ") {
    result = REG_EXPAND_SZ;
  } else if (name == "REG_LINK") {
    result = REG_LINK;
  } else if (name == "REG_RESOURCE_LIST") {
    result = REG_RESOURCE_LIST;
  } else if (name == "REG_MULTI_SZ") {
    result = REG_MULTI_SZ;
  } else if ((name == "string") || (name == "REG_SZ")) {
    result = REG_SZ;
  }
  return result;
}

/**
  * -------- 根据查找出来的类型查找注册表的数据类型 --------
  * 返回类型：DWORD
  * 返回结果：[ 注册表数据类型 ]
**/
DWORD FindRegeditDataTypeFromNumber(const int num) {
  DWORD result = REG_SZ;
  if (num == 1) {
    result = REG_SZ;
  } else if (num == 2) {
    result = REG_EXPAND_SZ;
  } else if (num == 3) {
    result = REG_BINARY;
  } else if (num == 4) {
    result = REG_DWORD;
  // } else if (num == 5) {
  //   result = ;
  // } else if (num == 6) {
  //   result = ;
  } else if (num == 7) {
    result = REG_MULTI_SZ;
  // } else if (num == 8) {
  //   result = ;
  } else if (num == 11) {
    result = REG_QWORD;
  }
  // REG_LINK
  // REG_RESOURCE_LIST
  // REG_DWORD_BIG_ENDIAN

  // REG_DWORD_LITTLE_ENDIAN
  // REG_DWORD_BIG_ENDIAN
  // REG_NONE
  return result;
}

/**
  * -------- 根据查找出来的类型查找注册表的数据类型文本 --------
  * 返回类型：String
  * 返回结果：[ 注册表数据类型 ]
**/
std::string FindRegeditDataTypeTextFromNumber(const int num) {
  std::string result = "";
  if (num == 1) {
    result = "REG_SZ";
  } else if (num == 2) {
    result = "REG_EXPAND_SZ";
  } else if (num == 3) {
    result = "REG_BINARY";
  } else if (num == 4) {
    result = "REG_DWORD";
  // } else if (num == 5) {
  //   result = ;
  // } else if (num == 6) {
  //   result = ;
  } else if (num == 7) {
    result = "REG_MULTI_SZ";
  // } else if (num == 8) {
  //   result = ;
  } else if (num == 11) {
    result = "REG_QWORD";
  }
  // REG_LINK
  // REG_RESOURCE_LIST
  // REG_DWORD_BIG_ENDIAN

  // REG_DWORD_LITTLE_ENDIAN
  // REG_DWORD_BIG_ENDIAN
  // REG_NONE
  return result;
}

/**
  * -------- 读取注册表 --------
  * 返回类型：String
  * 返回结果：[ 读取出来的值字符串化 ]
**/
std::string RegeditRead(std::string path, std::string name) {
  std::string p0 = path.substr(0, path.find("\\")); // 查找根键
  HKEY hkey = FindRegeditHKEY(p0);
  std::string p1 = path.substr(path.find("\\") + 1); // 路径，要消除前面的HKLM
  std::string lastreturn = ""; // 最后的返回值
  HKEY hKEY; // 定义有关的键，在查询结束时关闭
  LPCTSTR data_set = p1.c_str(); // 打开与路径data_Set相关的hKEY
  // char dwValue[256]; // 用于存储查询结果的变量，默认为字符串数据
  // DWORD dwValue; // 长整型数据，如果是字符串数据用char数组
  LONG openreturn = (::RegOpenKeyEx(hkey, data_set, 0, KEY_READ, &hKEY));
  //访问注册表，hKEY则保存此函数所打开的键的句柄
  lastreturn = openreturn;
  if (openreturn == ERROR_SUCCESS)
  {
    DWORD dwType = REG_SZ;
    DWORD dwSize;
    // 空查询一次，用于自动设置dwSize的值
    openreturn = ::RegQueryValueEx(hKEY, name.c_str(), 0, &dwType, NULL, &dwSize);
    // openreturn = ::RegQueryValueEx(hKEY, name.c_str(), 0, NULL, NULL, &dwSize);
    // lastreturn = std::to_string(dwSize);
    // lastreturn = std::to_string(dwType);
    // 正式查询
    // LONG queryreturn = (::RegQueryValueEx(hKEY, name.c_str(), 0, &dwType, (LPBYTE)&dwValue, &dwSize));
    DWORD newdwType = FindRegeditDataTypeFromNumber(dwType);
    // lastreturn = std::to_string(newdwType);
    LONG queryreturn;
    std::string type = FindRegeditDataTypeTextFromNumber(newdwType);
    if (type == "REG_SZ") {
      char dwValue[256];
      // dwSize = sizeof(dwValue);
      queryreturn = (::RegQueryValueEx(hKEY, name.c_str(), 0, &newdwType, (LPBYTE)&dwValue, &dwSize));
      lastreturn = dwValue;
    } else if (type == "REG_DWORD") {
      DWORD dwValue;
      // dwSize = sizeof(dwValue);
      queryreturn = (::RegQueryValueEx(hKEY, name.c_str(), 0, &newdwType, (LPBYTE)&dwValue, &dwSize));
      // lastreturn = "name => " + name + ";";
      // lastreturn += "newdwType => " + std::to_string(newdwType) + ";";
      // lastreturn += "type => " + type + ";";
      // lastreturn += "queryreturn => " + std::to_string(queryreturn) + ";";
      // lastreturn += "size => " + std::to_string(dwSize) + ";";
      // lastreturn += "value => " + std::to_string(dwValue) + ";";
      lastreturn = std::to_string(dwValue);
    }
    // lastreturn = type;
    // lastreturn = std::to_string(queryreturn);
    // lastreturn = std::to_string(dwSize);
    // lastreturn = dwValue;
    // lastreturn = FindRegeditDataTypeTextFromNumber(dwType);
    if (queryreturn != ERROR_SUCCESS)
    {
      lastreturn = ""; // 查询错误的情况返回空字符串
      // } else {
      //   lastreturn = dwValue; // 查询正确的情况返回值
    }
  }
  // lastreturn = std::to_string(info[1]);
  ::RegCloseKey(hKEY);
  return lastreturn;
}

/**
  * -------- 写入注册表 --------
  * 返回类型：String [ 0, 1 ]
  * 返回结果：[ 是否写入注册表成功 ]
**/
std::string RegeditWrite(std::string path, std::string name, std::string value, std::string type) {
  std::string p0 = path.substr(0, path.find("\\")); // 查找根键
  HKEY hkey = FindRegeditHKEY(p0);
  std::string p1 = path.substr(path.find("\\") + 1); // 路径地址
  // int lastreturn1 = 0;
  std::string lastreturn = "0";

  HKEY hKEY; // 定义有关的键，在查询结束时关闭
  // HKEY hTempKey; // 打开与路径data_Set相关的hKEY
  LPCTSTR data_set = p1.c_str();

  // DWORD dwValue; // 长整型数据，如果是字符串数据用char数组
  LONG openreturn = (::RegOpenKeyEx(hkey, data_set, 0, KEY_ALL_ACCESS, &hKEY));
  //访问注册表，hKEY则保存此函数所打开的键的句柄
  // lastreturn = std::to_string(openreturn);
  if (openreturn == ERROR_SUCCESS) {
    DWORD dwSize;
    DWORD dwType;
    LONG writereturn;
    // LONG createreturn = ::RegCreateKey(hKEY, name.c_str(), &hTempKey);
    // lastreturn = std::to_string(createreturn);
    // if (createreturn == ERROR_SUCCESS) {
    // LONG writereturn = ::RegSetValueEx(hKEY, name.c_str(), 0, dwType, (LPBYTE)value.c_str(), MAX_PATH);
    if (type == "REG_DWORD") {
      dwType = REG_DWORD; // 设置类型
      DWORD dwValue = atol(value.c_str()); // 字符串转DWORD
      dwSize = sizeof(dwValue); // 再次计算长度，写入才不会出错
      writereturn = ::RegSetValueEx(hKEY, name.c_str(), 0, dwType, (const byte *)&dwValue, dwSize);
    } else if (type == "REG_BINARY") {
      dwType = REG_BINARY;
      dwSize = sizeof(value);
      writereturn = ::RegSetValueEx(hKEY, name.c_str(), 0, dwType, (const unsigned char *)value.c_str(), dwSize);
    } else {
      dwType = REG_SZ; // 设置类型
      dwSize = sizeof(value); // 计算字符串长度
      writereturn = ::RegSetValueEx(hKEY, name.c_str(), 0, dwType, (const unsigned char *)value.c_str(), dwSize);
    }
    if (writereturn == ERROR_SUCCESS) {
      lastreturn = "1"; // 写入成功
    }
    // }
  }
  ::RegCloseKey(hKEY);
  return lastreturn;
}

/**
  * -------- 删除注册表 --------
  * 返回类型：String [ 0, 1 ]
  * 返回结果：[ 是否删除注册表成功 ]
**/
std::string RegeditDelete(std::string path, std::string name) {
  std::string p0 = path.substr(0, path.find("\\")); // 查找根键
  HKEY hkey = FindRegeditHKEY(p0);
  std::string p1 = path.substr(path.find("\\") + 1);
  std::string lastreturn = "0"; // 最后的返回值

  HKEY hKEY; // 定义有关的键，在查询结束时关闭
  LPCTSTR data_set = p1.c_str(); // 打开与路径data_Set相关的hKEY
  LONG openreturn = (::RegOpenKeyEx(hkey, data_set, 0, KEY_SET_VALUE, &hKEY));
  //访问注册表，hKEY则保存此函数所打开的键的句柄
  lastreturn = openreturn;
  if (openreturn == ERROR_SUCCESS)
  {
    LONG queryreturn = ::RegDeleteValue(hKEY, name.c_str());
    if (queryreturn != ERROR_SUCCESS)
    {
      lastreturn = "0"; // 查询错误的情况返回空字符串
    } else {
      lastreturn = "1"; // 查询正确的情况返回值
    }
  }
  // lastreturn = std::to_string(info[1]);
  ::RegCloseKey(hKEY);
  return lastreturn;
}


/**
  * --------------------------------- 字体操作部分 ---------------------------------
**/

/**
  * -------- 判断字体是否存在的回调函数 --------
  * 返回类型：int [0|1]
**/
int CALLBACK EnumFontsProc(LOGFONT* lplf, TEXTMETRIC* lptm, DWORD	dwType, LPARAM	lpData) {
  CString tempFontName(lplf->lfFaceName); // 获取查找到的字体名称
  // _ERROR = std::to_string(tempFontName.GetLength());
  // _ERROR = CT2A(tempFontName);
  _HASFONT = tempFontName.GetLength(); // 获取查找到的字体名称长度
  if (tempFontName.GetLength() > 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
  * -------- 判断字体是否存在 --------
  * 返回类型：String [0|1]
  * 返回结果：[
  *     0： 字体不存在,
  *     1： 字体存在，
  * ]
**/
std::string FontFind(std::string name) {
  _ERROR = "";
  _HASFONT = 0;
  std::string result = "0";
  // 提取整个屏幕的DC
  HDC hdc = ::GetDC(NULL);
  LPARAM  lp;
  // 判断字体是否在系统可用
  lp = EnumFonts(hdc, name.c_str(), (FONTENUMPROC)EnumFontsProc, 0);
  _ERROR = std::to_string(_HASFONT);
  if(_HASFONT <= 0) {
    _ERROR = "font not find";
    result = "0";
  } else {
    // _ERROR = "font found";
    _ERROR = "";
    result = "1";
  } 
  return result;
}

/**
  * -------- 添加字体 --------
  * 返回类型：String [0-4]
  * 返回结果：[
  *     0： 字体文件不存在,
  *     1-3： 有操作动作，但未成功，检查一下字体文件是否正常，
  *     4： 操作成功，
  * ]
**/
std::string FontAdd(std::string file) {
  // int r0 = AddFontResource(_T("c:\\windows\\fonts\\mh_Courier.ttf"));
  int r = std::experimental::filesystem::exists(_T(file.c_str())); // 查询字体文件是否存在
  int rr = 0;
  if (r > 0) {
    // 字体文件存在才进行操作
    int r0 = AddFontResource(_T(file.c_str())); // 添加字体到系统字体列表内
    int err = GetLastError(); // 获取执行结果是否出错
    // int r0 = AddFontResourceA(_T(filepath.c_str()));
    // 发送消息给各个顶部窗口进行字体修改的更新-WM_FONTCHANGE
    // SendMessage会造成卡死，弃用
    // int r1 = ::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    // int r2 = SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    int r3 = ::PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    int r4 = PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    // 重置系统光标，更新用户配置文件之后广播WM_SETTINGCHANGE消息
    int r5 = SystemParametersInfo(SPI_SETCURSORS, 0, NULL, SPIF_SENDCHANGE);
    rr = r0 + r3 + r4 + r5; // r0 + r1 + r2 + r3 + r4 + r5;
  } else {
    _ERROR = "can't find font file.";
  }
  return std::to_string(rr);
}

/**
  * -------- 删除字体 --------
  * 返回类型：String [0-4]
  * 返回结果：[
  *     0： 字体文件不存在,
  *     1-3： 有操作动作，但未成功，检查一下字体文件是否正常，
  *     4： 操作成功，
  * ]
**/
std::string FontDelete(std::string file) {
  int r = std::experimental::filesystem::exists(_T(file.c_str())); // 查询字体文件是否存在
  int rr = 0;
  if (r > 0) {
    // 字体文件存在才进行操作
    int r0 = RemoveFontResource(_T(file.c_str())); // 从系统字体列表中删除字体
    int err = GetLastError(); // 获取执行结果是否出错
    // 发送消息给各个顶部窗口进行字体修改的更新-WM_FONTCHANGE
    // SendMessage会造成卡死，弃用
    // int r1 = ::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    // int r2 = SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    int r3 = ::PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    int r4 = PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    // 重置系统光标，更新用户配置文件之后广播WM_SETTINGCHANGE消息
    int r5 = SystemParametersInfo(SPI_SETCURSORS, 0, NULL, SPIF_SENDCHANGE);
    rr = r0 + r3 + r4 + r5; // r0 + r1 + r2 + r3 + r4 + r5;
  } else {
    _ERROR = "can't find font file.";
  }
  return std::to_string(rr);
}

/**
  * --------------------------------- 对外操作部分 ---------------------------------
**/

/**
  * -------- 读取注册表 --------
  * 返回类型：String。
  * 返回结果： [
  *     "": 【 error不为空 =》 参数错误，   error为空 => 注册表读取不到或者值就是为空的 】,
  *     "读取到的值"
    ]
**/
String ReadReg(const CallbackInfo& info) {
  _ERROR = std::string("");
  if ((info[0] == NULL) || (info[1] == NULL)) {
    _ERROR = std::string("param error."); // std::string("传入参数错误");
    return String::New(info.Env(), std::string(""));
  }
  std::string path = info[0].As<String>().Utf8Value();
  std::string name = "";
  if (info[1] == NULL) {
    name = path.substr(path.find_last_of("\\") + 1); // 键名
    path = path.substr(0, path.find_last_of("\\")); // 路径
  } else {
    name = info[1].As<String>().Utf8Value(); // 键名
  }
  return String::New(info.Env(), RegeditRead(path, name));
}

/**
  * -------- 新增或修改注册表 --------
  * 返回类型：String。
  * 返回结果： [
  *     "": 参数错误,
  *     "0"： 操作失败,
  *     "1"： 操作成功
    ]
**/
String WriteReg(const CallbackInfo& info) {
  _ERROR = std::string("");
  if ((info[0] == NULL) || (info[1] == NULL) || (info[2] == NULL) || (info[3] == NULL)) {
    _ERROR = std::string("param error."); // std::string("传入参数错误");
    return String::New(info.Env(), std::string(""));
  }
  std::string path = info[0].As<String>().Utf8Value();
  std::string name = info[1].As<String>().Utf8Value();
  std::string value = info[2].As<String>().Utf8Value();
  std::string type = info[3].As<String>().Utf8Value();
  return String::New(info.Env(), RegeditWrite(path, name, value, type));
}

/**
  * -------- 删除注册表 --------
  * 返回类型：String。
  * 返回结果： [
  *     "": 参数错误,
  *     "0"： 操作失败,
  *     "1"： 操作成功
    ]
**/
String DeleteReg(const CallbackInfo& info) {
  _ERROR = std::string("");
  if ((info[0] == NULL) || (info[1] == NULL)) {
    _ERROR = std::string("param error."); // std::string("传入参数错误");
    return String::New(info.Env(), std::string(""));
  }

  std::string path = info[0].As<String>().Utf8Value();
  std::string name = info[1].As<String>().Utf8Value(); // 键名
  return String::New(info.Env(), RegeditDelete(path, name));
}

// 添加字体
String AddFont(const CallbackInfo& info) {
  _ERROR = std::string("");
  if (info[0] == NULL) {
    _ERROR = std::string("param error."); // std::string("传入参数错误");
    return String::New(info.Env(), std::string(""));
  }

  std::string file = info[0].As<String>().Utf8Value();
  return String::New(info.Env(), FontAdd(_FONTPATH + file));
}

// 删除字体
String DeleteFont(const CallbackInfo& info) {
  _ERROR = std::string("");
  if (info[0] == NULL) {
    _ERROR = std::string("param error."); // std::string("传入参数错误");
    return String::New(info.Env(), std::string(""));
  }

  std::string file = info[0].As<String>().Utf8Value();
  return String::New(info.Env(), FontDelete(_FONTPATH + file));
}
#endif

// 字体是否安装
String HasFont(const CallbackInfo& info) {
std::string result = "0";
#if defined(_WIN32) || defined(_WIN64)
  result = FontFind(info[0].As<String>().Utf8Value());
#else
  _ERROR = "system is not win";
  result = "0";
#endif
  return String::New(info.Env(), result);
}

// 安装注册字体
String InstallFont(const CallbackInfo& info) {
std::string result = "0";
#if defined(_WIN32) || defined(_WIN64)
  _ERROR = "";
  std::string file = info[0].As<String>().Utf8Value(); // 获取字体文件名称
  std::string filename = file.substr(0, file.find_last_of('.'));
  if (info[1] != NULL) {
    filename = info[1].As<String>().Utf8Value();
  }
  int r = std::experimental::filesystem::exists(_T(_FONTPATH + file.c_str()));
  if (r <= 0) {
    _ERROR = "font add faile: can't find font file."; // "查找不到字体文件";
    result = "0";
  } else {
    std::string filesubfix = file.substr(file.find_last_of('.') + 1);
    std::string result1 = RegeditWrite(_REGPATH_1, filename, file, "REG_SZ");
    if (result1 == "1") {
      std::string result2 = RegeditWrite(_REGPATH_2, filename, filename, "REG_SZ");
      if (result2 == "1") {
        std::string addresult = FontAdd(_FONTPATH + file);
        if (addresult == "4") {
          result = "1";
        } else if (addresult != "0") {
          _ERROR = "font add faile: " + addresult + "."; // "字体添加失败";
          result = "0";
        } else {
          _ERROR = "font add faile: " + _ERROR; // "字体添加失败";
          result = "0";
        }
      } else {
        _ERROR = "font add faile: write regedit faile."; // "注册表写入失败";
        result = "0";
      }
    } else {
      _ERROR = "font add faile: write regedit faile."; // "注册表写入失败";
      result = "0";
    }
  }
#else
  _ERROR = "system is not win";
  result = "0";
#endif
  return String::New(info.Env(), result);
}

// 删除字体
String RemoveFont(const CallbackInfo& info) {
std::string result = "0";
#if defined(_WIN32) || defined(_WIN64)
  _ERROR = "";
  std::string file = info[0].As<String>().Utf8Value(); // 获取字体文件名称
  std::string filename = file.substr(0, file.find_last_of('.'));
  if (info[1] != NULL) {
    filename = info[1].As<String>().Utf8Value();
  }
  int r = std::experimental::filesystem::exists(_T(_FONTPATH + file.c_str()));
  if (r <= 0) {
    _ERROR = "font add faile: can't find font file."; // "查找不到字体文件";
    result = "0";
  } else {
    std::string filesubfix = file.substr(file.find_last_of('.') + 1);
    std::string result1 = RegeditDelete(_REGPATH_1, filename);
    if (result1 == "1") {
      std::string result2 = RegeditDelete(_REGPATH_2, filename);
      if (result2 == "1") {
        std::string addresult = FontDelete(_FONTPATH + file);
        if (addresult == "4") {
          result = "1";
        } else if (addresult != "0") {
          _ERROR = "font remove faile: " + addresult + "."; // "字体移除失败-操作失败";
          result = "0";
        } else {
          _ERROR = "font remove faile: " + _ERROR; // "字体移除失败-字体文件不存在";
          result = "0";
        }
      } else {
        _ERROR = "font remove faile: write regedit faile."; // "注册表写入失败";
        result = "0";
      }
    } else {
      _ERROR = "font remove faile: write regedit faile."; // "注册表写入失败";
      result = "0";
    }
  }
#else
  _ERROR = "system is not win";
  result = "0";
#endif
  return String::New(info.Env(), result);
}

// 返回错误
String GetError(const CallbackInfo& info) {
  return String::New(info.Env(), _ERROR);
}

Napi::Object  Init(Env env, Object exports) {
  // exports.Set("addfont", Function::New(env, AddFont)); // addfont(字体文件地址)
  // exports.Set("delfont", Function::New(env, DeleteFont)); // delfont(字体文件地址)
  // exports.Set("readreg", Function::New(env, ReadReg)); // readreg(路径, 键名)都是字符串类型传入
  // exports.Set("savereg", Function::New(env, WriteReg)); // savereg(路径, 键名， 值， 类型)都是字符串类型传入
  // exports.Set("delreg", Function::New(env, DeleteReg)); // delreg(路径, 键名)都是字符串类型传入
  exports.Set("hasFont", Function::New(env, HasFont)); // 字体是否安装
  exports.Set("installFont", Function::New(env, InstallFont)); // 安装注册字体
  exports.Set("removeFont", Function::New(env, RemoveFont)); // 删除字体
  exports.Set("getError", Function::New(env, GetError)); // getError()
  return exports;
}

NODE_API_MODULE(addon, Init)
