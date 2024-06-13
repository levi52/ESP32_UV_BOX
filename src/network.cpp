#include "network.h"

extern int16_t brightness;

Preferences prefs; // 声明Preferences对象

// WIFI
String ssid = "";  // WIFI名称 
String password = "";  // WIFI密码 
String wifiNames;   // WIFI名称列表
IPAddress staticIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
const int webPort = 80; // 设置Web端口号
WebServer server(webPort);  // 开启web服务, 创建TCP SERVER,参数: 端口号,最大连接数
const char* AP_SSID  = "ESP_UV_BOX_WIFI";   // 设置AP热点名称
const char* HOST_NAME = "ESP_UV_BOX";   // 设置设备名
static HTTPClient httpClient;

// TIME
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.ntsc.ac.cn");//ntp.ntsc.ac.cn ntp.aliyun.com
time_t epochTime;
struct tm *ptm;
int currentWeek,currentHour,currentMinute,currentSecond;
int currentDay,currentMonth,currentYear; 
String wk[7] = {"周日","周一","周二","周三","周四","周五","周六"};

// BILIBILI
const String followURL = "https://api.bilibili.com/x/relation/stat?vmid=";
const String viewURL = "https://api.bilibili.com/x/space/upstat?mid=";
String uid = ""; // 
int followingCount;
int fansCount;
int likes;
int videoView;

// WEATHER
String key = ""; // 
const String cityURL = "https://geoapi.qweather.com/v2/city/lookup?key=";  // 查询城市代码的接口
const String nowURL = "https://devapi.qweather.com/v7/weather/now?key="; // 实时天气接口
const String futureURL = "https://devapi.qweather.com/v7/weather/7d?key="; // 7日天气预报接口
const String airURL = "https://devapi.qweather.com/v7/air/now?key="; // 空气质量接口
String location = ""; // 城市ID 
String city;  // 城市
String adm; // 上级城市区划
String data = "";
uint8_t *outbuf;
int queryTimeout = 6000; // 查询天气超时时间(ms)
bool queryNowWeatherSuccess = false;
bool queryFutureWeatherSuccess;
bool queryAirSuccess;
NowWeather nowWeather; // 记录查询到的实况天气数据
FutureWeather futureWeather; // 未来天气

