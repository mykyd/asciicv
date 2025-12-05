# asciicv

Basic command-line image to ascii converter. Uses OpenCV for image processing and Ncurses for TUI.
Has some neat navigation while in the ncurses window.

# Install

```
git clone https://github.com/mykyd/asciicv
cd asciicv
g++ asciicv.cpp -o asciicv $(pkg-config --cflags --libs opencv4) -l ncurses -l tinfo
```

# Usage
```
./asciicv <path> <height[px]> [save_file_name]
```
  While you are in the ncurses window:
  ```
  k/j: scroll up/down
  h/l: scroll left/right
  q: quit
  s: save
  ```
You can either save immediately, or after looking at the output
