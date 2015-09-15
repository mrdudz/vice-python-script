# Vice with Python scripting.

    ./autogen.sh
    ./configure --enable-sdlui --disable-lame --with-sdlsound --without-pulse --without-alsa --without-oss
    make
    src/x64 -script test.py

## Usage Ideas

* Automated testing of software
* Play text games online / through SSH
* Make long scripted demo shows
* Deluxe versions of games or demos that warps through loader parts & swaps disks automatically
* Debug overlays on screen
* Automated screenshot, audio dumping etc of many files
* Prototyping routines in Python before implementing in Assembler
* Auto playing bots?

## API

### Implemented

```
run_opcodes(opcode_count)
run_frames(frame_count)
warp_frames(frame_count)
break_on(pc, callback)
on_frame(callback)
read_memory(start, size)
attach_disk(disk_image, device)
put_key(ascii)
put_raw_key(col, row, pressed)
get_regs()
```

### Suggested

```
reset()
get_screen()
render_text(x,y,text)
diff_memory(start, size)
write_memory(start, bytes)
get_mem(start)
put_mem(start, byte)
get_a()
get_x()
get_y()
get_pc()
get_sp()
get_sr()
```