// 配置WiFi的网页代码
const String ROOT_HTML_PAGE1 PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="content-type" content="text/html; charset=UTF-8" />
    <title>登录页面</title>
    <style>
        #content,
        .login,
        .login-card a,
        .login-card h1,
        .login-help {
            text-align: center
        }
        body,
        html {
            margin: 0;
            padding: 0;
            width: 100%;
            height: 100%;
            display: table
        }
        #content {
            font-family: 'Source Sans Pro', sans-serif;
            -webkit-background-size: cover;
            -moz-background-size: cover;
            -o-background-size: cover;
            background-size: cover;
            display: table-cell;
            vertical-align: middle
        }
        .login-card {
            padding: 40px;
            width: 274px;
            background-color: #F7F7F7;
            margin: 0 auto 10px;
            border-radius: 20px;
            box-shadow: 8px 8px 15px rgba(0, 0, 0, .3);
            overflow: hidden
        }
        .login-card h1 {
            font-weight: 400;
            font-size: 2.3em;
            color: #1383c6
        }
        .login-card h1 span {
            color: #f26721
        }
        .login-card img {
            width: 70%;
            height: 70%
        }
        .login-card input[type=submit] {
            width: 100%;
            display: block;
            margin-bottom: 10px;
            position: relative
        }
        .login-card input[type=text],
        input[type=password] {
            height: 44px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 10px;
            -webkit-appearance: none;
            background: #fff;
            border: 1px solid #d9d9d9;
            border-top: 1px solid silver;
            padding: 0 8px;
            box-sizing: border-box;
            -moz-box-sizing: border-box
        }
        .login-card input[type=text]:hover,
        input[type=password]:hover {
            border: 1px solid #b9b9b9;
            border-top: 1px solid #a0a0a0;
            -moz-box-shadow: inset 0 1px 2px rgba(0, 0, 0, .1);
            -webkit-box-shadow: inset 0 1px 2px rgba(0, 0, 0, .1);
            box-shadow: inset 0 1px 2px rgba(0, 0, 0, .1)
        }
        .login {
            font-size: 14px;
            font-family: Arial, sans-serif;
            font-weight: 700;
            height: 36px;
            padding: 0 8px
        }
        .login-submit {
            -webkit-appearance: none;
            -moz-appearance: none;
            appearance: none;
            border: 0;
            color: #fff;
            text-shadow: 0 1px rgba(0, 0, 0, .1);
            background-color: #4d90fe
        }
        .login-submit:disabled {
            opacity: .6
        }
        .login-submit:hover {
            border: 0;
            text-shadow: 0 1px rgba(0, 0, 0, .3);
            background-color: #357ae8
        }
        .login-card a {
            text-decoration: none;
            color: #666;
            font-weight: 400;
            display: inline-block;
            opacity: .6;
            transition: opacity ease .5s
        }
        .login-card a:hover {
            opacity: 1
        }
        .login-help {
            width: 100%;
            font-size: 12px
        }
        .list {
            list-style-type: none;
            padding: 0;
            display: flex
        }
        .list__item {
            margin: 0 0 .7rem;
            padding-left: 5px;
            color: #4d90fe;
        }
        label {
            display: -webkit-box;
            display: -webkit-flex;
            display: -ms-flexbox;
            display: flex;
            -webkit-box-align: center;
            -webkit-align-items: center;
            -ms-flex-align: center;
            align-items: center;
            text-align: left;
            font-size: 14px;
        }
        input[type=checkbox] {
            -webkit-box-flex: 0;
            -webkit-flex: none;
            -ms-flex: none;
            flex: none;
            margin-right: 10px;
            float: left
        }
        .error {
            font-size: 14px;
            font-family: Arial, sans-serif;
            font-weight: 700;
            height: 25px;
            padding: 0 8px;
            padding-top: 10px;
            -webkit-appearance: none;
            -moz-appearance: none;
            appearance: none;
            border: 0;
            color: #fff;
            text-shadow: 0 1px rgba(0, 0, 0, .1);
            background-color: #ff1215
        }
        @media screen and (max-width:450px) {
            .login-card {
                width: 70% !important
            }
            .login-card img {
                width: 30%;
                height: 30%
            }
        }
    </style>
</head>
<body style="background-color: #e5e9f2">
    <div id="content">
        <form name='input' action='/configwifi' method='POST'>
            <div class="login-card">
                <h1>ESP32 UV BOX</h1>
                <form name="login_form" method="post" action="$PORTAL_ACTION$">
                    <input type="text" name="ssid" placeholder="请输入WiFi 名称" id="ssid" list="data-list" ;
                        style="border-radius: 10px">
                    <datalist id="data-list">
)rawliteral";
const String ROOT_HTML_PAGE2 PROGMEM = R"rawliteral(
                    <input type="password" name="password" placeholder="请输入WiFi 密码" id="password" ;
                        style="border-radius: 10px">
                    <input type="text" name="key" placeholder="请输入Private KEY" id="key" ;
                        style="border-radius: 10px">
                    <input type="text" name="location" placeholder="请输入Location ID" id="location" ;
                        style="border-radius: 10px">
                    <input type="text" name="uid" placeholder="请输入bilibili UID" id="uid" ;
                        style="border-radius: 10px">
                    <div class="login-help">
                        <ul class="list">
                            <li class="list__item">
                                <a target="_blank" href="https://id.qweather.com/#/login?redirect=https%3A%2F%2Fconsole.qweather.com%2F%23%2Fapps&lang=zh">和风天气</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://docs.qq.com/sheet/DTWNWWU9FeXhwSERG?tab=BB08J2">Location ID</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://space.bilibili.com/378576508">bilibili</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://oshwhub.com/levi_01">oshwhub</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://levi52.github.io/">blog</a>
                            </li>
                            
                        </ul>
                    </div>
                    <input type="submit" class="login login-submit" value="保 存" id="login" ; disabled;
                        style="border-radius: 15px">
                </form>
            </div>
        </form>
    </div>
</body>
</html>
)rawliteral";

