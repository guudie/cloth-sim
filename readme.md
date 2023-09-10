### Requirements
* `GNU make`
* Python 3.x
* `g++` compiler with the following libraries installed
  * `glm` ([github](https://github.com/g-truc/glm)), which provides vector computation functionality
  * `SDL2` ([libsdl.org](https://www.libsdl.org/)), which provides rendering functionality

### Build and run
With `GNU make` installed, open cmd/terminal in the `cloth-sim/` folder and run the command `make`, the output executable with be named `app`.

Before running the file, generate data by running `py generate_tests.py` to get cloth information; this command will generate a `data.txt` text file.

Run `app`.

### Interactions
Move the cursor close to a point (particle) in the cloth, click and hold left mouse button to pick up a particle near the position of the mouse, drag the mouse while holding to see effect.

To cut the cloth, simply hold right mouse button.

If you have additional side buttons (mouse 5), hold it and drag through the cloth to see another effect.