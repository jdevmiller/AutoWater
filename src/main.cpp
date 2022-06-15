#include <Arduino.h>

const int AirValue = 620;
const int WaterValue = 310;
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

int READ_PIN = A0;
int MOTOR_PIN = 2;
bool WATERING = false;
int WATERING_CUTOFF = 95;
int SUCCULENT_DRYNESS_THRESHOLD = 40;
int delayBetweenSeconds = 10;
int wateringTimeSeconds = 7;
int probeChecksWithoutReporting = 0;

void water(){
    Serial.print("Watering");
    digitalWrite(MOTOR_PIN, LOW);
    for (int i = wateringTimeSeconds; i > 0; i--){
        Serial.print("..." + (String) i );
        delay(1000);
    }
    digitalWrite(MOTOR_PIN, HIGH);
    Serial.println("...0");
}

void readProbe(){
    soilMoistureValue = analogRead(READ_PIN);
    soilMoisturePercent = (int) map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    probeChecksWithoutReporting++;
}

void reportProbeValue(){
    Serial.print("Moisture level = ");
    if (soilMoisturePercent >= 100)
    {
        Serial.print("100 %");
    }
    else if(soilMoisturePercent <= 0)
    {
        Serial.print("0 %");
    }
    else
    {
        Serial.print((String) soilMoisturePercent + " %");
    }

    Serial.println(" (" + (String) soilMoistureValue + ")");
}

bool moistureValueChanged(int oldMoistureValue) {
    int tolerance = 2;
    int maxVal = oldMoistureValue + tolerance;
    int minVal = oldMoistureValue - tolerance;
    if (soilMoistureValue > minVal && soilMoistureValue < maxVal){
        return false;
    }
    return true;
}


void readDelay(int delaySeconds){
    for (int i = 0; i < 3; i++){
        int existingMoisture = soilMoistureValue;
        readProbe();
        if (moistureValueChanged(existingMoisture) || (probeChecksWithoutReporting >= 10) || WATERING){
            reportProbeValue();
            probeChecksWithoutReporting = 0;
        }

        delay(delaySeconds * 1000 / 3);
    }
}


void setup() {
    Serial.begin(9600);
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(MOTOR_PIN, HIGH);
}


void loop() {

    readDelay(delayBetweenSeconds);

    if ((soilMoisturePercent < SUCCULENT_DRYNESS_THRESHOLD) && !WATERING){
        WATERING = true;
        Serial.println("Watering initiated...");
    }

    if (WATERING){
        if (soilMoisturePercent > WATERING_CUTOFF)
        {
            WATERING = false;
            Serial.println("Watering terminated.");
        }
        water();
    }
}