/**
 * @name set_info
 * @brief 保存数据
*/
void set_info(void)
{
    prefs.begin("project");
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
    prefs.putString("key", key);
    prefs.putString("location", location);
    prefs.putString("uid", uid);
    prefs.putInt("brightness", brightness);
    prefs.end();
}

/**
 * @name get_info
 * @brief 读取数据
*/
void get_info(void)
{
    prefs.begin("project");
    ssid = prefs.getString("ssid", "");
    password = prefs.getString("password", "");
    key = prefs.getString("key", "");
    location = prefs.getString("location", "");
    uid = prefs.getString("uid", "");
    brightness = prefs.getInt("brightness", 10);
    prefs.end();
}

/**
 * @name wifiConfigBySoftAP
 * @brief AP配置WiFi
*/
void wifiConfigBySoftAP(void)
{
    // 开启AP模式
    ap_init();
    // 扫描WiFi,并将扫描到的WiFi组成option选项字符串
    wifi_scan();
    // 启动服务器
    server_init();
}

/**
 * @name wifi_connect
 * @brief 连接WiFi
*/
void wifi_connect(int timeOut_s)
{
    WiFi.hostname(HOST_NAME);   // 设置设备名
    Serial.println("进入connectToWiFi()函数");
    WiFi.mode(WIFI_STA);    // 设置为STA模式并连接WIFI
    WiFi.setAutoConnect(true);  // 设置自动连接
    if(ssid != "") //ssid不为空
    {
        Serial.println("用web配置信息连接.");
        WiFi.begin(ssid.c_str(), password.c_str()); // c_str(),获取该字符串的指针
        ssid = "";
        password = "";
    } 
    else
    {
        Serial.println("用nvs保存的信息连接.");
        WiFi.begin();   // begin()不传入参数，默认连接上一次连接成功的wifi
    }
    int connectTime = 0;    // 用于连接计时，如果长时间连接不成功，复位设备
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        connectTime++;
        if(connectTime > 2 * timeOut_s)
        {
            Serial.println("网络连接失败");
            wifiConfigBySoftAP();
            return;
        }
    }
    if(WiFi.status() == WL_CONNECTED)
    {
        Serial.println("网络连接成功");
        Serial.printf("SSID:%s", WiFi.SSID().c_str());
        Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
        Serial.print("LocalIP:");
        Serial.print(WiFi.localIP());
        Serial.print(" ,GateIP:");
        Serial.println(WiFi.gatewayIP());
        // Serial.print(WiFi.status());
        server.stop();  // 停止开发板所建立的网络服务器。
    }
}

/**
 * @name wifi_check
 * @brief 检查WiFi是否连接成功
 * @return bool
*/
bool wifi_check(void)
{
    if(WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi连接失败");
        return false;
    }
    else
    {
        Serial.println("WiFi连接成功");
        return true;
    }
}

/**
 * @name wifi_disconnect
 * @brief 断开WiFi
*/
void wifi_disconnect(void)
{
    WiFi.disconnect();
}

/**
 * @name wifi_init
 * @brief 扫描网络
*/
bool wifi_scan(void)
{
    Serial.println("scan start");
    Serial.println("--------->");
    // 扫描附近WiFi
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if(n == 0) 
    {
        Serial.println("no networks found");
        wifiNames += "<option>no networks found</option>";
        return false;
    } 
    else 
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) 
        {
            // Print SSID and RSSI for each network found
            int32_t rssi = WiFi.RSSI(i);
            // Serial.print(i + 1);
            // Serial.print(": ");
            // Serial.print(WiFi.SSID(i));
            // Serial.print(" (");
            // Serial.print(rssi);
            // Serial.print(")");
            String signalStrength;
            if(rssi >= -35)
            {
                signalStrength = " (信号极强)";
            }
            else if(rssi >= -50)
            {
                signalStrength = " (信号强)";
            }else if(rssi >= -70)
            {
                signalStrength = " (信号中)";
            }
            else
            {
                signalStrength = " (信号弱)";
            }
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
            wifiNames += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + signalStrength + "</option>";
            delay(10);
        }
        wifiNames += "</datalist>";
        return true;
    }
}

