// Define pin connections & motor's steps per revolution
const int dirPin = 5;
const int stepPin = 4;
const int dirPin2 = 3;
const int stepPin2 = 2;
const int stepsPerRevolution = 1000;
unsigned long delayNum = 2500;

void setup()
{
    // Declare pins as Outputs
    pinMode(stepPin, OUTPUT);
    pinMode(stepPin2, OUTPUT);

    pinMode(dirPin, OUTPUT);
    pinMode(dirPin2, OUTPUT);

}
void loop()
{
    // Set motor direction clockwise
    digitalWrite(dirPin, HIGH);
    digitalWrite(dirPin2, HIGH);


    // Spin motor slowly
    for(int x = 0; x < stepsPerRevolution; x++)
    {
        digitalWrite(stepPin, HIGH);
        digitalWrite(stepPin2, HIGH);

        delayMicroseconds(delayNum);
        digitalWrite(stepPin, LOW);
        digitalWrite(stepPin2, LOW);

        delayMicroseconds(delayNum);
    }
    delay(1000); // Wait a second

    // Set motor direction counterclockwise
    digitalWrite(dirPin, LOW);

    // Spin motor also slowly
    for(int x = 0; x < stepsPerRevolution; x++)
    {
        digitalWrite(stepPin, HIGH);
        digitalWrite(stepPin2, HIGH);

        delayMicroseconds(delayNum);
        digitalWrite(stepPin, LOW);
        digitalWrite(stepPin2, LOW);

        delayMicroseconds(delayNum);
    }
    delay(1000); // Wait a second
}