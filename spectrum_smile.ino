#define ADDR_0 2
#define ADDR_1 3
#define ADDR_2 4
#define ADDR_3 5
#define ADDR_4 6
#define ADDR_5 7
#define ADDR_6 8
#define ADDR_7 9
#define ADDR_8 10
#define ADDR_9 11
#define ADDR_10 12
#define ADDR_11 13
#define ADDR_12 22
#define ADDR_13 24
#define ADDR_14 26
#define ADDR_15 28

#define DATA_0 30
#define DATA_1 32
#define DATA_2 34
#define DATA_3 36
#define DATA_4 38
#define DATA_5 40
#define DATA_6 42
#define DATA_7 44

#define CLOCK 31
#define IORQ 33
#define MEMREQ 35
#define RD 37
#define WR 39
#define WAIT 41

bool clockrun = false;
bool clockvalue = false;
int lowcount = 0;
int highcount = 0;
int ignore = false;

int IDLE_MODE = 0;
int READ_MODE = 1;
int WRITE_MODE = 2;

int currentmode = IDLE_MODE;
int modestage = 0;

char currentdata = 0;

char readData()
{
  char res = 0;
  res += digitalRead(DATA_0) ? 1 : 0;
  res += digitalRead(DATA_1) ? 2 : 0;
  res += digitalRead(DATA_2) ? 4 : 0;
  res += digitalRead(DATA_3) ? 8 : 0;
  res += digitalRead(DATA_4) ? 16 : 0;
  res += digitalRead(DATA_5) ? 32 : 0;
  res += digitalRead(DATA_6) ? 64 : 0;
  res += digitalRead(DATA_7) ? 128 : 0;

  return res;
}

void writeData(char value)
{
  digitalWrite(DATA_0, value & 1);
  digitalWrite(DATA_1, (value & 2) >> 1);
  digitalWrite(DATA_2, (value & 4) >> 2);
  digitalWrite(DATA_3, (value & 8) >> 3);
  digitalWrite(DATA_4, (value & 16) >> 4);
  digitalWrite(DATA_5, (value & 32) >> 5);
  digitalWrite(DATA_6, (value & 64) >> 6);
  digitalWrite(DATA_7, (value & 128) >> 7);
}

void setAddress(short value)
{
  digitalWrite(ADDR_0, value & 1);
  digitalWrite(ADDR_1, (value & 2) >> 1);
  digitalWrite(ADDR_2, (value & 4) >> 2);
  digitalWrite(ADDR_3, (value & 8) >> 3);
  digitalWrite(ADDR_4, (value & 16) >> 4);
  digitalWrite(ADDR_5, (value & 32) >> 5);
  digitalWrite(ADDR_6, (value & 64) >> 6);
  digitalWrite(ADDR_7, (value & 128) >> 7);
  digitalWrite(ADDR_8, (value & 256) >> 8);
  digitalWrite(ADDR_9, (value & 512) >> 9);
  digitalWrite(ADDR_10, (value & 1024) >> 10);
  digitalWrite(ADDR_11, (value & 2048) >> 11);
  digitalWrite(ADDR_12, (value & 4096) >> 12);
  digitalWrite(ADDR_13, (value & 8192) >> 13);
  digitalWrite(ADDR_14, (value & 16384) >> 14);
  digitalWrite(ADDR_15, (value & 32768) >> 15);
}

// LOW FUNCTION (12)
// When this is called, it keeps track of t-states using high and low functions called which
// Set the modestage so we can catch when the clock goes high/low and what to do on that current t-state.
void low()
{
 if(currentmode == (int)IDLE_MODE || !clockrun)
 {
    return;
 }
 else if(currentmode == (int)READ_MODE)
 {
  switch(modestage)
  {
    case 0:
      // First Clock Cycle Low
      digitalWrite(MEMREQ, LOW);
      digitalWrite(RD, LOW);
      modestage = 1;
      break;
    case 1:
      break;
    case 2:
      // Second clock cycle system low
      digitalWrite(WAIT, LOW);
      modestage = 3;
      break;
    case 3:
      break;
    case 4:
      // last clock cycle low (after read)
      digitalWrite(MEMREQ, HIGH);
      digitalWrite(RD, HIGH);
      currentmode = IDLE_MODE;
      modestage = 0;
      endRead();
      break;
  }
 }
 else if(currentmode == (int)WRITE_MODE)
 {
  switch(modestage)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
  }
 }
}