/**
 * @name ap_init
 * @brief 初始化AP模式
*/
void ap_init(void)
{
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(staticIP, gateway, subnet);
    if(WiFi.softAP(AP_SSID))   // 开启AP热点,如需要密码则添加第二个参数
    {                           
        Serial.print("Soft AP IP address = ");
        Serial.println(WiFi.softAPIP());    // 接入点ip
        Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());    // 接入点mac
    } 
    else
    { 
        Serial.println("WiFi AP Failed");
        delay(1000);
        Serial.println("restart now...");
        ESP.restart();  // 重启复位esp32
    }
}

/**
 * @name doClient
 * @brief 处理客户端请求
*/
void doClient(void)
{
    server.handleClient();
}

/**
 * @name server_init
 * @brief 初始化服务器
*/
void server_init(void)
{
    server.on("/", HTTP_GET, handleRoot);   // 当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
    server.on("/configwifi", HTTP_POST, handleConfigWifi);  // 当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
    server.onNotFound(handleNotFound);  //当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
    server.begin(); //启动TCP SERVER
    Serial.println("WebServer started!");
}

/**
 * @name handleRoot
 * @brief 处理根目录请求
*/
void handleRoot(void)
{
    server.send(200, "text/html", ROOT_HTML_PAGE1 + wifiNames + ROOT_HTML_PAGE2);   
}

/**
 * @name handleConfigWifi
 * @brief 处理配置WiFi请求
*/
void handleConfigWifi(void) // 返回http状态
{
    // 判断是否有WiFi名称f
    if(server.hasArg("ssid"))
    {
        Serial.print("获得WiFi名称:");
        ssid = server.arg("ssid");
        Serial.println(ssid);
    }
    else
    {
        Serial.println("错误, 没有发现WiFi名称");
        server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现WiFi名称");
        return;
    }
    // 判断是否有WiFi密码
    if(server.hasArg("password"))
    {
        Serial.print("获得WiFi密码:");
        password = server.arg("password");
        Serial.println(password);
    }
    else
    {
        Serial.println("错误, 没有发现WiFi密码");
        server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现WiFi密码");
        return;
    }
    // 判断是否有KEY
    if(server.hasArg("key"))
    {
        Serial.print("获得和风天气 KEY:");
        key = server.arg("key");
        Serial.println(key);
    }
    else
    {
        Serial.println("错误, 没有发现和风天气 KEY");
        server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现和风天气 KEY");
        return;
    }
    // 判断是否有location
    if(server.hasArg("location"))
    {
        Serial.print("获得位置 Location ID:");
        location = server.arg("location");
        Serial.println(location);
    }
    else
    {
        Serial.println("错误, 没有发现位置 Location ID");
        server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现位置 Location ID");
        return;
    }
    // 判断是否有uid
    if(server.hasArg("uid"))
    {
        Serial.print("获得bilibili UID:");
        uid = server.arg("uid");
        Serial.println(uid);
    }
    else
    {
        Serial.println("错误, 没有发现bilibili UID");
        server.send(200, "text/html", "<meta charset='UTF-8'>错误, 没有发现bilibili UID");
        return;
    }
    // 获得了所需要的一切信息，给客户端回复
    server.send(200, "text/html", "<meta charset='UTF-8'><style type='text/css'>body {font-size: 2rem;}</style><br/><br/>WiFi: " + ssid + "<br/>密码: " + password + "<br/>KEY: " + key + "<br/>location: " + location + "<br/>uid: " + uid + "<br/>已取得相关信息,即将尝试连接,请手动关闭此页面。");
    delay(1500);
    WiFi.softAPdisconnect(true);    // 参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
    server.close(); // 关闭web服务
    WiFi.softAPdisconnect();    // 在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
    set_info();
    delay(1500);
    ESP.restart();
}

/**
 * @name handleNotFound
 * @brief 处理未找到请求
*/
void handleNotFound(void)   // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
{           
  handleRoot(); // 访问不存在目录则返回配置页面
}

/**
 * @name time_init
 * @brief ntp初始化
*/
void time_init(void)
{
    timeClient.begin();
    timeClient.setTimeOffset(28800);  // 8区 8*3600
    timeClient.update();
}

