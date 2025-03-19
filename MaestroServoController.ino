/* This example shows how to smoothly control a single servo on a
Maestro servo controller using the PololuMaestro library. It
assumes you have an RC hobby servo connected on channel 0 of your
Maestro, and that you have already used the Maestro Control
Center software to verify that the servo is powered correctly and
moves when you command it to from the Maestro Control Center
software.

Before using this example, you should go to the Serial Settings
tab in the Maestro Control Center and apply these settings:

* Serial mode: UART, fixed baud rate
* Baud rate: 9600
* CRC disabled

Be sure to click "Apply Settings" after making any changes.

This example also assumes you have connected your Arduino to your
Maestro appropriately. If you have not done so, please see
https://github.com/pololu/maestro-arduino for more details on how
to make the connection between your Arduino and your Maestro. */

#include <PololuMaestro.h>

// Channel configuration
const uint8_t ENABLED_CHANNELS[] = {0, 1, 2}; // Specify which channels to enable (0-23)
const uint8_t NUM_CHANNELS = sizeof(ENABLED_CHANNELS) / sizeof(ENABLED_CHANNELS[0]);

// Global servo settings
const uint16_t DEFAULT_SPEED = 0;
const uint16_t DEFAULT_ACCELERATION = 127; // Moderate acceleration
const uint16_t MID_POSITION = 6000;        // 1500us (90 degrees)

// Demo settings
const uint16_t DEMO_SPEED = 0;        // Full speed for demos
const uint16_t DEMO_ACCEL_SMOOTH = 5; // Faster, smoother acceleration
const uint16_t DEMO_ACCEL_QUICK = 0;  // No acceleration for quick movements
const int DEMO_DELAY = 300;           // Shorter delay between movements
const int PATTERN_DELAY = 1000;       // Shorter delay between patterns

// Servo angle limits
const int MIN_ANGLE = 70;
const int MAX_ANGLE = 110;

// Using Serial1 (hardware UART) for Maestro communication
MiniMaestro maestro(Serial1);

// Maps angle in degrees to quarter-microsecond units
// Input: 60-120 degrees
// Output: ~4000-8000 quarter-microseconds (1000-2000us)
uint16_t mapAngleToTarget(int angle)
{
    // Constrain input angle to valid range
    angle = constrain(angle, MIN_ANGLE, MAX_ANGLE);

    // Map 60-120 degrees to 4000-8000 quarter-microseconds
    return map(angle, MIN_ANGLE, MAX_ANGLE, 4000, 8000);
}

void setChannelParams(uint8_t channel, uint16_t speed, uint16_t accel)
{
    maestro.setSpeed(channel, speed);
    maestro.setAcceleration(channel, accel);
}

void moveServo(uint8_t channel, int angle)
{
    uint16_t target = mapAngleToTarget(angle);
    maestro.setTarget(channel, target);
}

void initializeChannel(uint8_t channel)
{
    Serial.print("Initializing channel ");
    Serial.print(channel);
    Serial.println("...");

    setChannelParams(channel, DEFAULT_SPEED, DEFAULT_ACCELERATION);
    moveServo(channel, 90); // Mid position
}

void resetAllServos()
{
    for (uint8_t i = 0; i < NUM_CHANNELS; i++)
    {
        initializeChannel(ENABLED_CHANNELS[i]);
    }
    delay(PATTERN_DELAY);
}

// Demo movement patterns
void randomPattern()
{
    Serial.println("Random movement pattern...");
    for (int i = 0; i < 3; i++)
    { // Reduced to 3 random positions
        for (uint8_t j = 0; j < NUM_CHANNELS; j++)
        {
            setChannelParams(ENABLED_CHANNELS[j], DEMO_SPEED, DEMO_ACCEL_QUICK);
            int randomAngle = random(MIN_ANGLE, MAX_ANGLE + 1);
            moveServo(ENABLED_CHANNELS[j], randomAngle);
        }
        delay(DEMO_DELAY);
    }
}

