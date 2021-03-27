# UZU
![scr1](screenshot-1.png)
![scr2](screenshot-2.png)
#  Build Instructions
###  Prerequisites: 
* **gcc**, **clang** compiler 
* **meson**
* **ninja**
### Install dependencies

#### sdl2 libraries
install sdl libraries via apt
```
$ sudo apt-get install libsdl2-dev libsdl2-gfx-dev libsdl2-image libsdl2-mixer
```
#### json-c
We need install json-c library manually
```
$ git clone https://github.com/json-c/json-c.git
$ mkdir json-c-build
$ cd json-c-build
$ cmake ../json-c  -DBUILD_STATIC_LIBS=TRUE
```

```
$make install
```
### Build & run
```
$ meson build
$ ninja -C build install
$ cd dist && ./uzu
```
