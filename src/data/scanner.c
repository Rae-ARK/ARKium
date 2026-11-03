#include "scanner.h"

#include <string.h>

/* "123 - Title.md", "Chapter 12.txt", "Ch.07 Title.txt", etc. */
static GRegex *
chapter_number_regex(void) {
  static GRegex *regex = NULL;
  if (!regex) {
    regex = g_regex_new(
        "^(?:chapter|ch\\.?)?\\s*0*([0-9]+)[\\s:._-]*(.*)$",
        G_REGEX_CASELESS, 0, NULL);
  }
  return regex;
}

static gboolean
has_chapter_suffix(const char *name) {
  return g_str_has_suffix(name, ".txt") || g_str_has_suffix(name, ".TXT") ||
         g_str_has_suffix(name, ".md") || g_str_has_suffix(name, ".MD");
}

static char *
strip_known_suffix(const char *name) {
  const char *suffixes[] = {".txt", ".TXT", ".md", ".MD"};
  for (size_t i = 0; i < G_N_ELEMENTS(suffixes); i++) {
    if (g_str_has_suffix(name, suffixes[i])) {
      return g_strndup(name, strlen(name) - strlen(suffixes[i]));
    }
  }
  return g_strdup(name);
}

/* Filename-tokenization fallback: underscores/dashes -> spaces, trimmed. */
static char *
humanize(const char *raw) {
  char *copy = g_strdup(raw);
  for (char *c = copy; *c; c++) {
    if (*c == '_') {
      *c = ' ';
    }
  }
  g_strstrip(copy);
  if (*copy == '\0') {
    g_free(copy);
    return g_strdup("Untitled");
  }
  return copy;
}

static ArkiumChapter *
parse_chapter_file(const char *full_path, const char *filename,
                    const char *arc) {
  g_autofree char *stem = strip_known_suffix(filename);

  GMatchInfo *match_info = NULL;
  int parsed_number = -1;
  g_autofree char *title = NULL;

  if (g_regex_match(chapter_number_regex(), stem, 0, &match_info)) {
    g_autofree char *number_str = g_match_info_fetch(match_info, 1);
    g_autofree char *rest = g_match_info_fetch(match_info, 2);
    parsed_number = (int)g_ascii_strtoll(number_str, NULL, 10);
    g_strstrip(rest);
    if (*rest != '\0') {
      title = humanize(rest);
    } else {
      title = g_strdup_printf("Chapter %d", parsed_number);
    }
  } else {
    title = humanize(stem);
  }
  g_clear_pointer(&match_info, g_match_info_free);

  return arkium_chapter_new(full_path, title, arc, parsed_number);
}

static int
compare_chapters(gconstpointer a, gconstpointer b) {
  ArkiumChapter *ca = *(ArkiumChapter **)a;
  ArkiumChapter *cb = *(ArkiumChapter **)b;
  if (ca->parsed_number >= 0 && cb->parsed_number >= 0) {
    return ca->parsed_number - cb->parsed_number;
  }
  if (ca->parsed_number >= 0) {
    return -1; /* numbered chapters sort before unnumbered ones */
  }
  if (cb->parsed_number >= 0) {
    return 1;
  }
  return g_strcmp0(ca->title, cb->title);
}

static int
compare_strings(gconstpointer a, gconstpointer b) {
  return g_strcmp0(*(const char **)a, *(const char **)b);
}

static int
compare_novels(gconstpointer a, gconstpointer b) {
  ArkiumNovel *na = *(ArkiumNovel **)a;
  ArkiumNovel *nb = *(ArkiumNovel **)b;
  return g_strcmp0(na->title, nb->title);
}

/* Scans one directory's chapter files (not recursing further) into
 * out_chapters, sorted. arc may be NULL. */
