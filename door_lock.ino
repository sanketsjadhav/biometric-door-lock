#include <SoftwareSerial.h>
SoftwareSerial fingerPrint(A5, A4);

#include<Servo.h>
Servo myServo;

#include <Adafruit_Fingerprint.h>
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);

#define enroll 14
#define del 17
#define up 15
#define down 16
#define servoPin 9

void setup() 
{
    delay(1000);
    myServo.attach(servoPin);
    myServo.write(180);
    pinMode(enroll, INPUT_PULLUP);
    pinMode(up, INPUT_PULLUP); 
    pinMode(down, INPUT_PULLUP); 
    pinMode(del, INPUT_PULLUP); 
     finger.begin(57600);
    Serial.begin(9600);
  
    if (finger.verifyPassword()) 
    {
      Serial.println("Found fingerprint sensor!");
      delay(1000);
    } 
    else 
    {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
    }
}

void loop() 
{

 if(digitalRead(up)==0 || digitalRead(down)==0)
 {
  for(int i=0;i<5;i++)
  {
    
    int result=getFingerprintIDez();
    if(result>=0)
    {      
        myServo.write(100);
        delay(5000);
        myServo.write(180);
        return;
     }
   }
 }
 checkKeys();
 delay(1000);
}

void checkKeys()
{
   if(digitalRead(enroll) == 0)
   {
    while(digitalRead(enroll) == 0);
    Serial.println("ready to enroll");
    Enroll();
   }

   else if(digitalRead(del) == 0)
   {
    Serial.println("ready to delete");
    delet();
   }  
}

void Enroll()
{
   int count=0;
 
   while(1)
   {

     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }

     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
     else if(digitalRead(del) == 0)
     {
          id=count;
          getFingerprintEnroll();
          return;
     }

       else if(digitalRead(enroll) == 0)
     {        
          return;
     }
 }
}

void delet()
{
   int count=0;
  
   while(1)
   {
    
     if(digitalRead(up) == 0)
     {
       count++;
       if(count>25)
       count=0;
       delay(500);
     }

     else if(digitalRead(down) == 0)
     {
       count--;
       if(count<0)
       count=25;
       delay(500);
     }
     else if(digitalRead(del) == 0)
     {
          id=count;
          deleteFingerprint(id);
          return;
     }

       else if(digitalRead(enroll) == 0)
     {        
          return;
     }
 }
}

uint8_t getFingerprintEnroll() 
{
  int p = -1;
 
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
     
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No Finger");
     
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
     
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
     
      break;
    default:
      Serial.println("Unknown error");
      
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
     
      return p;
    default:
      Serial.println("Unknown error");
    
      return p;
  }
  
  Serial.println("Remove finger");
 
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
    
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      return;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  }
  else {
    Serial.println("Unknown error");
    return p;
  }   
}

int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  
  if (p != FINGERPRINT_OK)  
  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  
  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
   
   delay(2000);  
  return -1;
  }
  // found a match!
  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID); 
  return finger.fingerID; 
}

uint8_t deleteFingerprint(uint8_t id) 
{
  uint8_t p = -1;  
  
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) 
  {
    Serial.println("Deleted!");
   
    delay(1000);
  } 
  
  else 
  {
    Serial.print("Something Wrong"); 
    delay(2000);
    return p;
  }   
}
