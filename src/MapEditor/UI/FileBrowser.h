//
// Created by kkeiper1103 on 5/25/2023.
//

#ifndef PREDATION_FILEBROWSER_H
#define PREDATION_FILEBROWSER_H

#include <cassert>
#include <dirent.h>
#include <unistd.h>

#ifndef _WIN32
# include <pwd.h>
#endif

#include <nuklear/nuklear.h>
#include <physfs.h>

/* ===============================================================
 *
 *                          GUI
 *
 * ===============================================================*/
struct icons {
    struct nk_image desktop;
    struct nk_image home;
    struct nk_image computer;
    struct nk_image directory;

    struct nk_image default_file;
    struct nk_image text_file;
    struct nk_image music_file;
    struct nk_image font_file;
    struct nk_image img_file;
    struct nk_image movie_file;
};

enum file_groups {
    FILE_GROUP_DEFAULT,
    FILE_GROUP_TEXT,
    FILE_GROUP_MUSIC,
    FILE_GROUP_FONT,
    FILE_GROUP_IMAGE,
    FILE_GROUP_MOVIE,
    FILE_GROUP_MAX
};

enum file_types {
    FILE_DEFAULT,
    FILE_TEXT,
    FILE_C_SOURCE,
    FILE_CPP_SOURCE,
    FILE_HEADER,
    FILE_CPP_HEADER,
    FILE_MP3,
    FILE_WAV,
    FILE_OGG,
    FILE_TTF,
    FILE_BMP,
    FILE_PNG,
    FILE_JPEG,
    FILE_PCX,
    FILE_TGA,
    FILE_GIF,
    FILE_MAX
};

struct file_group {
    enum file_groups group;
    const char *name;
    struct nk_image *icon;
};

struct file {
    enum file_types type;
    const char *suffix;
    enum file_groups group;
};

struct media {
    int font;
    int icon_sheet;
    struct icons icons;
    struct file_group group[FILE_GROUP_MAX];
    struct file files[FILE_MAX];

    ~media() {
        glDeleteTextures(1,(const GLuint*)&icons.home.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.directory.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.computer.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.desktop.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.default_file.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.text_file.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.music_file.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.font_file.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.img_file.handle.id);
        glDeleteTextures(1,(const GLuint*)&icons.movie_file.handle.id);
    }
};

#define MAX_PATH_LEN 512
struct file_browser {
    /* path */
    char file[MAX_PATH_LEN];
    char home[MAX_PATH_LEN];
    char desktop[MAX_PATH_LEN];
    char directory[MAX_PATH_LEN];

    /* directory content */
    char **files;
    char **directories;
    size_t file_count;
    size_t dir_count;
    struct media *media;
};


char* str_duplicate(const char *src);
void dir_free_list(char **list, size_t size);
char** dir_list(const char *dir, int return_subdirs, size_t *count);
struct file_group FILE_GROUP(enum file_groups group, const char *name, struct nk_image *icon);
struct file FILE_DEF(enum file_types type, const char *suffix, enum file_groups group);
struct nk_image* media_icon_for_file(struct media *media, const char *file);

void media_init(struct media *media);
void file_browser_reload_directory_content(struct file_browser *browser, const char *path);
void file_browser_init(struct file_browser *browser, struct media *media);
void file_browser_free(struct file_browser *browser);

struct nk_image icon_load(const char *filename);

void ui_file_browser(nk_context* ctx, struct file_browser* browser, const char* base);

#endif //PREDATION_FILEBROWSER_H
