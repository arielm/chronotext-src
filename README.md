# chronotext-src

Based on [chronotext-cross](https://github.com/arielm/chronotext-cross) for infrastructure. See also: [chronotext.org](http://chronotext.org).

Source code for the chronotext experiments:
- [TextNoise](sketches/TextNoise)
- [Spiral Landscape](sketches/SpiralLandscape)
- [Desert](sketches/Desert)
- [Cliffs](sketches/Cliffs)
- [Water](sketches/Water)
- [Topographic Text](sketches/TopographicText)
- [Steal](sketches/Steal)
- [Fourty Five](sketches/FourtyFive)
- [Babel Tower](sketches/BabelTower)
- [The Text Time Curvature](sketches/TTC)
- [The Book of Sand](sketches/BookOfSand)
- [Sliding Text](sketches/SlidingText)
- [Turn Table](sketches/TurnTable)
- [Laser Guided](sketches/LaserGuided)
- [Height Variation](sketches/HeightVariation)
- [Straight in the Desert](sketches/StraightInTheDesert)
- [Javascriptorium](sketches/JS)
- [Ho Tokyo, Tokyo](sketches/Tokyo)
- [Rock Around the Bunker](sketches/Branly)
- [From Weimar to Kingston](sketches/TokyoBehind)
- [Décamouflé de Branly](sketches/BranlyBehind)
- [The War of the Words](sketches/WarOfTheWords)
- [Twitter Maze](sketches/TwitterMaze)
- [Lui les Hébreux Moi Pharaon](sketches/Chanson)

***

1. Unless the experiment has its own instructions, follow [these instructions](https://arielm.github.io/cross-blog/2022/10/06/hello-world.html) for running on macOS or Linux, or [these instructions](https://arielm.github.io/cross-blog/2022/10/06/running-in-the-browser.html) for running in the browser.

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