void high()
{
 if(currentmode == (int)IDLE_MODE || !clockrun)
 {
    return;
 }
 else if(currentmode == (int)READ_MODE)
 {
  switch(modestage)
  {
    case 0:
      break;
    case 1:
      // First clock cycle high
      digitalWrite(WAIT, HIGH);
      modestage = 2;
      break;
    case 2:
      break;
    case 3:
      // Second cycle hight
      delay(0.2);
      currentdata = readData();
      modestage = 4;
      break;
  }
 }
 else if(currentmode == (int)WRITE_MODE)
 {
  switch(modestage)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
  }
 }
}

// RUN CLOCK FUNCTION (responds to High and Low levels of CLOCK to CALL low() and high() functions (11)
void runClock()
{
  clockrun = true;
  clockvalue = digitalRead(CLOCK);
  lowcount = 0;
  highcount = 0;
  
  if(clockvalue)
  {
    ignore = false;
  }
  else
  {
    ignore = true;
  }
  
  while(clockrun)
  {
    clockvalue = digitalRead(CLOCK);
    
    if(!clockvalue && ignore)
    {
      continue;
    }
  
    ignore = false;
    
    while(clockvalue)
    {  
      clockvalue = digitalRead(CLOCK);
    }
  
    // Clock is now low after being high
    // 1/2 Clock Cycle
  
    lowcount++;
    low();
  
    while(!clockvalue)
    {  
      clockvalue = digitalRead(CLOCK);
    }
  
    // Clock is now high after being low
    // 1/2 Clock Cycle
  
    highcount++;
    high();
  }
}

// THIS IS CALLED (6)
void doRead(short address)
{
  // SET DATA PINS AS INPUT (7)
  pinMode(DATA_0, INPUT);
  pinMode(DATA_1, INPUT);
  pinMode(DATA_2, INPUT);
  pinMode(DATA_3, INPUT);
  pinMode(DATA_4, INPUT);
  pinMode(DATA_5, INPUT);
  pinMode(DATA_6, INPUT);
  pinMode(DATA_7, INPUT);

  // SET CURRENT MODE TO READ_MODE (8)
  currentmode = READ_MODE;
  // SET STAGE TO 0 (9)
  modestage = 0;

  // CALL SET ADDRESS FUNCTION(9)
  setAddress(address);

  // START THE CLOCK LOOP (10)
  runClock();
}

void endRead()
{
  clockrun = false;
}

void doWrite(short address, char data)
{
}

//START HERE (1)
void setup() {
  //SET REQUEST PINS AS OUTPUT (2)
  pinMode(MEMREQ, OUTPUT);
  pinMode(IORQ, OUTPUT);
  pinMode(RD, OUTPUT);
  pinMode(WR, OUTPUT);
  pinMode(WAIT, OUTPUT);


  //SET REQUEST PINS TO HIGH (3)
  digitalWrite(MEMREQ, HIGH);
  digitalWrite(IORQ, HIGH);
  digitalWrite(RD, HIGH);
  digitalWrite(WR, HIGH);
  digitalWrite(WAIT, LOW);
}

void loop() {
  // AFTER SETUP, THIS FUNCTION KICKS IN (4)
  delay(2000);

  // CALL READ FUNCTION (5)
  doRead(0x8000); //32768

  if(currentdata == 0x12) //18
  {
    Serial.println("Correctly Read");
  }
}