/**
 * @name get_time
 * @brief NTP获取当前时间
*/
void get_time(void)
{
    timeClient.update();  // 更新时间
    epochTime = timeClient.getEpochTime();
    ptm = gmtime(&epochTime);//(time_t*)&epochTime
    currentYear = ptm->tm_year+1900;//-100
    currentMonth = ptm->tm_mon+1;
    currentDay = ptm->tm_mday;
    currentWeek = timeClient.getDay();
    currentHour = timeClient.getHours();
    currentMinute = timeClient.getMinutes();
    currentSecond = timeClient.getSeconds();
}

/**
 * @name get_weather_now
 * @brief 获取当前天气
*/
void get_weather_now(void)
{
    data = "";
    queryNowWeatherSuccess = false; // 先置为false
    String url = nowURL + key + "&location=" + location;
    httpClient.setConnectTimeout(queryTimeout);
    httpClient.begin(url);
    //启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    // Serial.println(ESP.getFreeHeap());
    Serial.println("正在获取天气数据");
    //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
    if(httpCode == HTTP_CODE_OK) {
        // 解压Gzip数据流
        int len = httpClient.getSize();
        uint8_t buff[2048] = { 0 };
        WiFiClient *stream = httpClient.getStreamPtr();
        while (httpClient.connected() && (len > 0 || len == -1)) {
            size_t size = stream->available();  // 还剩下多少数据没有读完？
            // Serial.println(size);
            if(size) {
            size_t realsize = ((size > sizeof(buff)) ? sizeof(buff) : size);
            // Serial.println(realsize);
            size_t readBytesSize = stream->readBytes(buff, realsize);
            // Serial.write(buff,readBytesSize);
            if(len > 0) len -= readBytesSize;
            outbuf = (uint8_t *)malloc(sizeof(uint8_t) * 5120);
            uint32_t outprintsize = 0;
            int result = ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 5120, outprintsize);
            // Serial.write(outbuf, outprintsize);
            for (int i = 0; i < outprintsize; i++) {
                data += (char)outbuf[i];
            }
            free(outbuf);
            // Serial.println(data);
            }
            delay(1);
        }
        // 解压完，转换json数据
        StaticJsonDocument<2048> doc; //声明一个静态JsonDocument对象
        DeserializationError error = deserializeJson(doc, data); //反序列化JSON数据
        if(!error){ //检查反序列化是否成功
            //读取json节点
            String code = doc["code"].as<const char*>();
            if(code.equals("200")){
                queryNowWeatherSuccess = true;       
                //读取json节点
                nowWeather.text = doc["now"]["text"].as<const char*>(); // 天气类型
                nowWeather.icon = doc["now"]["icon"].as<int>(); // 天气图标
                nowWeather.temp = doc["now"]["temp"].as<int>(); // 温度
                String feelslikes = doc["now"]["feelsLike"]; // 体感温度
                nowWeather.feelsLike = feelslikes  + "℃";
                String windDirs = doc["now"]["windDir"]; // 风向
                nowWeather.windDir = windDirs;
                String windScales = doc["now"]["windScale"]; // 风力等级
                nowWeather.windScale = windScales + "级";
                nowWeather.humidity = doc["now"]["humidity"].as<int>(); // 湿度
                nowWeather.vis = doc["now"]["vis"].as<int>(); // 能见度
                nowWeather.pressure = doc["now"]["pressure"].as<int>(); // 大气压强
                Serial.println("获取天气成功");
            }
        }  
    }
    if(!queryNowWeatherSuccess){
    Serial.print("请求实时天气错误：");
    Serial.println(httpCode);
    }
    //关闭与服务器连接
    httpClient.end();
}