static void
scan_chapter_files(const char *dir_path, const char *arc,
                    GPtrArray *out_chapters) {
  GError *error = NULL;
  GDir *dir = g_dir_open(dir_path, 0, &error);
  if (!dir) {
    g_warning("ARKium: could not open %s: %s", dir_path, error->message);
    g_clear_error(&error);
    return;
  }

  GPtrArray *found =
      g_ptr_array_new_with_free_func((GDestroyNotify)arkium_chapter_free);

  const char *name;
  while ((name = g_dir_read_name(dir))) {
    if (name[0] == '.' || !has_chapter_suffix(name)) {
      continue;
    }
    g_autofree char *full_path = g_build_filename(dir_path, name, NULL);
    g_ptr_array_add(found, parse_chapter_file(full_path, name, arc));
  }
  g_dir_close(dir);

  g_ptr_array_sort(found, compare_chapters);
  for (guint i = 0; i < found->len; i++) {
    g_ptr_array_add(out_chapters, g_ptr_array_index(found, i));
  }
  /* Ownership of the elements moved into out_chapters; free just the
   * container. */
  g_ptr_array_set_free_func(found, NULL);
  g_ptr_array_free(found, TRUE);
}

static ArkiumNovel *
scan_novel_folder(const char *novel_path, const char *folder_name) {
  g_autofree char *title = humanize(folder_name);
  ArkiumNovel *novel = arkium_novel_new(novel_path, title);

  GError *error = NULL;
  GDir *dir = g_dir_open(novel_path, 0, &error);
  if (!dir) {
    g_warning("ARKium: could not open %s: %s", novel_path, error->message);
    g_clear_error(&error);
    return novel;
  }

  GPtrArray *subdirs = g_ptr_array_new_with_free_func(g_free);
  gboolean has_direct_chapters = FALSE;

  const char *name;
  while ((name = g_dir_read_name(dir))) {
    if (name[0] == '.') {
      continue;
    }
    g_autofree char *full_path = g_build_filename(novel_path, name, NULL);
    if (g_file_test(full_path, G_FILE_TEST_IS_DIR)) {
      g_ptr_array_add(subdirs, g_strdup(name));
    } else if (has_chapter_suffix(name)) {
      has_direct_chapters = TRUE;
    }
  }
  g_dir_close(dir);

  if (subdirs->len > 0) {
    g_ptr_array_sort(subdirs, compare_strings);
    for (guint i = 0; i < subdirs->len; i++) {
      const char *subdir_name = g_ptr_array_index(subdirs, i);
      g_autofree char *subdir_path =
          g_build_filename(novel_path, subdir_name, NULL);
      g_autofree char *arc_title = humanize(subdir_name);
      scan_chapter_files(subdir_path, arc_title, novel->chapters);
    }
  }
  if (has_direct_chapters) {
    /* Chapters directly in the novel folder count as an un-arced
     * trailing group, after any arc subfolders. */
    scan_chapter_files(novel_path, NULL, novel->chapters);
  }

  g_ptr_array_free(subdirs, TRUE);
  return novel;
}

ArkiumLibrary *
arkium_scan_library(const char *root_path, GError **error) {
  GDir *dir = g_dir_open(root_path, 0, error);
  if (!dir) {
    return NULL;
  }

  ArkiumLibrary *library = g_new0(ArkiumLibrary, 1);
  library->novels =
      g_ptr_array_new_with_free_func((GDestroyNotify)arkium_novel_free);

  const char *name;
  while ((name = g_dir_read_name(dir))) {
    if (name[0] == '.') {
      continue;
    }
    g_autofree char *full_path = g_build_filename(root_path, name, NULL);
    if (!g_file_test(full_path, G_FILE_TEST_IS_DIR)) {
      continue;
    }
    ArkiumNovel *novel = scan_novel_folder(full_path, name);
    if (novel->chapters->len > 0) {
      g_ptr_array_add(library->novels, novel);
    } else {
      arkium_novel_free(novel);
    }
  }
  g_dir_close(dir);

  g_ptr_array_sort(library->novels, compare_novels);
  return library;
}
