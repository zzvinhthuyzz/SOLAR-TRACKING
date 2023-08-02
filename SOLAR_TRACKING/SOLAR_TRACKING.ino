

//======================================
//        PHẦN CHỈNH SỬA
//======================================
#define ENDSTOP_MAX_PIN     9
#define ENDSTOP_MIN_PIN     8
#define INVERT_ENDSTOP_MAX  true
#define INVERT_ENDSTOP_MIN  true
#define MODE_AB
//#define MODE_0_1
#define HOZ_A 10
#define HOZ_B 10
int TRIG_AB[2] = { 0, 0 };
#ifdef MODE_0_1
int trig[4] = { 1000, 1000, 1000, 1000 };
#endif

// =====================================
//        PHẦN CHỈNH SỬA
//======================================
#define N 40
float min_value[4] = { 100.0, 100.0, 100.0, 100.0 };
int max_value[4] = { 2430, 2593, 2792, 2477 };
float ref[4] = { 671.0, 750.0, 853.0, 693.0 };
int sensor_pin[4] = { A1, A0, A2, A3 };
int angle_pin[2] = { A4, A5 };
int offset_angle[2] = { 0, 0 };
int angle[2];
int sensor[4], valA[2], valB[2], center, value[4];
float sensor_float[4];

#define DIR_A 1
#define DIR_B 0
#define ENA 8
#define ENB 13
#define IN1 9
#define IN2 10
#define IN3 12
#define IN4 11
#define A_AXIS 1
#define B_AXIS 0
#define ENDSTOP 0
#define INVERT_ENDSTOP  1
#define MAX  0
#define MIN  1
bool read_endstop(int VAR) {
  static bool mapping[2][2] = {
    { ENDSTOP_MAX_PIN, ENDSTOP_MAX_PIN},
    {INVERT_ENDSTOP_MAX, INVERT_ENDSTOP_MIN}
  };
  int  AXIS_ENSTOP_PIN = mapping[VAR][ENDSTOP];
  bool INVERT_LOGIC   = mapping[VAR][INVERT_ENDSTOP];
  bool logic = digitalRead(AXIS_ENSTOP_PIN);
  logic = INVERT_LOGIC ? !logic : logic;
  return logic;
}

void Drive_MotorA(bool a, bool b) {
  digitalWrite(IN1, a);
  digitalWrite(IN2, b);
}

void Drive_MotorB(bool a, bool b) {
  digitalWrite(IN3, a);
  digitalWrite(IN4, b);
}

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  Serial.begin(9600);
}
void loop() {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < 4; j++) {
      sensor_float[j] += (float)analogRead(sensor_pin[j]);
    }
  }
  for (int i = 0; i < 2; i++) {
    angle[i] = map(analogRead(angle_pin[i]), 0, 1024, 0, 270);
  }

  for (int i = 0; i < 4; i++) {
    sensor_float[i] = sensor_float[i] / (float)N;
    sensor[i] = (int)sensor_float[i];
    //sensor[i] = map(sensor[i], 0, max_value[i], 0, 1000) - off_set[i];
#ifdef MODE_0_1
    value[i] = sensor[i] > trig[i] ? 1 : 0;
#else
    value[i] = sensor[i];
#endif
  }



  Serial.print("S[0->3] = [");
  Serial.print(sensor[0]);
  Serial.print(" , ");
  Serial.print(sensor[1]);
  Serial.print(" , ");
  Serial.print(sensor[2]);
  Serial.print(" , ");
  Serial.print(sensor[3]);
  Serial.print(" ]   ");

#ifdef MODE_0_1
  Serial.print("Value[0->3] = [");
  Serial.print(value[0]);
  Serial.print(" , ");
  Serial.print(value[1]);
  Serial.print(" , ");
  Serial.print(value[2]);
  Serial.print(" , ");
  Serial.print(value[3]);
  Serial.print(" ]   ");
#endif

  /*
    [0]-------------[1]
     |               |
     | SOLAR SENSORS |
     |               |
    [2]-------------[3]

    B[0] = ([0] + [1])/2
    B[1] = ([2] + [3])/2

    A[0] = ([0] + [2])/2
    A[1] = ([1] + [3])/2

    A[0] > A[1]
    B[0] > B[1]
  */
  angle[A_AXIS] = 50;
  valB[0] = (value[0] + value[1]) / 2 + TRIG_AB[1];
  valB[1] = (value[2] + value[3]) / 2 - TRIG_AB[1];

  valA[0] = (value[0] + value[2]) / 2 + TRIG_AB[0];
  valA[1] = (value[1] + value[3]) / 2 - TRIG_AB[0];

  Serial.print("val_A = [");
  Serial.print(valA[0]);
  Serial.print(" , ");
  Serial.print(valA[1]);
  Serial.print(" ]   ");
  Serial.print("val_B = [");
  Serial.print(valB[0]);
  Serial.print(" , ");
  Serial.print(valB[1]);
  Serial.print(" ]   ");


  if ((valA[0] - valA[1] > HOZ_A)) {
    Drive_MotorA(!DIR_A, DIR_A);
    Serial.print(" A=RIGHT");
    Serial.print("     ");
  } else if ((valA[0] - valA[1] < -HOZ_A)) {
    Drive_MotorA(DIR_A, !DIR_A);
    Serial.print(" A=LEFT");
    Serial.print("     ");
  } else {
    Drive_MotorA(0, 0);
    Serial.print(" A=STOP");
    Serial.print("     ");
  }

  if ((valB[0] - valB[1] > HOZ_B)) {
    Drive_MotorB(!DIR_B, DIR_B);
    Serial.print(" B=UP ");
    Serial.print("     ");
  } else if ((valB[0] - valB[1] < -HOZ_B)) {
    Drive_MotorB(DIR_B, !DIR_B);
    Serial.print(" B=DOWN ");
    Serial.print("     ");
  } else {
    Drive_MotorB(0, 0);
    Serial.print(" B=STOP ");
    Serial.print("     ");
  }

  Serial.println();
  delay(100);
}
