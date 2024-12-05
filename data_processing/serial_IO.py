import serial
#需要发送的数据变量
#i: gyro变量：ax,ay,az,gx,gy,gz
#   开关变量：trigger trigger按下时间需要和电机转动时间匹配
#o: 是否击中：hit


# 设置串口连接
# 替换 '/dev/ttyACM0' 为你的 Arduino 串口，Windows 上通常是 'COM3' 或 'COM4'
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)  # 打开串口，波特率为 9600

# 给 Arduino 一些时间来初始化
arduino.flush()

while True:
    if arduino.in_waiting > 0:  # 如果串口有数据可读
        line = arduino.readline()  # 读取一行数据
        print(line.decode('utf-8').strip())  # 打印数据并去除换行符

