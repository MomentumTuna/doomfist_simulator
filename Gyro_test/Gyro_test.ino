#include <Wire.h> // 引入I2C通信库

// MPU6050 地址
const int MPU_ADDR = 0x68;

// 存储加速度和陀螺仪数据的变量
int16_t AcX, AcY, AcZ; // 加速度
int16_t GyX, GyY, GyZ; // 陀螺仪

void setup() {
  Wire.begin(); // 初始化I2C通信
  Serial.begin(9600); // 初始化串口通信，用于调试输出

  // 初始化 MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // 访问电源管理寄存器
  Wire.write(0);    // 设置为0唤醒传感器
  Wire.endTransmission(true);

  Serial.println("MPU6050 初始化完成");
}

void loop() {
  // 读取加速度和陀螺仪数据
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // 从寄存器地址 0x3B 开始读取数据
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true); // 请求14字节数据

  // 读取加速度数据
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // 跳过温度数据（可选）
  Wire.read();
  Wire.read();

  // 读取陀螺仪数据
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // 输出数据到串口监视器
  Serial.print("加速度X: "); Serial.print(AcX);
  Serial.print(" 加速度Y: "); Serial.print(AcY);
  Serial.print(" 加速度Z: "); Serial.println(AcZ);

  Serial.print("陀螺仪X: "); Serial.print(GyX);
  Serial.print(" 陀螺仪Y: "); Serial.print(GyY);
  Serial.print(" 陀螺仪Z: "); Serial.println(GyZ);

  delay(500); // 每隔 500ms 读取一次
}
