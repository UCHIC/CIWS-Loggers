#include <stdio.h>
#include <stdlib.h>

unsigned int detectPeaks(float x[], unsigned int signal_size);

int main()
{
    const unsigned int MAG1 = 2597;
    const unsigned int MAG2 = 8381;
    float mag1[MAG1];
    float mag2[MAG2];
    float num;

    FILE *data_in;

    data_in = fopen("ConstRate_X.txt", "r");

    unsigned int i;
    unsigned int peaksDetected = 0;

    for(i = 0; i < MAG1; i++)
    {
        fscanf(data_in, "%f", &num);
        mag1[i] = num;
    }

    fclose(data_in);

    data_in = fopen("SweepRate_X.txt", "r");

    for(i = 0; i < MAG2; i++)
    {
        fscanf(data_in, "%f", &num);
        mag2[i] = num;
    }

    fclose(data_in);

    printf("Test Case 1: ConstRate_X.txt\n");
    peaksDetected = detectPeaks(mag1, MAG1);
    printf("\tPeaks Detected: %d\n", peaksDetected);

    printf("Test Case 2: SweepRate_X.txt\n");
    peaksDetected = detectPeaks(mag2, MAG2);
    printf("\tPeaks Detected: %d\n", peaksDetected);

    return 0;
}

unsigned int detectPeaks(float x[], unsigned int signal_size)
{
    float s = 0;                                                // s is the derivative of x.
    float sf[] = {0, 0};                                        // sf is the filtered derivative of x.
    const float a = 0.2;                                        // a is a constant parameter for low-pass filtering.
    unsigned int k;                                             // Loop Index
    float y[] = {0, 0};                                         // y is filtered x.
    char slopeWasPositive = 0;                                  // Used to track zero-crossings
    char slopeIsPositive = 0;                                   // Used to track zero-crossings
    unsigned int pulseCount = 0;                                // Pulses Counted

    for(k = 1; k < signal_size; k++)
    {
        y[1] = y[0] + (a * (x[k] - y[0]));          // Low Pass Filter: x to y
        s = y[1] - y[0];                            // Discrete derivative of y[]
        sf[1] = sf[0] + (a * (s - sf[0]));          // Filter the derivative s: s to sf
        sf[1] += 0.0005;                            // Offset Derivative to ignore noise
        y[0] = y[1];                                // Update y[]
        sf[0] = sf[1];                              // Update sf[]
        if (sf[1] > 0)                              // If the value of the derivative is greater than zero
            slopeIsPositive = 1;                    //      Then the slope of the signal is positive.
        else if (sf[1] < 0)                         // Else If the value of the derivative is less than zero
            slopeIsPositive = 0;                    //      Then the slope of the signal is negative.

        if (!slopeIsPositive && slopeWasPositive)   // If a positive to negative sign change occurs
            pulseCount += 1;                        //      Then increment the pulse count.

        if (slopeIsPositive)                        // If the slope is positive
            slopeWasPositive = 1;                   //      Then on the next run, we'll know the slope WAS positive
        else                                        // Otherwise
            slopeWasPositive = 0;                   //      We know the slope WAS NOT positive.

    }

    return pulseCount;
}
