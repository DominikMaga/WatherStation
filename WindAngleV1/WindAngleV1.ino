#include <Wire.h>
#include <HMC5883L.h>
#include <ADXL345.h>
#include <MLX90316.h>
#include <Metro.h>

int pinSS = 5;
int pinSCK = 3;
int pinMOSI = 4;

HMC5883L compass;
ADXL345 accelerometer;
Metro mlxMetro = Metro(5);
MLX90316 mlx_1  = MLX90316();

float heading1;
float heading2;
float windDirection;
int32_t wd;
void setup() {
  Serial.begin(9600);


  mlx_1.attach(pinSS, pinSCK, pinMOSI );

  if (!accelerometer.begin())
  {
    delay(500);
  }
  accelerometer.setRange(ADXL345_RANGE_2G);


  // Initialize Initialize HMC5883L
  while (!compass.begin())
  {
    delay(500);
  }
  Wire.begin(100);
  Wire.onRequest(requestEvent);
  // Set measurement range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);

  // Wartości uzyskane w prcesie kalibracji
  compass.setOffset(34, -54);


}

void loop() {
  //getEncoderAngle();
  getWindAngle();
  //getNorthAngle();

  delay(100);
}
void getWindAngle() {
  float a = getEncoderAngle();
  float b = getNorthAngle();
  if (mlxMetro.check() == 1) {
    Serial.print(a);
    Serial.print(" : ");
    Serial.print(b);
    Serial.print(" : ");
    float temp = a - b;
    Serial.print(temp);
    Serial.print(" : ");
    if (temp < 0) {
      windDirection = (360 + temp);
      windDirection=windDirection*100;
      wd=(int32_t)windDirection;
      Serial.print("+ ");
      Serial.println(windDirection);
    } else {
      windDirection = temp;
      windDirection=windDirection*100;
      wd=(int32_t)windDirection;
      Serial.println(windDirection);     
    }
  }
}

float getEncoderAngle() {
  float angle;
  if (mlxMetro.check() == 1) {

    angle = mlx_1.readAngle();
    if (angle >= 0) {
      return 360 - (angle * 0.1);
    } else {
      return -1.00;
    }
  }
}

float getNorthAngle() {
  // Wczytanie wektorów
  Vector mag = compass.readNormalize();
  Vector acc = accelerometer.readScaled();
 
  heading1 = noTiltCompensate(mag);
  heading2 = tiltCompensate(mag, acc);

  if (heading2 == -1000)
  {
    heading2 = heading1;
  }
  // dekliancja kąta magnetycznego: http://magnetic-declination.com/     // (+) Positive or (-) for negative
  // Schemat: (deg + (min / 60.0)) / (180 / M_PI);
  float declinationAngle = (5.0 + (50.0 / 60.0)) / (180 / M_PI);
  heading1 += declinationAngle;
  heading2 += declinationAngle;

  heading1 = correctAngle(heading1);
  heading2 = correctAngle(heading2);

  heading1 = heading1 * 180 / M_PI;
  heading2 = heading2 * 180 / M_PI;

  return heading2;
}



// No tilt compensation
float noTiltCompensate(Vector mag)
{
  float heading = atan2(mag.YAxis, mag.XAxis);
  return heading;
}

// Tilt compensation
float tiltCompensate(Vector mag, Vector normAccel)
{
  // Pitch & Roll

  float roll;
  float pitch;

  roll = asin(normAccel.YAxis);
  pitch = asin(-normAccel.XAxis);

  if (roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78)
  {
    return -1000;
  }

  // Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
  float cosRoll = cos(roll);
  float sinRoll = sin(roll);
  float cosPitch = cos(pitch);
  float sinPitch = sin(pitch);

  // Tilt compensation
  float Xh = mag.XAxis * cosPitch + mag.ZAxis * sinPitch;
  float Yh = mag.XAxis * sinRoll * sinPitch + mag.YAxis * cosRoll - mag.ZAxis * sinRoll * cosPitch;

  float heading = atan2(Yh, Xh);

  return heading;
}

// Correct angle
float correctAngle(float heading)
{
  if (heading < 0) {
    heading += 2 * PI;
  }
  if (heading > 2 * PI) {
    heading -= 2 * PI;
  }

  return heading;
}
void requestEvent()
{
 
int32_t bigNum = wd;
byte myArray[4];
 
myArray[0] = (bigNum >> 24) & 0xFF;
myArray[1] = (bigNum >> 16) & 0xFF;
myArray[2] = (bigNum >> 8) & 0xFF;
myArray[3] = bigNum & 0xFF;
 
Wire.write(myArray, 4);
}

