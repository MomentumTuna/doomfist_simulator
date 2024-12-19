#include <Arduino.h>
#include <Wire.h>

// 定义按钮和马达的引脚
const int MPU_ADDR = 0x68;  // MPU6050 I2C 地址
const int buttonPin = 11;   // 按钮连接到引脚 11
const int motorPin = 10;    // 马达连接到引脚 10

// 定义变量存储按钮状态
int buttonState = 0;  // 按钮状态（0 = 未按下，1 = 按下）

// MPU6050 数据结构
typedef struct{
    int16_t AcX, AcY, AcZ; // 加速度
    int16_t GyX, GyY, GyZ; // 陀螺仪
}IMU;

IMU IMU_Data;

void setup() {
  Serial.begin(9600);  // 初始化串口通信

  // 初始化按钮和马达引脚
  pinMode(buttonPin, INPUT_PULLUP);  // 设置按钮引脚为输入，并启用内部上拉电阻
  pinMode(motorPin, OUTPUT);         // 设置马达引脚为输出

  // 启动时，确保马达关闭
  digitalWrite(motorPin, LOW);

  // 初始化 I2C 通信
  Wire.begin();

  // // 初始化 MPU6050
  // Wire.beginTransmission(MPU_ADDR);
  // Wire.write(0x6B);  // 访问电源管理寄存器
  // Wire.write(0);     // 设置为0，唤醒传感器
  // Wire.endTransmission(true);

  delay(100);  // 等待 100 毫秒
}

void loop() {
  // 读取按钮的状态
  buttonState = digitalRead(buttonPin);

  // 如果按钮被按下，启动震动马达
  if (buttonState == LOW) {
    Serial.println("Button pressed");
    digitalWrite(motorPin, HIGH);  // 启动震动马达
  } else {
    Serial.println("Button not pressed");
    digitalWrite(motorPin, LOW);   // 停止震动马达
  }

  // 读取 MPU6050 的加速度和陀螺仪数据
  // Wire.beginTransmission(MPU_ADDR);
  // Wire.write(0x3B);  // 加速度计和陀螺仪的数据起始地址
  // Wire.endTransmission(false);
  // Wire.requestFrom(MPU_ADDR, 14, true); // 请求 14 个字节的数据

  // // 读取加速度数据
  // IMU_Data.AcX = Wire.read() << 8 | Wire.read();  
  // IMU_Data.AcY = Wire.read() << 8 | Wire.read();
  // IMU_Data.AcZ = Wire.read() << 8 | Wire.read();

  // // 跳过温度数据
  // Wire.read();
  // Wire.read();

  // // 读取陀螺仪数据
  // IMU_Data.GyX = Wire.read() << 8 | Wire.read();
  // IMU_Data.GyY = Wire.read() << 8 | Wire.read();
  // IMU_Data.GyZ = Wire.read() << 8 | Wire.read();

  // // 输出加速度和陀螺仪数据
  // Serial.print("AcX: ");
  // Serial.print(IMU_Data.AcX);
  // Serial.print(" AcY: ");
  // Serial.print(IMU_Data.AcY);
  // Serial.print(" AcZ: ");
  // Serial.println(IMU_Data.AcZ);
  // Serial.print("GyX: ");
  // Serial.print(IMU_Data.GyX);
  // Serial.print(" GyY: ");
  // Serial.print(IMU_Data.GyY);
  // Serial.print(" GyZ: ");
  // Serial.println(IMU_Data.GyZ);

  delay(100);  // 延迟 100 毫秒
}
