# 🦖 Arduino LCD Dino Game

A Google Dino-inspired endless runner built with an **Arduino Uno** and a **16x2 character LCD**.

The project started as a simple experiment in displaying a custom dinosaur character and gradually evolved into a playable game featuring jumping, animated movement, cactus obstacles, collision detection, scoring, randomized spawning, and increasing difficulty.

The project was intentionally developed in stages, with each stage introducing a new embedded-systems and programming concept.

---

## 🎮 Gameplay

The player controls a small dinosaur running across the bottom row of a 16x2 LCD.

Press the button to jump over incoming cacti.

Successfully avoiding a cactus increases the score. As the score increases, the cactus movement becomes faster and their spawn timing becomes less predictable.

A collision ends the game. Pressing the button after a game over resets the game.

### Features

* 🦖 Animated dinosaur running
* 🦘 Dinosaur jumping
* 🌵 Multiple cactus designs
* 🎲 Random cactus selection
* ⏱️ Non-blocking timing using `millis()`
* 🧱 C++ `Cactus` class
* ♻️ Reusable cactus object pool
* 💥 Collision detection
* 🏆 Score system
* 📈 Dynamic difficulty
* 🎲 Randomized cactus spawn intervals
* 🔄 Game-over and restart system
* 💾 Custom LCD characters

---

## 🛠️ Hardware

| Component          | Quantity |
| ------------------ | -------: |
| Arduino Uno        |        1 |
| 16x2 Character LCD |        1 |
| Push Button        |        1 |
| 10kΩ resistor*     |        1 |
| Jumper wires       |        — |
| Breadboard         |        1 |

*The button uses the Arduino's internal pull-up resistor, so an external resistor is not required with the current implementation.

---

## 🔌 LCD Connections

The LCD uses the Arduino's 4-bit interface.

| LCD Pin | Arduino |
| ------- | ------- |
| RS      | D8      |
| EN      | D9      |
| D4      | D4      |
| D5      | D5      |
| D6      | D6      |
| D7      | D7      |

The game button is connected to:

```text
Button → Arduino D12
```

The button uses:

```cpp
pinMode(jump_btn, INPUT_PULLUP);
```

Therefore the button is:

```text
HIGH → not pressed
LOW  → pressed
```

---

# 📁 Project Structure

```text
arduino-lcd-dino-game/
│
├── assets/
│   ├── dino.png
│   ├── cactuses.png
│   └── gameplay.png
│
├── dino_running_animation/
│   └── dino_running_animation.ino
│
├── dino_cactus_spawner/
│   └── dino_cactus_spawner.ino
│
├── dino_score_system/
│   └── dino_score_system.ino
│
└── README.md
```

Each folder represents an important development stage rather than simply containing different versions of the same code.

---

# 🧩 Development Stages

## 1. `dino_running_animation`

This was the foundation of the game.

The first goal was to create a dinosaur that could:

* Display on the LCD
* Animate while running
* Jump when the button is pressed
* Return to the ground after the jump

The dinosaur uses three custom LCD characters as running frames.

```cpp
byte dino[3][8]
```

The frames are cycled using a timer:

```cpp
if((current_tms - previous_tms) >= 60){

    dino_Rframe++;

    if(dino_Rframe > 2){
        dino_Rframe = 0;
    }

    lcd.createChar(0, dino[dino_Rframe]);

    lcd.setCursor(0, 1);
    lcd.write(byte(0));

    previous_tms = current_tms;
}
```

### Why `millis()`?

The animation does not use:

```cpp
delay();
```

Instead, it compares timestamps using `millis()`.

This allows the Arduino to continue handling other parts of the game while the animation runs.

The same timing principle is later used for:

* Cactus spawning
* Cactus movement
* Jump duration
* Game logic

---

# 🌵 2. `dino_cactus_spawner`

Once the dinosaur could run and jump, the next challenge was creating obstacles.

Three cactus designs were created as custom LCD characters:

```cpp
byte C1[8];
byte C2[8];
byte C3[8];
```

The game stores their patterns in an array of pointers:

```cpp
const byte* cactusPatterns[3] = {
    C1,
    C2,
    C3
};
```

This allows the game to randomly select a cactus design.

```cpp
int type = random(0, 3);
```

---

## C++ Cactus Class

Instead of managing every cactus using separate variables, the project introduced a C++ class:

```cpp
class Cactus
```

Each cactus object stores its own:

* X position
* Character pattern
* LCD character slot
* Active state

Conceptually:

```text
Cactus
├── position
├── pattern
├── character slot
└── active/inactive
```

### Object Pool

The game creates three reusable cactus objects:

```cpp
Cactus cactuses[3];
```

Rather than constantly creating and destroying objects, inactive objects are reused.

When a cactus needs to spawn:

```cpp
if(!cactuses[i].isActive()){
    cactuses[i].activate(...);
}
```

This creates a simple **object pool**.

It is a useful pattern for embedded systems because the number of objects is known ahead of time and memory usage remains predictable.

---

# 🎲 Randomized Spawning

The random number generator is seeded using:

```cpp
randomSeed(analogRead(A0));
```

This prevents the cactus sequence from following the same predictable pattern every time the Arduino resets.

The game also randomizes the delay between cactus spawns.

Instead of always using:

```text
1500 ms
1500 ms
1500 ms
1500 ms
```

the game selects a new interval from a range.

For example:

```text
1200–1800 ms
```

This produces gameplay that feels less predictable.

---

# 💥 Collision Detection

The initial collision system uses the LCD's horizontal position.

The dinosaur occupies:

```text
X = 0
```

A collision is detected when an active cactus reaches the same position:

```cpp
if(cactuses[i].getPosX() == 0 && !in_jump){
    gameOver = true;
}
```

The jump state is also considered.

Therefore:

```text
Cactus reaches X = 0
        │
        ├── Dinosaur jumping → continue
        │
        └── Dinosaur on ground → GAME OVER
```

This is a deliberately simple collision model suited to the limited resolution of a 16x2 LCD.

---

# 🏆 3. `dino_score_system`

This folder contains the **final version of the game**.

The scoring system is based on a simple event:

> A cactus successfully leaving the screen means the player successfully avoided it.

When a cactus moves past the left edge:

```cpp
if(cactuses[i].getPosX() < 0){

    score++;

    updateScore();

    updateDifficulty();

    cactuses[i].deactivate();
}
```

This gives us a clean game event:

```text
Cactus leaves screen
        ↓
     score++
        ↓
  update display
        ↓
 update difficulty
```

---

# 📈 Dynamic Difficulty

The game becomes progressively harder as the player survives.

Cactus movement is controlled by:

```cpp
cactus_moveInterval
```

Because this is a time interval:

```text
smaller interval = faster cactus
```

The difficulty increases at different score thresholds.

| Score | Cactus movement interval |
| ----: | -----------------------: |
|   0–4 |                   150 ms |
|   5–9 |                   130 ms |
| 10–14 |                   110 ms |
| 15–19 |                    90 ms |
|   20+ |                    75 ms |

This means the game does not simply become faster immediately. The player gets a chance to adapt before the next difficulty level.

---

# 🎲 Dynamic Spawn Timing

The final game also varies the time between cactus spawns.

The basic system uses a minimum and maximum:

```cpp
unsigned long cactus_spawnMin = 1200;
unsigned long cactus_spawnMax = 1800;
```

After each spawn, a new interval is selected:

```cpp
cactus_spawnInterval = random(
    cactus_spawnMin,
    cactus_spawnMax
);
```

This prevents the player from learning a perfectly predictable rhythm.

The game therefore has two independent difficulty factors:

```text
                 GAME DIFFICULTY
                       │
             ┌─────────┴─────────┐
             │                   │
             ▼                   ▼
       Cactus speed         Spawn timing
             │                   │
             ▼                   ▼
       Gets faster          Becomes random
```

---

# 🔄 Game State

The final version uses a simple game state:

```cpp
bool gameOver = false;
```

During normal gameplay:

```cpp
if(!gameOver){
    // game logic
}
```

When a collision occurs:

```cpp
gameOver = true;
```

The gameplay logic stops, effectively freezing the game.

The button remains available, allowing the player to restart.

---

# 🔁 Reset System

The `resetGame()` function resets the important parts of the game:

```text
Game state
     ↓
Cactus objects
     ↓
Timers
     ↓
Score
     ↓
LCD
```

The cactus pool is cleared:

```cpp
for(int i = 0; i < 3; i++){
    cactuses[i].deactivate();
}
```

The score is reset:

```cpp
score = 0;
```

And the timers are synchronized with the current `millis()` value.

This prevents old timer values from causing unexpected behaviour immediately after restarting.

---

# 🧠 Concepts Practiced

This project ended up covering significantly more than just Arduino LCD programming.

### Embedded Systems

* GPIO
* Digital input
* Internal pull-up resistors
* LCD interfacing
* Custom LCD characters
* Hardware timing

### C/C++

* Arrays
* Functions
* Pointers
* `const` pointers
* Classes
* Objects
* Constructors
* Private/public members
* Object pools
* State management

### Game Programming

* Game loops
* Player input
* Animation
* Collision detection
* Game states
* Scoring
* Difficulty scaling
* Randomized events

### Software Design

One of the main lessons from the project was separating responsibilities.

For example:

```text
dinoRun()
    ↓
Dinosaur animation

spawnCactus()
    ↓
Create obstacle

updateCactuses()
    ↓
Move obstacles

checkCollision()
    ↓
Detect failure

updateScore()
    ↓
Display score

updateDifficulty()
    ↓
Adjust gameplay
```

Instead of putting everything inside `loop()`, each part of the game has a specific responsibility.

---

# 🚀 Future Improvements

Possible improvements for future versions include:

* Better collision detection using multiple horizontal positions
* Different jump heights
* Cactus combinations
* High-score storage using EEPROM
* Increasing spawn difficulty with score
* More dinosaur animation frames
* Game-over screen
* Start screen
* Pause functionality
* Sound effects using a buzzer
* Difficulty modes
* Improved LCD graphics
* More efficient LCD updating
* PCB version of the game
* Custom enclosure

A future hardware revision could turn the project from a breadboard prototype into a standalone **Arduino LCD game console**.

---

# 📸 Project Assets

The `assets/` directory contains visual documentation of the project:

* Dinosaur and cactus graphics
* Gameplay screenshot
* Project visuals used for documentation

---

# 🎯 Project Goal

The purpose of this project was not simply to recreate the Google Dino game.

It was built as a practical exercise in gradually combining concepts:

```text
LCD
 ↓
Custom Characters
 ↓
Animation
 ↓
Input
 ↓
Jumping
 ↓
Objects
 ↓
Cactus Spawning
 ↓
Collision
 ↓
Game State
 ↓
Scoring
 ↓
Difficulty
 ↓
Randomized Gameplay
```

What began as a simple dinosaur displayed on a 16x2 LCD eventually became a small embedded game engine running on an **Arduino Uno**.

---

## 🦖 Final Result

A tiny 16x2 LCD.

An Arduino Uno.

A single button.

And somehow...

**a playable Dino game.** 🌵🔥

> Built as part of an ongoing exploration of Arduino, embedded systems, C++, electronics, and game programming.