/**
 * @name get_weather_future
 * @brief 获取未来天气
*/
void get_weather_future(void)
{
    data = "";
    queryFutureWeatherSuccess = false; // 先置为false
    String url = futureURL + key + "&location=" + location;
    httpClient.setConnectTimeout(queryTimeout);
    httpClient.begin(url);
    //启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    Serial.println("正在获取一周天气数据");
    //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
    if(httpCode == HTTP_CODE_OK) 
    {
        // 解压Gzip数据流
        int len = httpClient.getSize();
        uint8_t buff[2048] = { 0 };
        WiFiClient *stream = httpClient.getStreamPtr();
        while (httpClient.connected() && (len > 0 || len == -1)) 
        {
            size_t size = stream->available();  // 还剩下多少数据没有读完？
            // Serial.println(size);
            if(size) 
            {
                size_t realsize = ((size > sizeof(buff)) ? sizeof(buff) : size);
                // Serial.println(realsize);
                size_t readBytesSize = stream->readBytes(buff, realsize);
                // Serial.write(buff,readBytesSize);
                if(len > 0) len -= readBytesSize;
                outbuf = (uint8_t *)malloc(sizeof(uint8_t) * 5120);
                uint32_t outprintsize = 0;
                int result = ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 5120, outprintsize);
                // Serial.write(outbuf, outprintsize);
                for (int i = 0; i < outprintsize; i++) 
                {
                    data += (char)outbuf[i];
                }
                free(outbuf);
                // Serial.println(data);
            }
            delay(1);
        }
        // 解压完，转换json数据
        StaticJsonDocument<2048> doc; //声明一个静态JsonDocument对象
        DeserializationError error = deserializeJson(doc, data); //反序列化JSON数据
        if(!error)
        { //检查反序列化是否成功
            //读取json节点
            String code = doc["code"].as<const char*>();
            if(code.equals("200"))
            {
                queryFutureWeatherSuccess = true;
                //读取json节点
                futureWeather.day0wea = doc["daily"][0]["textDay"].as<const char*>();
                futureWeather.day0wea_img = doc["daily"][0]["iconDay"].as<int>();
                futureWeather.day0date = doc["daily"][0]["fxDate"].as<const char*>();
                futureWeather.day0tem_day = doc["daily"][0]["tempMax"].as<int>();
                futureWeather.day0tem_night = doc["daily"][0]["tempMin"].as<int>();

                futureWeather.day1wea = doc["daily"][1]["textDay"].as<const char*>();
                futureWeather.day1wea_img = doc["daily"][1]["iconDay"].as<int>();
                futureWeather.day1date = doc["daily"][1]["fxDate"].as<const char*>();
                futureWeather.day1tem_day = doc["daily"][1]["tempMax"].as<int>();
                futureWeather.day1tem_night = doc["daily"][1]["tempMin"].as<int>();

                futureWeather.day2wea = doc["daily"][2]["textDay"].as<const char*>();
                futureWeather.day2wea_img = doc["daily"][2]["iconDay"].as<int>();
                futureWeather.day2date = doc["daily"][2]["fxDate"].as<const char*>();
                futureWeather.day2tem_day = doc["daily"][2]["tempMax"].as<int>();
                futureWeather.day2tem_night = doc["daily"][2]["tempMin"].as<int>();

                futureWeather.day3wea = doc["daily"][3]["textDay"].as<const char*>();
                futureWeather.day3wea_img = doc["daily"][3]["iconDay"].as<int>();
                futureWeather.day3date = doc["daily"][3]["fxDate"].as<const char*>();
                futureWeather.day3tem_day = doc["daily"][3]["tempMax"].as<int>();
                futureWeather.day3tem_night = doc["daily"][3]["tempMin"].as<int>();

                futureWeather.day4wea = doc["daily"][4]["textDay"].as<const char*>();
                futureWeather.day4wea_img = doc["daily"][4]["iconDay"].as<int>();
                futureWeather.day4date = doc["daily"][4]["fxDate"].as<const char*>();
                futureWeather.day4tem_day = doc["daily"][4]["tempMax"].as<int>();
                futureWeather.day4tem_night = doc["daily"][4]["tempMin"].as<int>();

                futureWeather.day5wea = doc["daily"][5]["textDay"].as<const char*>();
                futureWeather.day5wea_img = doc["daily"][5]["iconDay"].as<int>();
                futureWeather.day5date = doc["daily"][5]["fxDate"].as<const char*>();
                futureWeather.day5tem_day = doc["daily"][5]["tempMax"].as<int>();
                futureWeather.day5tem_night = doc["daily"][5]["tempMin"].as<int>();

                futureWeather.day6wea = doc["daily"][6]["textDay"].as<const char*>();
                futureWeather.day6wea_img = doc["daily"][6]["iconDay"].as<int>();
                futureWeather.day6date = doc["daily"][6]["fxDate"].as<const char*>();
                futureWeather.day6tem_day = doc["daily"][6]["tempMax"].as<int>();
                futureWeather.day6tem_night = doc["daily"][6]["tempMin"].as<int>();

                Serial.println("获取未来天气成功");
            }
        }  
    } 
    if(!queryFutureWeatherSuccess)
    {
        Serial.print("请求一周天气错误：");
        Serial.println(httpCode);
    }
    //关闭与服务器连接
    httpClient.end();
}

