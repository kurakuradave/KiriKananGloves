#include <Wire.h>
#include <LSM303.h>

LSM303 compass;

//int irX = 0;
//int irY = 0;m
int acX = 0;
int acY = 0;
//boolean irXMove = false;
//boolean irYMove = false;
// left hand flag for move detection
boolean acXLMove = false;
boolean acYLMove = false;
// right hand
boolean acXRMove = false;
boolean acYRMove = false;
// flag for activation
boolean tracking = false;

// os switching
String os = "knoppix";
int osKeyDelay = 100;

int deltaX = 15;
int deltaY = 10;
int deltaWheel = 1;

// left hand
int acXLOffset = -4;
int acYLOffset = -3;
// right hand
int acXROffset = 350;
int acYROffset = 320;

long lastZoom;

const int butpin = 11;
int butval = LOW;
int pbutval = LOW;

// right hand ADXL335 analog
int adxlX = 0;
int adxlY = 0;
int adxlZ = 0;


 
void setup()
{  
  pinMode( butpin, INPUT );
    //slaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI
    Serial.begin(19200);
    while( !Serial );
    Wire.begin();
    // Compass initialization
    compass.init();
    compass.enableDefault();
    compass.m_min = (LSM303::vector<int16_t>){  -543,   -465,   -384};
    compass.m_max = (LSM303::vector<int16_t>){  +313,   +418,   +312};
  
}




void loop()
{ 
    if( Serial.available() > 0 ){
        int cd = Serial.parseInt();
        if( cd == 111 ){
            calibrate( "left" );
        } else if( cd == 222 ) {
            calibrate( "right" );
        } else if( cd == 0 ) {
            if( os == "knoppix" ) switchOS( "ubuntu" );
            else if( os == "ubuntu" ) switchOS( "knoppix" );
        }
    }
    // button
    butval = digitalRead( butpin );
    if( butval == HIGH && pbutval == LOW ) {
        tracking = !tracking;
        if( tracking ) { 
            Mouse.begin(); 
            Keyboard.begin();
          Serial.println( "Tracking..." );
        } else { 
          Mouse.end(); 
          Keyboard.end();
          Serial.println( "NOT Tracking..." );
        }
    }
    pbutval = butval;
    
    compass.read();
    //float heading = compass.heading();
    // left hand
    acX = floor( compass.a.x / -1500 );
    if( acX - acXLOffset != 0 )
        acXLMove = true;
    else
        acXLMove = false;
    
    acY = floor( compass.a.y / -800 );
    if( acY - acYLOffset != 0 )
        acYLMove = true;
    else
        acYLMove = false;
    
    // right hand        
    adxl335Read();
    if( floor( abs( adxlX - acXROffset ) / 20 ) > 1 ) 
        acXRMove = true;
    else
        acXRMove = false;
    if( floor( abs( adxlY - acYROffset ) / 20 ) > 1 )
        acYRMove = true;
    else
        acYRMove = false;

    // move mouse    
      if( tracking && ( acXLMove || acYLMove ) ) {
        int mouseX = 0;
        int mouseY = 0;
        int mouseWheel = 0;
        // right hand AND left hand
        if( acXRMove ){
            if( ( floor( ( adxlX - acXROffset ) / 45 ) < 0 ) && ( acX - acXLOffset >= 6 ) ) {
                if( millis() - lastZoom > osKeyDelay ){
                  Serial.println( "zoom IN" );
                  // CTRL-ALT-PLUS:
                  if( os == "ubuntu" ){
                      Keyboard.press(KEY_LEFT_CTRL);
                      Keyboard.press(KEY_LEFT_ALT);
                      Keyboard.print( '=' );
                  } else if( os == "knoppix" ) {
                        Keyboard.press( KEY_LEFT_GUI );                
                        Keyboard.press( KEY_UP_ARROW );
                  }
                  delay( 100 );
                  Keyboard.releaseAll(); 
                  lastZoom =   millis();  
                }
            } else if(floor( ( adxlX - acXROffset ) / 45 ) > 0 && ( acX - acXLOffset <= -6 ) ){
                if( millis() - lastZoom > osKeyDelay ) {
                   Serial.println( "zoom OUT" );
                   // CTRL-ALT-MINUS:
                   if( os == "ubuntu" ) {
                       Keyboard.press(KEY_LEFT_CTRL);
                        Keyboard.press(KEY_LEFT_ALT);
                        Keyboard.print( '-' );
                   } else if( os == "knoppix" ) {
                         Keyboard.press( KEY_LEFT_GUI );
                        Keyboard.press( KEY_DOWN_ARROW );
                   }
                    delay( 100 );
                    Keyboard.releaseAll(); 
                    lastZoom = millis();
                }
            }
        } else if( acYRMove ) {
            if( floor( ( adxlY - acYROffset ) / 40 ) < 0 && acY - acYLOffset <= -12 ) {
                mouseWheel = -1;
            } else if( floor( ( adxlY - acYROffset ) / 40 ) > 0 && acY - acYLOffset >= 12 ) {
                mouseWheel = 1;
            }
            Mouse.move( mouseX, mouseY, mouseWheel );
            delay( 500 );
            /* debug output
            Serial.print( "amouse >>>" );
            Serial.print( mouseX );
            Serial.print( "," );
            Serial.print( mouseY );
            Serial.print( "," );
            Serial.println( mouseWheel );
            */   
        } else {
            // left hand only
            if( acX - acXLOffset <= -6 )
                mouseX = -1 * deltaX;
            else if( acX - acXLOffset >= 6 )
                mouseX = deltaX;
            if( acY - acYLOffset <= -12 )
                mouseY = -1 * deltaY;
            else if( acY - acYLOffset >= 12 )
                mouseY = deltaY;
            
            /* debug output
            Serial.print( "mouse >>>" );
            Serial.print( mouseX );
            Serial.print( "," );
            Serial.print( mouseY );
            Serial.print( "," );
            Serial.println( mouseWheel );
             */   
            Mouse.move( mouseX, mouseY, mouseWheel );
        }
    }
    
    /*
    Serial.print( "compass: " );
    Serial.print( compass.a.x );
    Serial.print( " " );
    Serial.print( compass.a.y );
    Serial.print( " " );
    Serial.print( compass.a.z );
    Serial.print( "   adxl: " );
    Serial.print( adxlX );
    Serial.print( " " );
    Serial.print( adxlY );
    Serial.print( " " );
    Serial.println( adxlZ );
   */                                            
    delay(50);
}


