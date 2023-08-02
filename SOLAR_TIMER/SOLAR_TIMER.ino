#include "config.h"
#include "config_adv.h"
#include "Data_Table.h"
#include "DS1307.h"
#include <avr/pgmspace.h>
DS1307 rtc;
String time, axis, data, table;
int temp = 0;
int n = 0;
bool state_p = RUN;
int year = YEAR,
    month = MONTH,
    day = DAY,
    hour = HOUR,
    min = MINUTE,
    sec = SECOND;
long step_now[2], step_setpoint[2];
float angle_now[2], angle_setpoint[2], angle_target[2], d_angle[2];
bool is_home = false;



//==============================================================
//                           MAPPING
//==============================================================
int mapping(int AXIS_P, int TYPE) {
  static int MAPPING_[7][2] =
  { { DIR_A_PIN        , DIR_B_PIN         },
    { ENDSTOP_A_PIN    , ENDSTOP_B_PIN     },
    { STEP_A_PIN       , STEP_B_PIN        },
    { INVERT_DIR_A     , INVERT_DIR_B      },
    { INVERT_ENDSTOP_A , INVERT_ENDSTOP_B  },
    { INVERT_HOME_DIR_A, INVERT_HOME_DIR_B },
    { STEP_PER_ANGLE_A , STEP_PER_ANGLE_B  },
  };
  return MAPPING_[TYPE][AXIS_P];
}


//==============================================================
//                        MAPPING FLOAT
//==============================================================
float mapping_float(int AXIS_P, int TYPE) {
  static float MAPPING[2][2] = {
    { MAX_ANGLE_A      ,      MAX_ANGLE_B  },
    { MIN_ANGLE_A      ,      MIN_ANGLE_B  }
  };
  return MAPPING[TYPE][AXIS_P];
}


//==============================================================
//                        GET_DAY_OF_YEAR
//==============================================================
int get_day_of_year(int d, int m, int y) {
  // 1   2   3   4   5   6   7   8   9  10  11  12
  static int num_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int sum = 0;
  for (int i = 0; i < m - 1; i++) {
    sum += num_day[i];
  }
  //  if (y % 4 == 0 && m > 1)sum = sum + d + 1 ;
  //  else sum = sum + d;
  sum = sum + d;
  return sum;
}

//==============================================================
//                        GOTO_HOME
//==============================================================
void goto_home(int AXIS_P) {
  unsigned long time_now = millis();
  int AXIS_PIN = AXIS_P;
  set_dir(AXIS_P, mapping(AXIS_P, INVERT_HOME_DIR));
  while (millis() - time_now < BREAK_TIME * 1000 && !read_endstop(AXIS_P)) {
    move_axis_step(AXIS_P);
  }
  float angle_home[2] = {ANGLE_HOME_A, ANGLE_HOME_B};
  angle_now[AXIS_P] = angle_home[AXIS_P];
  step_now[AXIS_P]  = mapping(AXIS_P, STEP_PER_ANGLE) * angle_now[AXIS_P];
}


//==============================================================
//                       READ_ENDSTOP
//==============================================================
bool read_endstop(int AXIS_P) {
  int  AXIS_ENSTOP_PIN = mapping(AXIS_P, ENDSTOP);
  bool INVERT_LOGIC   = mapping(AXIS_P, INVERT_ENDSTOP);
  bool logic = digitalRead(AXIS_ENSTOP_PIN);
  logic = INVERT_LOGIC ? !logic : logic;
  return logic;
}


//==============================================================
//                       GO_HOME_ALL
//==============================================================
void goto_home_all() {
  goto_home(B_AXIS);
  goto_angle(B_AXIS, 0);
  goto_home(A_AXIS);
  goto_angle(A_AXIS, 0);
}


//==============================================================
//                         GOTO_STEP
//==============================================================
void goto_step(int AXIS_P) {
  if (step_now[AXIS_P] < step_setpoint[AXIS_P]) {
    set_dir(AXIS_P, mapping(AXIS_P, INVERT_DIR));
    while (step_now[AXIS_P] < step_setpoint[AXIS_P]) {
      move_axis_step(AXIS_P);
      step_now[AXIS_P]++;
    }
  } else {
    set_dir(AXIS_P, !mapping(AXIS_P, INVERT_DIR));
    while (step_now[AXIS_P] > step_setpoint[AXIS_P]) {
      move_axis_step(AXIS_P);
      step_now[AXIS_P]--;
    }
  }
}