/**
 * @name get_air
 * @brief 获取空气质量
*/
void get_air(void)
{
    data = "";
    queryAirSuccess = false; // 先置为false
    String url = airURL + key + "&location=" + location;
    httpClient.setConnectTimeout(queryTimeout);
    httpClient.begin(url);
    //启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    Serial.println("正在获取空气质量数据");
    //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
    if(httpCode == HTTP_CODE_OK) 
    {
        // 解压Gzip数据流
        int len = httpClient.getSize();
        uint8_t buff[2048] = { 0 };
        WiFiClient *stream = httpClient.getStreamPtr();
        while (httpClient.connected() && (len > 0 || len == -1)) 
        {
            size_t size = stream->available();  // 还剩下多少数据没有读完？
            // Serial.println(size);
            if(size) 
            {
                size_t realsize = ((size > sizeof(buff)) ? sizeof(buff) : size);
                // Serial.println(realsize);
                size_t readBytesSize = stream->readBytes(buff, realsize);
                // Serial.write(buff,readBytesSize);
                if(len > 0) len -= readBytesSize;
                outbuf = (uint8_t *)malloc(sizeof(uint8_t) * 5120);
                uint32_t outprintsize = 0;
                int result = ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 5120, outprintsize);
                // Serial.write(outbuf, outprintsize);
                for (int i = 0; i < outprintsize; i++) 
                {
                    data += (char)outbuf[i];
                }
                free(outbuf);
                // Serial.println(data);
            }
            delay(1);
        }
        // 解压完，转换json数据
        StaticJsonDocument<2048> doc; //声明一个静态JsonDocument对象
        DeserializationError error = deserializeJson(doc, data); //反序列化JSON数据
        if(!error)
        { //检查反序列化是否成功
            //读取json节点
            String code = doc["code"].as<const char*>();
            if(code.equals("200"))
            {
                queryAirSuccess = true;
                //读取json节点
                nowWeather.air = doc["now"]["aqi"].as<int>();
                nowWeather.category = doc["now"]["category"].as<const char*>();
                nowWeather.pm10 = doc["now"]["pm10"].as<const char*>();
                nowWeather.pm2p5 = doc["now"]["pm2p5"].as<const char*>();
                nowWeather.no2 = doc["now"]["no2"].as<const char*>();
                nowWeather.so2 = doc["now"]["so2"].as<const char*>();
                nowWeather.co = doc["now"]["co"].as<const char*>();
                nowWeather.o3 = doc["now"]["o3"].as<const char*>();
                Serial.println("获取空气质量成功");
            }
        }
    } 
    if(!queryAirSuccess)
    {
        Serial.print("请求空气质量错误：");
        Serial.println(httpCode);
    }
    //关闭与服务器连接
    httpClient.end();
}