void calibrate( String tgt ) {
    int startclock = millis();
    int stopclock = startclock + 10000;
    while( startclock < stopclock ){
        if( tgt == "left" ) {
            acXLOffset = floor( compass.a.x / -1500 );
            acYLOffset = floor( compass.a.y / -800 );
        } else if( tgt == "right" ) {
            acXROffset = analogRead( A0 );
            acYROffset = analogRead( A1 );
        }
        Serial.println( "Attempting to calibrate, dont move hand..." );
        delay( 1000 );
        startclock = millis();
    }
           
    /*
    Serial.print( "acXLOffset, acYLOffset: " );
    Serial.print( acXLOffset );
    Serial.print( " " );
    Serial.println( acYLOffset );
    
    Serial.print( "acXROffset, acYROffset: " );
    Serial.print( acXROffset );
    Serial.print( " " );
    Serial.println( acYROffset );
    */
}

void adxl335Read() {
  adxlX = analogRead( A0 );
  adxlY = analogRead( A1 );
  adxlZ = analogRead( A2 );
}

void switchOS( String tgt ) {
    if( tgt == "knoppix" ) {
        os = "knoppix";
        osKeyDelay == 100;
        Serial.println( "OS: KNOPPIX" );
    } else if( tgt == "ubuntu" ) {
        os = "ubuntu";
        osKeyDelay == 1000;
        Serial.println( "OS: UBUNTU" );    
    }
}
