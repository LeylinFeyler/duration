# duration

Simple command-line tool that calculates total duration of video or audio files.

Uses **ffprobe** from ffmpeg.

## build

```
make
```

## usage

```
duration [options] <files|directories>
```

## options

```
-v        video duration (default)
-a        audio duration
-all      scan folders recursively
-l        list duration of each file
-h        show help
```

## example

```
duration
duration -all videos/
duration -a music/
duration -l -all videos/
```
