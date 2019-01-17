#ifndef MEDIA_OBJECT_H
#define MEDIA_OBJECT_H 1
struct Media_Object {
  char *file_url_name;
  char *name;
  char *cookies;
  char *headers;
  int web_content;
  int is_web_file;
  int is_playing;
  int id;
  int argc;
  char **argv;
};

#endif