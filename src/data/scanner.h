#pragma once

#include "novel.h"

G_BEGIN_DECLS

/*
 * Scans a library root folder into an ArkiumLibrary.
 *
 * Shape matches docs/ARCHITECTURE.md's "Library Folder Model":
 * immediate subfolders of root_path are novels; a novel's immediate
 * subfolders (if any) are arcs, each holding chapter files; a novel
 * with no subfolders holds its chapter files directly. Only .txt and
 * .md files are considered chapters. Files are read in place, never
 * copied.
 *
 * Chapter ordering/titles use the heuristic described there: a
 * leading "Chapter N" / "N" prefix in the filename, falling back to
 * filename tokenization (underscores -> spaces) when no number is
 * found.
 */
ArkiumLibrary *arkium_scan_library(const char *root_path, GError **error);

G_END_DECLS
