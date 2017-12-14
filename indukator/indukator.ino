


// резисторы делителя напряжения
const float r1 = 99700;  // 100K
const float r2 =  9870;  // 10K

// эту константу (typVbg) необходимо откалибровать индивидуально
const float typVbg = 1.11; // 1.0 -- 1.2

float Vcc = 0.0;
float MaxVoltage = 0.0;

#define A_PIN 1
#define COUNT 5


int i;
float curVoltage;
char *stime;
int Led1 = 8;
int Led2 = 12;
int Led3 = 13;

/****************************************************************************
 * Главная программа
 ****************************************************************************/

void setup() {
  Serial.begin(9600);
  // определение опорного напряжения
  analogReference(DEFAULT);  // DEFAULT INTERNAL использовать Vcc как AREF
  delay(100);
  Vcc = readVcc();
  MaxVoltage = Vcc / (r2 / (r1 + r2));
  analogWrite(A_PIN, 0);
  pinMode(Led1 , OUTPUT);
  pinMode(Led2 , OUTPUT);
  pinMode(Led3 , OUTPUT);
  digitalWrite(Led1,HIGH);
  delay(500);
  digitalWrite(Led2,HIGH);
  delay(500);
  digitalWrite(Led3,HIGH);
  delay(500);
  digitalWrite(Led1,LOW);
  digitalWrite(Led2,LOW);
  digitalWrite(Led3,LOW);

}

void loop() {
  Vcc = readVcc();
  stime = TimeToString(millis()/1000);
  // считываем точное напряжение с A0, где будет находиться наш вольтметр с делителем напряжения
  curVoltage = 0.0;
  for (i = 0; i < COUNT; i++) {
      curVoltage = curVoltage + analogRead(A_PIN);
      delay(10);
  }
  curVoltage = curVoltage / COUNT;
  float v  = (curVoltage * Vcc) / 1024.0;
  float v2 = v / (r2 / (r1 + r2));
      if(v2 > 3.5 && v <= 5)
  {
    digitalWrite(Led1 , HIGH);
  }else
    {
    digitalWrite(Led1 , LOW);
    }
    if(v2 <= 3.5 && v2 > 3.1)
  {
    digitalWrite(Led2 , HIGH);
  }else
    {
    digitalWrite(Led2 , LOW);
    }
     if(v2 <= 3.1 && v2 >= 2.4)
  {
    digitalWrite(Led3 , HIGH);
  }else
    {
    digitalWrite(Led3 , LOW);
    }
     if(v2 <= 3.4)
  {
    digitalWrite(Led3 , HIGH);
    delay(200);
    digitalWrite(Led3 , LOW);
    delay(200);
  }
 
  Serial.println(v2);
    delay(1000);
  
  } 
 



/****************************************************************************
 * Функции
 ****************************************************************************/

float readVcc() {
  byte i;
  float result = 0.0;
  float tmp = 0.0;

  for (i = 0; i < 5; i++) {
    // Read 1.1V reference against AVcc
    // set the reference to Vcc and the measurement to the internal 1.1V reference
    #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ADMUX = _BV(MUX5) | _BV(MUX0);
    #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = _BV(MUX3) | _BV(MUX2);
    #else
        // works on an Arduino 168 or 328
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #endif

    delay(3); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Start conversion
    while (bit_is_set(ADCSRA,ADSC)); // measuring

    uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
    uint8_t high = ADCH; // unlocks both

    tmp = (high<<8) | low;
    tmp = (typVbg * 1024.0) / tmp;
    result = result + tmp;
    delay(5);
  }

  result = result / 5;
  return result;
}

// t is time in seconds = millis()/1000;
char * TimeToString(unsigned long t)
{
  static char str[12];
  long h = t / 3600;
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;
  sprintf(str, "%04ld:%02d:%02d", h, m, s);
  return str;
}
