# ENGG1340 Project - Group 4

> [!note]
> **GRADE OF THIS PROJECT**
>
> This project has received a grade of 19.9 (out of 20).

## Group Members
<!-- Identification of the team members. -->
Removed for Privacy

## Introduction

### Overall Introduction

Our project, **CounterStrike**, is a strategic defense game where players must protect their homeland from nuclear strikes. The game combines elements of resource management, decision-making, and tactical gameplay to create an engaging experience. Players will navigate a map, assess threats, and deploy resources to defend against incoming attacks while working toward victory by depleting the enemy's strength.

The game is designed to challenge players' strategic thinking and adaptability, offering a unique experience with every play through due to its dynamic event generation and multiple difficulty levels. By leveraging a text-based user interface, the game ensures accessibility and simplicity while maintaining depth in gameplay mechanics.

### Gameplay

- **Game Objective**: Players must defend their cities and the civilians from nuclear strikes by managing resources, deploying defensive missiles, and repairing damaged states. The ultimate goal is to deplete the enemy's health while protecting your states.
- **Game Mechanics**: Players navigate a map of cities, assess attack statuses, and make strategic decisions based on various factors such as economy, attack timing, and enemy health. The game features a turn-based system where players can issue commands including buying defensive missiles, launching intercept missiles, researching technologies, and repairing cities.
- **Game Flow**: The game begins with an introduction and a menu where players can choose to start a new game, load a saved game, or exit. Players will then navigate through the map, make decisions, and respond to incoming attacks. The game continues until either the player or the enemy is defeated.
- **Tutorial**: The game includes a tutorial page of the controls and actions available to players, you are highly recommended to read it before playing the game.
- **Game Over**: The game ends when either the player or the enemy is defeated. Players can view their statistics and scores at the end of the game.

## Features
<!-- A list of features that you have implemented and explain how each coding element 1 to 5 listed  -->
<!-- under the coding requirements aforementioned support your features. -->

1. **Generation of Random Events**
   - **Implementation**: In the game, we use the `discrete_distribution` and `uniform_int_distribution` classes and the `mt19937` algorithm provided the `<random>` library to generate random events, such as attack missile initial position, speed and damage.
   - **Support**: This feature enhances gameplay by introducing unpredictability, ensuring that each game session is unique and challenging.

2. **Data Structures for Storing Data**
   - **Implementation**: The game utilizes various data structures, such as `vector`, `array` provided by standard template library (STL) and custom classes (e.g., `City`, `Game`, `Menu`), to store game state, cities, and menu options.
   - **Support**: These structures facilitate efficient management and manipulation of game data, allowing for easy access and modification during gameplay.

3. **Dynamic Memory Management**
   - **Implementation**: The game employs dynamic memory management techniques, such as pointers and `new`/`delete` operators, to handle the creation and destruction of game objects (e.g. `Missile`, `TechNode`). We also use `std::vector` for dynamic arrays, which is consistent with C++11 standards and the RAII (Resource Acquisition Is Initialization) principle.
   - **Support**: With the help of `std::vector`, we can dynamically allocate memory for game objects as needed and automatically deallocated when they go out of scope, reducing the risk of memory leaks. Apart from this, we also use `new` and `delete` to manually manage some memory allocations for specific game objects, such as `Missile` and `TechNode`, which reduces the need for copying large objects and improves performance. The combination of applying both techniques allows for efficient memory usage and management throughout the game while reduces the difficulty of memory management.

4. **File Input/Output**
   - **Implementation**: We apply file I/O operations using the `<fstream>` library to read and write game data from files (including `general.txt`, `cities.txt`, `background.txt`, `title.txt`), such as city information and map background and general settings. Moreover, we implement the saving/loading functionality using `fstream` to allow players to save their progress and load previous game states.
   - **Support**: This feature enhances user experience by allowing players to save their progress and load previous game states, making the game more user-friendly and accessible. Moreover, it separates the game logic from the data, allowing for easier updates and modifications to the game without changing the core code.