//==============================================================
//                         GOTO_ANGLE
//==============================================================
void goto_angle(int AXIS_P, float angle_) {
  step_setpoint[AXIS_P] = mapping(AXIS_P, STEP_PER_ANGLE) * angle_;
  goto_step(AXIS_P);
  angle_now[AXIS_P] = angle_;
}


//==============================================================
//                           MOVE_STEP
//==============================================================
void move_axis_step(int AXIS_P) {
  digitalWrite(mapping(AXIS_P, STEP), HIGH);
  delayMicroseconds(SPEED_DELAY);
  digitalWrite(mapping(AXIS_P, STEP), LOW);
  delayMicroseconds(SPEED_DELAY);
}



//==============================================================
//                           SET_DIR
//==============================================================
void set_dir(int AXIS_P, bool dir_) {
  digitalWrite(mapping(AXIS_P, DIR_PIN), dir_);
}



//==============================================================
//                     GET_OFFSET_TABLE
//==============================================================
int get_offset_table(int hours_now) {
  int pick_time = hours_now - START_HOURS;
  if (pick_time < 0) {
    return -1;
  } else if (pick_time > RANGE_HOURS) {
    return -1;
  }
  return pick_time;
}



//==============================================================
//                           GET_ANGLE
//==============================================================
float get_angle(int AXIS_P, int index, int n) {
#ifdef MACHINE_A
  int A = AXIS_P == A_AXIS ? 0 : 13;
  if (n < 182)return pgm_read_float_near(&angle_table_A[n - 1][A + index]);
  else return pgm_read_float_near(&angle_table_B[n - 181 - 1][A + index]);
#else
  return pgm_read_float_near(&angle_table_C[n - 1][index]);
#endif
}



//==============================================================
//                           PRINT
//==============================================================
void print(int st) {
  time = "";
  time += day < 10 ? ("0" + String(day)) : (String(day));
  time += month < 10 ? ("/0" + String(month)) : ("/" + String(month));
  time += year < 10 ? ("/0" + String(year)) : ("/" + String(year));
  time += " - ";
  time += hour < 10 ? ("0" + String(hour)) : (String(hour));
  time += min < 10 ? (":0" + String(min)) : (":" + String(min));
  time += sec < 10 ? (":0" + String(sec)) : (":" + String(sec));

  axis = "";
#ifdef MACHINE_A
  axis += "A = " + String(angle_now[A_AXIS]);
  axis += "   ";
#endif
  axis += "B = " + String(angle_now[B_AXIS]);
  if (state_p == RUN) {
    table = "";
#ifdef MACHINE_A
    table += " A_TABLE = " + String(get_angle(A_AXIS, st, get_day_of_year(day, month, year)));
    table += "   ";
#endif
    table += " B_TABLE = " + String(get_angle(B_AXIS, st, get_day_of_year(day, month, year)));
  } else {
    table = "";
#ifdef MACHINE_A
    table += " A_TABLE = PAUSE ";
    table += "   ";
#endif
    table += " B_TABLE = PAUSE ";
  }
  data = time + " | " + axis + " | " + table + " | " ;

  Serial.print(data);
}



