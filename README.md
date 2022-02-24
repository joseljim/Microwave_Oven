# Microwave_Oven
Microwave oven developed on the NXP KL25Z board using several peripherals. This project was designed to emulate all the behavior of a real microwave oven using interrupts, infrared sensors, PWM, DC motors, I2C, real time clock (RTC), LCD screen, ADC, temperature sensor (LM35) and LEDs.

## Team

* José Luis Jiménez Arévalo [joseljim](https://github.com/joseljim)
* Eduardo García Olmos [EDUARDOGO](https://github.com/EGO72)

## Schematic

<img width="382" alt="sch" src="https://user-images.githubusercontent.com/78834111/155444998-d313b294-cf7b-466b-9b50-af20cadf153a.png">

## Interruptions

* Cancel: the timers and systems involved in the heating are stopped. -> Push button
* Open door: the same happens as with the cancel button.              -> E18-D80NK

## Materials

| **Components** | **Quantity** |
|------------|:--------:|
|Push Butthon|2         |
|LCD 20x4    |1         |
|Matrix Keypad|1        |
|Active Buzzer|1        |
|DC Motor     |1        |
|H Bridge     |1        |
|LM35         |1        |
|E18-D80NK    |1        |
|DS1307       |1        |



## Video

Project demostration [Youtube video](https://youtu.be/nM7_EuxoMyk).
