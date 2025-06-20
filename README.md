# Mechatronics

1. Cockroach: Assembling and debugging minibeacon PCBs. The cockroach is a 1ft X 1ft sized robot that has 2 DC motors, a photoresistor, LEDs, bumper sensors, and a microcontroller. When it is in the light, it moves forward and stops when it reaches in a dim place. During it's journey, if any of the bumpers gets triggered, the cockroach will go around the obstacle.

2. Motors: Drive DC motors, servos, and stepper motors, and create the software to control them. Drove a stepper motor directly using an H-bridge, and also using a dedicated driver board.

3. Automated Pinball: Started the project from scratch. Utilizing plywood, acrylic, and 3D printers to create our system. We used 1 DC motor, 1 Stepper motor, 2 Servos, and 4 Infrared sensors to control the game. The only human interaction is pulling the plunger to start the game, and everything else is automated. 

    How the game works: There are 4 alleys that the ball could go through. Since there's only one ball, it can only go through one alley at a time. The alley is randomly selected by the system and is indicated by an LED light. The motors will rotate and position pegs to attempt to funnel the ball into the alley. Once the ball goes through the indicated alley, the next randomized alley will be selected. On the right, there's a timing belt to transport the ball up the system to get the ball into the funnel. The game is completed after the ball goes into the indicated alley 3 times.

