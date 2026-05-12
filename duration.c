#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "utils/spinner.h"
#include <libavformat/avformat.h>
#include <libavutil/log.h>

#define MAX_FILES 1000
#define MAX_PATH 4096

// supported extensions for filtering files
const char* video_ext[] = {".mp4", ".mkv", ".avi", ".mov", ".webm"};
const char* audio_ext[] = {".mp3", ".flac", ".wav", ".ogg", ".aac", ".m4a"};

// simple container for file paths
typedef struct {
    char* paths[MAX_FILES];
    int count;
} FileList;


// check if filename has one of the given extensions
int has_extension(const char* filename, const char** extensions, int ext_count) {
    for(int i = 0; i < ext_count; i++) {
        size_t len = strlen(extensions[i]);
        size_t flen = strlen(filename);
        if(flen >= len && strcasecmp(filename + flen - len, extensions[i]) == 0)
            return 1;
    }
    return 0;
}

// stores file path in list
void add_file(FileList* list, const char* path) {
    if(list->count >= MAX_FILES) return;
    list->paths[list->count++] = strdup(path);
}

// recursively scans directory tree
void scan_dir(const char* dir_path, int recursive, const char** extensions, int ext_count, FileList* list) {
    DIR* dir = opendir(dir_path);
    if(!dir) return;

    struct dirent* entry;
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)
            continue;

        char fullpath[MAX_PATH];
        snprintf(fullpath, MAX_PATH, "%s/%s", dir_path, entry->d_name);

        if(entry->d_type == DT_REG) {
            if(has_extension(entry->d_name, extensions, ext_count))
                add_file(list, fullpath);
        }
        else if(entry->d_type == DT_DIR && recursive) {
            scan_dir(fullpath, recursive, extensions, ext_count, list);
        }
    }
    closedir(dir);
}

// reads media duration using ffmpeg library
double get_duration(const char* path) {
    AVFormatContext* fmt_ctx = NULL;

    // open file container
    if(avformat_open_input(&fmt_ctx, path, NULL, NULL) < 0)
        return 0.0;

    // read metadata and stream info
    if(avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        avformat_close_input(&fmt_ctx);
        return 0.0;
    }

    double duration = 0.0;

    // duration is stored in AV_TIME_BASE units
    if(fmt_ctx->duration != AV_NOPTS_VALUE)
        duration = (double)fmt_ctx->duration / AV_TIME_BASE;

    avformat_close_input(&fmt_ctx);

    return duration;
}

// print duration in human readable format
void print_duration(double seconds) {
    int h = (int)(seconds / 3600);
    int m = ((int)seconds % 3600)/60;
    int s = (int)seconds % 60;
    if(h>0) printf("%dh %dm %ds", h,m,s);
    else if(m>0) printf("%dm %ds", m,s);
    else printf("%ds", s);
}

// compares two file paths for sorting
int cmp_paths(const void* a, const void* b) {
    const char* s1 = *(const char**)a;
    const char* s2 = *(const char**)b;
    return strcasecmp(s1,s2);
}

// prints CLI help message
void print_help(const char* prog) {
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║            duration  —  media timer          ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");
    printf("Usage:\n");
    printf("  %s [options] <files|directories>\n\n", prog);
    printf("Options:\n");
    printf("  -v            Calculate video duration (default)\n");
    printf("  -a            Calculate audio duration\n");
    printf("  --all          Include all files recursively from given folders\n");
    printf("                (may take longer depending on file count)\n");
    printf("  -l            List duration of each file (in addition to total)\n");
    printf("  -h, --help    Show this help message\n\n");
    printf("Examples:\n");
    printf("  %s                       → video duration in current folder\n", prog);
    printf("  %s movie.mp4 folder/     → video duration from file + folder (non-recursive)\n", prog);
    printf("  %s --all folder/          → scan all subfolders too (recursive)\n", prog);
    printf("  %s -a music/             → audio duration in folder\n", prog);
    printf("  %s -a --all ~/Music       → audio duration in all subfolders\n\n", prog);
}

int main(int argc, char* argv[]) {
    // disable ffmpeg logs
    av_log_set_level(AV_LOG_QUIET);

    // parse arguments and set mode flags
    int mode_video = 1;
    int recursive = 0;
    int list_durations = 0;
    FileList files = {.count = 0};

    // if no arguments, scan current directory for videos
    if(argc==1) {
        scan_dir(".",0,video_ext,5,&files);
    }

    // handle arguments
    for(int i=1;i<argc;i++) {
        if(strcmp(argv[i],"-v")==0) mode_video=1;
        else if(strcmp(argv[i],"-a")==0) mode_video=0;
        else if(strcmp(argv[i],"--all")==0) recursive=1;
        else if(strcmp(argv[i],"-l")==0) list_durations=1;
        else if(strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0) {
            print_help(argv[0]);
            return 0;
        } else {
            struct stat st;
            if(stat(argv[i], &st)==0) {
                if(S_ISREG(st.st_mode)) add_file(&files, argv[i]);
                else if(S_ISDIR(st.st_mode)) 
                    scan_dir(argv[i], recursive, mode_video?video_ext:audio_ext, mode_video?5:6, &files);
            } else {
                printf("Warning: '%s' not found.\n", argv[i]);
            }
        }
    }

    // if no files found after arguments, scan cwd again
    if(files.count == 0) {
        scan_dir(".", recursive, mode_video ? video_ext : audio_ext, mode_video ? 5 : 6, &files);
    }

    qsort(files.paths, files.count, sizeof(char*), cmp_paths);

    if(files.count==0) {
        printf("No %s files found.\n", mode_video?"video":"audio");
        return 0;
    }

    // start spinner while calculating durations
    Spinner spinner;
    spinner_start(&spinner, list_durations?"Calculating total duration":"Processing files");

    double total_duration = 0.0;
    for(int i=0;i<files.count;i++) {
        double dur = get_duration(files.paths[i]);
        total_duration += dur;

        if(list_durations) {
            spinner_stop(&spinner,"");

            // get relative path from current directory
            static char cwd[1024];
            static size_t cwd_len = 0;
            if(cwd_len == 0) {
                if(getcwd(cwd, sizeof(cwd)) != NULL) {
                    cwd_len = strlen(cwd);
                }
            }

            const char* full_path = files.paths[i];
            const char* rel_path = full_path;

            if(strncmp(full_path, cwd, cwd_len) == 0) {
                rel_path = full_path + cwd_len;
                if(*rel_path == '/' || *rel_path == '\\') rel_path++;
                char tmp[1024];
                snprintf(tmp, sizeof(tmp), "/%s", rel_path);
                rel_path = tmp;
            }

            printf("%s — ", rel_path);
            print_duration(dur);
            printf("\n");

            spinner_start(&spinner,"Calculating total duration");
        }
    }

    spinner_stop(&spinner,"Finished!");
    printf("Total %s duration: ", mode_video?"video":"audio");
    print_duration(total_duration);
    printf("\n");

    for(int i=0;i<files.count;i++) free(files.paths[i]);
    return 0;
}