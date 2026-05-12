# duration

simple command-line tool that calculates total duration of video or audio files.

uses libavformat (ffmpeg) instead of external ffprobe process, so it is much faster.

---

## build

```bash
make
````

---

## install (optional)

```bash
chmod +x install.sh
./install.sh
```

installs binary to `/usr/local/bin/duration`

---

## usage

```bash
duration [options] <files|directories>
```

if no arguments are provided, current directory is scanned.

---

## options

```txt
-v        video duration (default)
-a        audio duration
-all      scan folders recursively
-l        list duration of each file
-h        show help
```

---

## examples

```bash
duration
```

scan current directory (videos by default)

```bash
duration -all videos/
```

scan all video files recursively

```bash
duration -a music/
```

scan audio files in folder

```bash
duration -l -all videos/
```

list each file duration + total

---

## notes

* uses ffmpeg libraries internally (libavformat, libavutil)
* no external ffprobe process is used anymore
* significantly faster for large directories
* spinner is used for progress display

