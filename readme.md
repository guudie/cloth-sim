### Requirements
For windows users, you can skip this section (**Requirements**) and the next one (**Build and run**) and just unzip the `application.zip` file if you just want a working executable. Otherwise, if you prefer to build it yourself, here are the requirements:
* GNU make
* Python 3.x (to generate test data)
* MingW64 (on Windows, [msys2.org](https://www.msys2.org/))
* `g++` compiler and the following libraries installed
  * `SDL2` ([libsdl.org](https://www.libsdl.org/)), which provides rendering functionality

### Build and run
With GNU make installed, open cmd/terminal in the `cloth-sim/` folder and run the command `make`, the output executable with be named `app`.

Before running the file, generate data by running `py generate_tests.py` to get cloth information; this command will generate a `data.txt` text file.

Run `app`.

### Prebuilt executable (for windows)
If building the program yourself is not an option, you can unzip `application.zip`, which contains the executable itself (`app.exe`), and a pre-generated `data.txt` file.

### Interactions
Move the cursor close to a point (particle) in the cloth, click and hold left mouse button to pick up a particle near the position of the mouse, drag the mouse while holding to see effect.

To cut the cloth, simply hold right mouse button.

If you have additional side buttons (mouse 5), hold it and drag through the cloth to see another effect.