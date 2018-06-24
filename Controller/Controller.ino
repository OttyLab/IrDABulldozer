
char CONTROL[][4] = {
  {0x00, 0x00, 0x05, 0xFA}, // Forward
  {0x00, 0x00, 0x00, 0xFF}, // Back
  {0x00, 0x00, 0x04, 0xFB}, // Stop
  {0x00, 0x00, 0x84, 0x7B}, // Dozer up
  {0x00, 0x00, 0x81, 0x7E}, // Dozer down
  {0x00, 0x00, 0x01, 0xFE}, // Dozer keep
};

#define SIG_LOW \
  do {\
    TCCR1 = 0x82;\
    digitalWrite(0, LOW);\
  } while (0)

#define SIG_HIGH \
  do {\
    TCCR1 = 0x92;\
    digitalWrite(0, HIGH);\
  } while (0)

#define LEAD_CODE \
  do {\
    SIG_HIGH;\
    delayMicroseconds(9000);\
    SIG_LOW;\
    delayMicroseconds(4500);\
  } while (0)

#define STOP_BIT \
  do {\
    SIG_HIGH;\
    delayMicroseconds(560);\
    SIG_LOW;\
  } while (0)

#define ZERO \
  do {\
    SIG_HIGH;\
    delayMicroseconds(560);\
    SIG_LOW;\
    delayMicroseconds(565);\
  } while (0)
  
#define ONE \
  do {\
    SIG_HIGH;\
    delayMicroseconds(560);\
    SIG_LOW;\
    delayMicroseconds(1690);\
  } while (0)

void send(int type) {
  LEAD_CODE;

  char* sig = CONTROL[type];
  for (int i = 0; i < 4; i++) {
    for (int j = 7; j >= 0; j--) {
      if ((sig[i] >> j) & 0x01) {
        ONE;
      } else {
        ZERO;
      }
    }
  }

  STOP_BIT;
}

void setup() {
  // put your setup code here, to run once:
  // INPUT
  pinMode(0, INPUT); // Forward
  pinMode(2, INPUT); // Back
  pinMode(3, INPUT); // Up
  pinMode(4, INPUT); // Down

  // IrDA
  pinMode(1, OUTPUT);
  TCCR1 = 0x92;
  OCR1C = 53;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(0)) {
    send(0);
  } else if (digitalRead(2)) {
    send(1);
  } else {
    send(2); 
  }
  
  delay(120);

  if (digitalRead(3)) {
    send(3);
  } else if (digitalRead(4)) {
    send(4);
  } else {
    send(5); 
  }
  delay(120);
}
