# TextNoise

An iPhone app for playing with text and generating sound ([more details](http://chronotext.org/TextNoise))

It's a mix of C++ / OpenGL and Objective-C / UIKit, relying on the Parse API for server-side database (you will need [proper credentials](https://github.com/arielm/chronotext-src/blob/main/sketches/TextNoise/src/ios/Application.mm#L19-L20) for the app to work fully as intended.)

1. Depends on [chronotext-cross](https://github.com/arielm/chronotext-cross) for infrastructure. Clone with:
```
git clone --recurse-submodules https://github.com/arielm/chronotext-cross
```

2. Clone this repository with:
```
git clone https://github.com/arielm/chronotext-src
```

3. Configure XCode:
- Go to Preferences/Locations/Custom Paths
- Add entry named CROSS_PATH with path pointing to your chronotext-cross repository
- Add entry named SRC_PATH with path pointing to your chronotext-src repository

4. Prepare with Cocoapods
```
cd chronotext-src/sketches/TextNoise/xcode/ios
pod install
```

5. Open TextNoise.xcworkspace