5. **Program Codes in Multiple Files**
   - **Implementation**: The project is organized into multiple files and folders. The `src` folder contains all the source code files and header files, including `main.cpp`, `game.cpp`, `menu.cpp`, `saver.cpp`, and `render.cpp`. While the `assets` folder contains all the assets used in the game, such as map background (`background.txt`), city information (`cities.txt`). After compiling, the binary files are generated in the `bin` folder, while an executable file and necessary asset files will be copied to `dist` file.
   - **Support**: This modular structure improves code readability and maintainability, allowing developers to work on different parts of the project concurrently and facilitating easier debugging. Moreover, it allows for a lower coupling between different components of the game, making it easier to update or replace individual parts without affecting the entire codebase.

6. **Multiple Difficulty Levels**
   - **Implementation**: The game includes options for varying difficulty levels, which can affect factors in the missile generation progress, making the game more challenging or easier based on player preference.
   - **Support**: By allowing players to choose their preferred difficulty level, the game enhances engagement and caters to a wider audience, promoting replayability.

<!-- A list of non-standard C/C++ libraries, if any, that are used in your work and integrated to your code repo. -->
<!-- Please also indicate what features in your game are supported by these libraries.  -->

## Dependencies

### Non-Standard Lib

**`<ncurses>`**

- **Purpose**: Provides functions for creating text-based user interfaces and provides a simpler window management in a terminal.
- **Features Supported**: It provides a user-friendly interface for handling user input, managing screen/window rendering, and displaying various text effects/colors, enhancing the overall user experience.

<!-- Compilation and execution instructions. This serves like a "Quick start" of your game. -->
<!-- The teaching team will follow your instructions to compile and run your game. -->

## Quick Start

1. First, ensure you have all the necessary dependencies installed (see [Dependencies](#dependencies)).
   For the `ncurses` library, it have been installed in the academy server.
2. Clone the repository to your local machine or download the source code.
3. Open a terminal and navigate to the project root directory (where the `makefile` is located).
4. Run the following command to compile the game:

   ```bash
   make all
   ```

   This will compile all the source files and create an executable file in the `dist` folder.
5. Navigate to the `dist` folder:

   ```bash
   cd dist
   ```

6. Make sure the terminal window is large enough to display the game properly. You may need to turn down the font size.

   This game requires at least at least 40 rows and 140 columns of terminal size. It is tested that on a smaller terminal, the game may not display correctly even pure black screen.

   If you are playing the game for the first time, you will be forced to read the tutorial page after pressing the `START THE GAME` button. You can return to start menu and actually start the game by pressing `START THE GAME` button again.

7. Run the game using the following command:

   ```bash
   ./main
   ```

8. Follow the on-screen instructions to start playing the game.
9. To exit the game, press `Ctrl + C`, `ESC` or following the in-game instructions.

## File Structure

```plaintext
project root
├── assets/
│    ├── video/
│    │   ├── frame0.txt
│    │   ├── frame1.txt
│    │   └── ...
│    ├── background.txt
│    ├── cities.txt
│    ├── general.txt
│    └── title.txt
├── bin/
│    ├── main
│    └── *.o
├── dist/
│    ├── main
│    ├── video/
│    ├── background.txt
│    ├── cities.txt
│    ├── general.txt
│    └── title.txt
├── src/
│    ├── game.cpp
│    ├── game.h
│    ├── menu.cpp
│    ├── menu.h
│    ├── saver.cpp
│    ├── saver.h
│    ├── render.cpp
│    ├── render.h
│    ├── main.cpp
│    └── utils.h
├── makefile
├── .gitignore
├── README.md
└── TODO.md
```

## Video Demonstration

- [Video Link](https://www.bilibili.com/video/BV1g4GdzxEVB)

<!-- Link of Video -->
