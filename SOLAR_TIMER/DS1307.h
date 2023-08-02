#ifndef DS1307_h
#define DS1307_h
class DS1307
{
  public:
    DS1307();
    void begin(void);
    void start(void);
    void stop(void);
    void get(uint8_t *day, uint8_t *month, uint16_t *year,uint8_t *hour,uint8_t *min,uint8_t *sec);
    void get(int *day, int *month, int *year,int *hour, int *min,int *sec );
    void set(uint8_t day, uint8_t month, uint16_t year,uint8_t hour,uint8_t min,uint8_t sec);
    void set(int day, int month, int year,int hour, int min,int sec );

  private:
    uint8_t bin2bcd(uint8_t val);
    uint8_t bcd2bin(uint8_t val);
};


#endif
