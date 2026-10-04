#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 18;

DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup() {
  Serial.begin(9600);
}

static bool measure_env(float *temperature, float *humidity) {

  static unsigned long measurement_timestamp = millis();

  // measure once every ~3 seconds
  if (millis() - measurement_timestamp > 3000ul) 
  {
    if (dht_sensor.measure(temperature, humidity) == true)
    {
      measurement_timestamp = millis();
      return(true);
    }
  }
  return(false);
}

void loop() {

  float temperature;
  float humidity;

  if(measure_env(&temperature, &humidity) == true) 
  {
    Serial.print("T = ");
    Serial.print(temperature, 1);
    Serial.print(" °C     H = ");
    Serial.print(humidity, 1);
    Serial.println(" %");
  }
}
