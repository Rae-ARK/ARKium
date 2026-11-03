#include "novel.h"

ArkiumChapter *
arkium_chapter_new(const char *path, const char *title, const char *arc,
                    int parsed_number) {
  ArkiumChapter *chapter = g_new0(ArkiumChapter, 1);
  chapter->path = g_strdup(path);
  chapter->title = g_strdup(title);
  chapter->arc = arc ? g_strdup(arc) : NULL;
  chapter->parsed_number = parsed_number;
  return chapter;
}

void
arkium_chapter_free(ArkiumChapter *chapter) {
  if (!chapter) {
    return;
  }
  g_free(chapter->path);
  g_free(chapter->title);
  g_free(chapter->arc);
  g_free(chapter);
}

ArkiumNovel *
arkium_novel_new(const char *path, const char *title) {
  ArkiumNovel *novel = g_new0(ArkiumNovel, 1);
  novel->path = g_strdup(path);
  novel->title = g_strdup(title);
  novel->chapters =
      g_ptr_array_new_with_free_func((GDestroyNotify)arkium_chapter_free);
  return novel;
}

void
arkium_novel_free(ArkiumNovel *novel) {
  if (!novel) {
    return;
  }
  g_free(novel->path);
  g_free(novel->title);
  g_ptr_array_free(novel->chapters, TRUE);
  g_free(novel);
}

int
arkium_novel_chapter_index(ArkiumNovel *novel, ArkiumChapter *chapter) {
  for (guint i = 0; i < novel->chapters->len; i++) {
    if (g_ptr_array_index(novel->chapters, i) == chapter) {
      return (int)i;
    }
  }
  return -1;
}

void
arkium_library_free(ArkiumLibrary *library) {
  if (!library) {
    return;
  }
  g_ptr_array_free(library->novels, TRUE);
  g_free(library);
}
