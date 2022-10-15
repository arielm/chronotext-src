# chronotext-src

Based on [chronotext-cross](https://github.com/arielm/chronotext-cross) for infrastructure. See also: [chronotext.org](http://chronotext.org).

Source code for the chronotext experiments:
- [TextNoise](sketches/TextNoise)
- [SpiralLandscape](sketches/SpiralLandscape)
- [Desert](sketches/Desert)
- [Cliffs](sketches/Cliffs)

***

1. Unless the experiment has its own README file, follow [these instructions](https://arielm.github.io/cross-blog/2022/10/06/hello-world.html) for running on macOS or Linux, or [these instructions](https://arielm.github.io/cross-blog/2022/10/06/running-in-the-browser.html) for running in the browser.

2. Clone this repository with:
   ```
   git clone https://github.com/arielm/chronotext-src
   ```

3. Add `SRC_PATH` variable:
   ```
   export SRC_PATH=path/to/chronotext-src
   ```

4. Pick a sketch, e.g.
   ```
   cd $SRC_PATH/sketches/SpiralLandscape
   ```

5. Run for your platform of choice:
   ```
   RUN_TEST -DPLATFORM=???
   ```
   Supported platform identifiers: `osx`, `linux`, `emscripten`