//==============================================================
//                       PRINT DATA TABLE
//==============================================================
void print_data_table() {
  for (int i = 0; i < 366; i++) {
    Serial.print("/*");
    if (i + 1 < 10) {
      Serial.print("00");
      Serial.print(i + 1);
    }
    else if (i + 1 < 100) {
      Serial.print("0");
      Serial.print(i + 1);
    }
    else Serial.print(i + 1);
    Serial.print("*/");
    Serial.print("{");
#ifdef MACHINE_A
    for (int j = 0; j < 13; j++) {
      float cr = get_angle(A_AXIS, j, i);
      Serial.print(cr);
      Serial.print(",");
    }
    Serial.print("/**/");
#endif
    for (int j = 0; j < 12; j++) {
      float cr = get_angle(B_AXIS, j, i);
      Serial.print(cr);
      Serial.print(",");
    }
    float cr = get_angle(B_AXIS, 12, i);
    Serial.print(cr);

    Serial.println("},");
  }
  Serial.println("========================================");
}
//==============================================================
//                           SETUP
//==============================================================
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("========================================");
  pinMode(STEP_A_PIN, OUTPUT);
  pinMode(STEP_B_PIN, OUTPUT);
  pinMode(EN_A_PIN, OUTPUT);
  pinMode(EN_B_PIN, OUTPUT);
  pinMode(DIR_A_PIN, OUTPUT);
  pinMode(DIR_B_PIN, OUTPUT);
  pinMode(ENDSTOP_A_PIN, INPUT);
  pinMode(ENDSTOP_B_PIN, INPUT);
  digitalWrite(EN_A_PIN, LOW);
  digitalWrite(EN_B_PIN, LOW);

  rtc.begin();
  if (UPLOAD_TIME == ON) rtc.set(day, month, year, hour, min, sec);
  rtc.start();
  //=======================
#ifdef MACHINE_A
  goto_home_all();
#else
  goto_home(B_AXIS);
  goto_angle(B_AXIS, 0  ); delay(1000);
#endif
/// ========================
  Serial.println("Setup Done");
  //  goto_home(A_AXIS);
}


//==============================================================
//                           LOOP
//==============================================================
void loop() {
  rtc.get(&day, &month, &year, &hour, &min, &sec);
  if (get_offset_table(hour) != -1) {
    state_p == RUN;
  }
  else state_p = STOP;

  if (state_p == RUN) {
    is_home = false;
    if (temp < 12)temp++;
    else temp = 0;
    for (int i = 0; i < 2; i++) {
      angle_setpoint[i] = get_angle(i, get_offset_table(hour), get_day_of_year(day, month, year));// chay theo ngay
//            angle_setpoint[i] = get_angle(i, temp, get_day_of_year(day, month, year)); // chay test hour
      //      angle_setpoint[i] = get_angle(A_AXIS, 0 , 1);// AXIS - INDEX - N
      if (angle_setpoint[i] > mapping_float(i, MAX_ANGLE))angle_setpoint[i] = mapping_float(i, MAX_ANGLE);
      if (angle_setpoint[i] < mapping_float(i, MIN_ANGLE))angle_setpoint[i] = mapping_float(i, MIN_ANGLE);
      d_angle[i] = 0;
      if (angle_setpoint[i] != angle_now[i]) {
        d_angle[i] = (angle_setpoint[i] - angle_now[i]) / 200.0;
        angle_target[i] = angle_now[i];
      }

    }
    for (int i = 0; i < 200; i++) {
#ifdef MACHINE_A
      angle_target[A_AXIS] += d_angle[A_AXIS];
      goto_angle(A_AXIS, angle_target[A_AXIS]);
#endif
      angle_target[B_AXIS] += d_angle[B_AXIS];
      goto_angle(B_AXIS, angle_target[B_AXIS]);
      rtc.get(&day, &month, &year, &hour, &min, &sec);
      print(temp);// <----------------------------------------------------------------------------(sua temp <-> hour)
#ifdef MACHINE_A
      Serial.print("A-SetAngle = ");
      Serial.print(angle_setpoint[A_AXIS]);
      Serial.print(" | ");
#endif
      Serial.print("B-SetAngle = ");
      Serial.print(angle_setpoint[B_AXIS]);
      Serial.println(" | ");
    }

    goto_angle(A_AXIS, angle_setpoint[A_AXIS]);
    goto_angle(B_AXIS, angle_setpoint[B_AXIS]);

  }
  else if (is_home == false) {
    is_home = true;
    goto_home_all();
  }
  //      goto_angle(A_AXIS, 0  ); delay(1000);
  //      goto_angle(B_AXIS, -67); delay(1000);
  //      goto_angle(B_AXIS, 0  ); delay(1000);
  //      goto_angle(B_AXIS,  67); delay(1000);
  //Serial.print("STATE=");
  // Serial.println(state_p);
  //  Serial.println(!read_endstop(A_AXIS));
  //    Serial.println(get_angle(A_AXIS, 0, get_day_of_year(day, month, year)));
}
