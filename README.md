# cribbage-zero
Program for everything cribbage. Finds best discards and best hand play.


## Install
```
git clone https://github.com/derpy-pug/cribbage-zero/
cd cribbage-zero/
```

## Build
#### Manual
In the top level folder with README. Make the build directory.
```
mkdir build
cd build
````

Then make the CMAKE files and compile.
```
cmake -S ../src -B . -DCMAKE_BUILD_TYPE={Debug/Release}
make -j
```

Executable location in build/app/. Executable name `app`. If in build folder.
```
app/app
```

#### Automatic

In the top level folder with README. Run the setup script.
```
./build {Debug/Release}
```

Then run with
```
./cribbage
```
