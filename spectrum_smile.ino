#define ADDR_1 2
#define ADDR_2 3
#define ADDR_3 4
#define ADDR_4 5
#define ADDR_5 6
#define ADDR_6 7
#define ADDR_7 8
#define ADDR_8 9
#define ADDR_9 10
#define ADDR_10 11
#define ADDR_11 12
#define ADDR_12 13
#define ADDR_13 22
#define ADDR_14 24
#define ADDR_15 26
#define ADDR_16 28

#define DATA_1 30
#define DATA_2 32
#define DATA_3 34
#define DATA_4 36
#define DATA_5 38
#define DATA_6 40
#define DATA_7 42
#define DATA_8 44

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
  res += digitalRead(DATA_1) ? 1 : 0;
  res += digitalRead(DATA_2) ? 2 : 0;
  res += digitalRead(DATA_3) ? 4 : 0;
  res += digitalRead(DATA_4) ? 8 : 0;
  res += digitalRead(DATA_5) ? 16 : 0;
  res += digitalRead(DATA_6) ? 32 : 0;
  res += digitalRead(DATA_7) ? 64 : 0;
  res += digitalRead(DATA_8) ? 128 : 0;

  return res;
}

void writeData(char value)
{
  digitalWrite(DATA_1, value & 1);
  digitalWrite(DATA_2, value & 2);
  digitalWrite(DATA_3, value & 4);
  digitalWrite(DATA_4, value & 8);
  digitalWrite(DATA_5, value & 16);
  digitalWrite(DATA_6, value & 32);
  digitalWrite(DATA_7, value & 64);
  digitalWrite(DATA_8, value & 128);
}

void setAddress(short value)
{
  digitalWrite(ADDR_1, value & 1);
  digitalWrite(ADDR_2, value & 2);
  digitalWrite(ADDR_3, value & 4);
  digitalWrite(ADDR_4, value & 8);
  digitalWrite(ADDR_5, value & 16);
  digitalWrite(ADDR_6, value & 32);
  digitalWrite(ADDR_7, value & 64);
  digitalWrite(ADDR_8, value & 128);
  digitalWrite(ADDR_9, value & 256);
  digitalWrite(ADDR_10, value & 512);
  digitalWrite(ADDR_11, value & 1024);
  digitalWrite(ADDR_12, value & 2048);
  digitalWrite(ADDR_13, value & 4096);
  digitalWrite(ADDR_14, value & 8192);
  digitalWrite(ADDR_15, value & 16384);
  digitalWrite(ADDR_16, value & 32768);
}

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
      digitalWrite(MEMREQ, LOW);
      digitalWrite(RD, LOW);
      modestage = 1;
      break;
    case 1:
      break;
    case 2:
      digitalWrite(WAIT, LOW);
      modestage = 3;
      break;
    case 3:
      break;
    case 4:
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
      digitalWrite(WAIT, HIGH);
      modestage = 2;
      break;
    case 2:
      break;
    case 3:
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

void doRead(short address)
{
  pinMode(DATA_1, INPUT);
  pinMode(DATA_2, INPUT);
  pinMode(DATA_3, INPUT);
  pinMode(DATA_4, INPUT);
  pinMode(DATA_5, INPUT);
  pinMode(DATA_6, INPUT);
  pinMode(DATA_7, INPUT);
  pinMode(DATA_8, INPUT);
  
  currentmode = READ_MODE;
  modestage = 0;

  setAddress(address);

  runClock();
}

void endRead()
{
  clockrun = false;
}

void doWrite(short address, char data)
{
}

void setup() {
  pinMode(MEMREQ, OUTPUT);
  pinMode(IORQ, OUTPUT);
  pinMode(RD, OUTPUT);
  pinMode(WR, OUTPUT);
  pinMode(WAIT, OUTPUT);

  digitalWrite(MEMREQ, HIGH);
  digitalWrite(IORQ, HIGH);
  digitalWrite(RD, HIGH);
  digitalWrite(WR, HIGH);
  digitalWrite(WAIT, LOW);
}

void loop() {
  delay(2000);
  doRead(0x8000); //32768

  if(currentdata == 0x12) //18
  {
    Serial.println("Correctly Read");
  }
}