void synchronizedSweep()
{
    Serial.println("Synchronized sweep pattern...");
    // Quick sweep to max
    for (uint8_t i = 0; i < NUM_CHANNELS; i++)
    {
        setChannelParams(ENABLED_CHANNELS[i], DEMO_SPEED, DEMO_ACCEL_SMOOTH);
        moveServo(ENABLED_CHANNELS[i], MAX_ANGLE);
    }
    delay(DEMO_DELAY);

    // Quick sweep to min
    for (uint8_t i = 0; i < NUM_CHANNELS; i++)
    {
        moveServo(ENABLED_CHANNELS[i], MIN_ANGLE);
    }
    delay(DEMO_DELAY);

    // Back to center
    for (uint8_t i = 0; i < NUM_CHANNELS; i++)
    {
        moveServo(ENABLED_CHANNELS[i], 90);
    }
    delay(DEMO_DELAY);
}

void waveSweep()
{
    Serial.println("Wave sweep pattern...");
    int positions[3] = {MIN_ANGLE, 90, MAX_ANGLE};

    // Simple wave effect
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        setChannelParams(ENABLED_CHANNELS[i], DEMO_SPEED, DEMO_ACCEL_SMOOTH);
        moveServo(ENABLED_CHANNELS[i], positions[i % 3]);
        delay(DEMO_DELAY / 2);
    }
    delay(DEMO_DELAY);

    // Reset to center
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        moveServo(ENABLED_CHANNELS[i], 90);
    }
    delay(DEMO_DELAY);
}

void runDemo()
{
    Serial.println("\n=== Starting Demo Mode ===");
    Serial.println("Press any key (except Enter) to exit demo");

    bool exitDemo = false;
    while (!exitDemo)
    {
        // Check for valid exit character (ignore newlines/carriage returns)
        while (Serial.available())
        {
            char c = Serial.read();
            if (c != '\n' && c != '\r')
            {
                exitDemo = true;
                break;
            }
        }

        if (!exitDemo)
        {
            randomPattern();
        }
        if (!exitDemo)
        {
            synchronizedSweep();
        }
        if (!exitDemo)
        {
            waveSweep();
        }
        if (!exitDemo)
        {
            delay(PATTERN_DELAY);
        }
    }

    // Clear any remaining characters in input buffer
    while (Serial.available())
    {
        Serial.read();
    }

    Serial.println("\n=== Exiting Demo Mode ===");
    resetAllServos();
}

void setup()
{
    // Initialize debug serial port
    Serial.begin(9600);
    Serial.println("Maestro Servo Controller Starting...");
    randomSeed(analogRead(0)); // Initialize random seed

    // Initialize hardware serial for Maestro
    Serial1.begin(9600);
    Serial.println("Serial1 initialized at 9600 baud");

    // Initialize all enabled channels
    resetAllServos();

    Serial.println("Enter 'd' for demo mode or angle (70-110 degrees) to move all servos:");
}

void loop()
{
    if (Serial.available() > 0)
    {
        if (Serial.peek() == 'd')
        {
            Serial.read(); // Clear the 'd'
            runDemo();
        }
        else
        {
            int angle = Serial.parseInt();

            // Clear input buffer
            while (Serial.available())
            {
                Serial.read();
            }

            if (angle >= MIN_ANGLE && angle <= MAX_ANGLE)
            {
                Serial.print("Moving all servos to ");
                Serial.print(angle);
                Serial.println(" degrees");

                for (uint8_t i = 0; i < NUM_CHANNELS; i++)
                {
                    moveServo(ENABLED_CHANNELS[i], angle);
                }
            }
            else
            {
                Serial.print("Invalid angle. Please enter a value between ");
                Serial.print(MIN_ANGLE);
                Serial.print(" and ");
                Serial.print(MAX_ANGLE);
                Serial.println(" degrees.");
            }
        }

        Serial.println("\nEnter 'd' for demo mode or angle (70-110 degrees) to move all servos:");
    }
}
