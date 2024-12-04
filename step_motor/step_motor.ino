// 定义控制信号引脚
int IN1 = 2;  // IN1连接到Arduino的2号引脚
int IN2 = 3;  // IN2连接到Arduino的3号引脚
int IN3 = 4; // IN3连接到Arduino的4号引脚
int IN4 = 5; // IN4连接到Arduino的5号引脚

class StepMotor {
public:
    enum Direction {
        CW, // 顺时针
        CCW // 逆时针
    };

    // 构造函数，初始化步进电机的基本参数
    StepMotor(int steps_per_revolution, int max_speed)
        : steps_per_revolution(steps_per_revolution), max_speed(max_speed),
          current_step(0), direction(CW), is_running(false) {}

    // 设置步进电机的转动方向
    void setDirection(Direction dir) {
        direction = dir;
    }

    // 设置步进电机的速度（步进信号的频率）
    void setSpeed(int rpm) {
        if (rpm > max_speed) {
            rpm = max_speed;
        }
        speed_delay = 60.0 / rpm / steps_per_revolution;
    }

    // 启动电机
    void start() {
        is_running = true;
        while (is_running) {
            step();
        }
    }

    // 停止电机
    void stop() {
        is_running = false;
    }

private:
    int steps_per_revolution;   // 每圈的步数
    int max_speed;              // 最大转速（每分钟的最大步数）
    int current_step;           // 当前步数
    Direction direction;        // 当前转动方向
    bool is_running;            // 电机是否正在运行
    double speed_delay;         // 控制速度的时间间隔（秒）

    int stepSequenceCW[4][4] = {
  {1, 1, 0, 0},  // 线圈1 和 线圈4 激活
  {1 ,0 ,1 ,0},  // 线圈2 和 线圈4 激活
  {0, 1, 0, 1},  // 线圈2 和 线圈3 激活
  {0, 0, 1, 1}   // 线圈1 和 线圈3 激活
  };

  // 定义步进信号序列（逆时针方向）
  int stepSequenceCCW[4][4] = {
    {0, 0, 1, 1},  // 线圈1 和 线圈3 激活
    {0, 1, 0, 1},  // 线圈2 和 线圈3 激活
    {1, 0, 1, 0},  // 线圈2 和 线圈4 激活
    {1, 1, 0, 0}   // 线圈1 和 线圈4 激活
  };

  void stepmotor_rotate(){
  // 控制顺时针旋转
  if(direction==CW){
      for (int i = 0; i < 4; i++) {
        digitalWrite(IN1, stepSequenceCW[i][0]);
        digitalWrite(IN2, stepSequenceCW[i][1]);
        digitalWrite(IN3, stepSequenceCW[i][2]);
        digitalWrite(IN4, stepSequenceCW[i][3]);
        delay(speed_delay);  // 步进时间（控制速度）
      }
  }else{
      for (int i = 0; i < 4; i++) {
        digitalWrite(IN1, stepSequenceCCW[i][0]);
        digitalWrite(IN2, stepSequenceCCW[i][1]);
        digitalWrite(IN3, stepSequenceCCW[i][2]);
        digitalWrite(IN4, stepSequenceCCW[i][3]);
        delay(speed_delay);  // 步进时间（控制速度）
      }
    }  
    }
    // 步进电机的执行步骤（模拟步进电机的步进过程）
    void step() {
        stepmotor_rotate();
        if (direction == CW) {
            current_step = (current_step + 1) % steps_per_revolution; // 顺时针
        } else {
            current_step = (current_step - 1 + steps_per_revolution) % steps_per_revolution; // 逆时针
        }

    }
};

StepMotor stepmotor(4096,14);

void setup() {
  // 初始化引脚为输出
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  stepmotor.setSpeed(10);
  stepmotor.start();
  delay(100);
  stepmotor.stop();
  delay(100);
  stepmotor.setDirection(StepMotor::CCW);
  stepmotor.setSpeed(5);
  stepmotor.start();
  delay(100);
  stepmotor.stop();
}