/**
 * @name get_city_id
 * @brief 查询城市ID
*/
void get_city_id(void)
{
    bool flag = false; // 是否成功获取到城市id的标志
    String url = cityURL + key + "&location=" + urlEncode(city) + "&adm=" + urlEncode(adm);
    // Serial.println(url);
    httpClient.setConnectTimeout(queryTimeout * 5);
    httpClient.begin(url);
    //启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    Serial.println("正在获取城市id");
    // 处理服务器答复
    if(httpCode == HTTP_CODE_OK) 
    {
        // 解压Gzip数据流
        int len = httpClient.getSize();
        uint8_t buff[2048] = { 0 };
        WiFiClient *stream = httpClient.getStreamPtr();
        while (httpClient.connected() && (len > 0 || len == -1)) 
        {
            size_t size = stream->available();  // 还剩下多少数据没有读完？
            // Serial.println(size);
            if(size) 
            {
                size_t realsize = ((size > sizeof(buff)) ? sizeof(buff) : size);
                // Serial.println(realsize);
                size_t readBytesSize = stream->readBytes(buff, realsize);
                // Serial.write(buff,readBytesSize);
                if(len > 0) len -= readBytesSize;
                outbuf = (uint8_t *)malloc(sizeof(uint8_t) * 5120);
                uint32_t outprintsize = 0;
                int result = ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 5120, outprintsize);
                // Serial.write(outbuf, outprintsize);
                for (int i = 0; i < outprintsize; i++) 
                {
                    data += (char)outbuf[i];
                }
                free(outbuf);
                // Serial.println(data);
            }
            delay(1);
        }
        // 解压完，转换json数据
        StaticJsonDocument<2048> doc; //声明一个静态JsonDocument对象
        DeserializationError error = deserializeJson(doc, data); //反序列化JSON数据
        if(!error)
        { //检查反序列化是否成功
            //读取json节点
            String code = doc["code"].as<const char*>();
            if(code.equals("200"))
            {
                flag = true;
                // 多结果的情况下，取第一个
                city = doc["location"][0]["name"].as<const char*>();
                location = doc["location"][0]["id"].as<const char*>();
                Serial.println("城市id :" + location);
                // 将信息存入nvs中
                // setWiFiCity();
            }
        }
    }
    if(!flag)
    {
        Serial.print("获取城市id错误: ");
        Serial.println(httpCode);
        Serial.print("城市错误，即将重启系统");
        // clearWiFiCity(); // 清除配置好的信息
        // restartSystem("城市名称无效", false);
    }
    Serial.println("获取成功");
    //关闭与服务器连接
    httpClient.end();
}

/**
 * @name get_biliFollow
 * @brief 获取B站关注数和粉丝数
*/
void get_biliFollow(void)
{
    String url = followURL + uid;
    httpClient.setConnectTimeout(queryTimeout * 5);
    httpClient.begin(url);
    // 启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    Serial.println("正在获取B站信息");
    // 处理服务器答复
    if(httpCode == HTTP_CODE_OK) 
    {
        String response = httpClient.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);
        // if(error) {
        //     Serial.print("deserializeJson() failed: ");
        //     Serial.println(error.c_str());
        //     return;
        // }
        if(!error)
        {
            JsonObject data = doc["data"];
            // long data_mid = data["mid"]; // 378576508
            followingCount = data["following"]; // 318
            fansCount = data["follower"]; // 56
            Serial.print("following: ");
            Serial.println(followingCount);
            Serial.print("follower: ");
            Serial.println(fansCount);
        }
    }
    else{
        Serial.print("B站信息获取失败");
        Serial.println(httpCode);
    }
}

/**
 * @name get_biliView
 * @brief 获取B站播放量和点赞量(暂时为0)
*/
void get_biliView(void)
{
    String url = viewURL + uid;
    httpClient.setConnectTimeout(queryTimeout * 5);
    httpClient.begin(url);
    //启动连接并发送HTTP请求
    int httpCode = httpClient.GET();
    Serial.println("正在获取B站信息");
    // 处理服务器答复
    if(httpCode == HTTP_CODE_OK) 
    {
        String response = httpClient.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);
        // if(error)
        // {
        //     Serial.print("deserializeJson() failed: ");
        //     Serial.println(error.c_str());
        //     return;
        // }
        if(!error)
        {
            JsonObject data = doc["data"];
            JsonObject data_archive = data["archive"];
            videoView = data_archive["view"]; // 10824
            likes = data["likes"]; // 546
            Serial.print("videoView: ");
            Serial.println(videoView);
            Serial.print("likes: ");
            Serial.println(likes);
        }
    }
    else
    {
        Serial.print("B站信息获取失败");
        Serial.println(httpCode);
    }
}

String urlEncode(const String& text) 
{
    String encodedText = "";
    for (size_t i = 0; i < text.length(); i++) 
    {
        char c = text[i];
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') 
        {
            encodedText += c;
        } 
        else if(c == ' ') 
        {
            encodedText += '+';
        } 
        else 
        {
            encodedText += '%';
            char hex[4];
            sprintf(hex, "%02X", (uint8_t)c);
            encodedText += hex;
        }
    }
    return encodedText;
}

