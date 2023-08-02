#define MACHINE_B
//====== TIME =========
#define UPLOAD_TIME       OFF
#define DAY               28           
#define MONTH             07
#define YEAR              2023
#define HOUR              16
#define MINUTE            42
#define SECOND            00

//====== STEPPER =========
#define STEP_A_PIN        2
#define STEP_B_PIN        6

#define DIR_A_PIN         3
#define DIR_B_PIN         7

#define INVERT_DIR_A      true
#ifdef MACHINE_B
#define INVERT_DIR_B      true
#else
#define INVERT_DIR_B      false
#endif 

#ifdef MACHINE_B
#define INVERT_HOME_DIR_A true
#define INVERT_HOME_DIR_B true
#else
#define INVERT_HOME_DIR_A false
#define INVERT_HOME_DIR_B false
#endif 


#define INVERT_ENDSTOP_A  true

#ifdef MACHINE_B
#define INVERT_ENDSTOP_B  false
#else
#define INVERT_ENDSTOP_B  true
#endif 

#define EN_A_PIN          4
#define EN_B_PIN          5


//====== ENDSTOP  =========
#define ENDSTOP_A_PIN     9
#define ENDSTOP_B_PIN     8

#define ENDSTOP_A_TRIG    0
#define ENDSTOP_B_TRIG    0

#define HOME_DIR_A        0
#define HOME_DIR_B        0

//====== TIMER  =========
#define BREAK_TIME        50

//====== AXIS  =========
#define A_AXIS 0
#define B_AXIS 1

//====== TABLE  =========
#define DIR_PIN         0
#define ENDSTOP         1
#define STEP            2
#define INVERT_DIR      3
#define INVERT_ENDSTOP  4
#define INVERT_HOME_DIR 5
#define STEP_PER_ANGLE  6
#define ANGLE_HOME      7

#define MAX_ANGLE       0
#define MIN_ANGLE       1

//====== DIMENSION  =========
#define ANGLE_HOME_A    -180
#define ANGLE_HOME_B    65

#define MAX_ANGLE_A     180
#define MAX_ANGLE_B     55.00

#define MIN_ANGLE_A    -180
#define MIN_ANGLE_B    -55.00

#define STEP_PER_ANGLE_A  400
#define STEP_PER_ANGLE_B  500

#define SPEED_DELAY       100
#define START_HOURS         6
#define END_HOURS          18
#define RANGE_HOURS       END_HOURS - START_HOURS
#define RESET_HOURS       19
#define ON true
#define OFF false

//====== CONDITION  =========
#define RUN             1
#define STOP            0
