# Zumo Search & Rescue Robot

The scenario behind this code is a ZUMO 32U4 robot that is performing search and rescue tasks through a building that is not safe for teams to enter due to low visibility unless given instruction to where someone is trapped. When the robot discovers a person it will emit a loud tone for the team to find.

# Development Environments Used

Processing:
This IDE was used to create the GUI for the sending of commands. This also used XBee pro modules that would allow connection through serial port input and out to read and interperet commands that are being sent to the Zumo robot. For the GUI the G4P library was used as it also allowed for a GUI builder to be used alongside which automatically generated a large part of the functionality code.

Arduino IDE:
This IDE was used to program the Zumo itself, included with this was the imported library for the Zumo 32U4 and the board manager that included the Pololu Zumo board format. This code is used to interpret the input that has been given to the robot and decided what the next action to perform is. It also includes the TurnSensor cpp and header files that are used in conjunction with the gyro sensor to allow it to function. This environment also allowed me to access other sensors aboard the Zumo (line & proximity) to move and control the robots actions with just simple single character inputs.

# Task approach

The first priority for my work was to be able to easily control the robots movements manually and make sure it would travel correctly and accurately based on given inputs. This also included a lot of back and forth between changing speeds and other variables to make sure that the robot could not just dart off across the map. After this basic control was completed the next issue to face was allowing the robot to make turns around a corner by itself rather than the user having to input the turn command multiple times. This was solved by using the onboard gyro of the Zumo and utilising the Turn sensor files included with multiple examples which gave instruction on how to use the gyro sensor and supporting functions that are needed.

The next issue came in the form of detection both of the lines and the proximity sensors. The first part to this was solved by viewing the data that is output in the line sensor test example to understand how the values are altered based on different surfaces beneath each sensor. Once I had that part figured I adapted the border detect example to work within value limits that I had observed during the previous example and created a small buffer for any random changes to the sensor without reaching a wall.
After this was completed came the room searching methods, this was achieved by adapting another given example to face another object and track it. This showed me the necessary method to input and use proximity data to give a certain result in this case it was a difference of tones depending on wether an object was detected in the room or not.


# Libraries and external used
1) G4P - http://www.lagers.org.uk/g4p/
2) Pololu boards for Arduino IDE - https://files.pololu.com/arduino/package_pololu_index.json
3) Turn Sensor - https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples/MazeSolver
