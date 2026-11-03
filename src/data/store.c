#include "store.h"

#include <errno.h>
#include <sqlite3.h>

struct _ArkiumStore {
  sqlite3 *db;
};

#define ARKIUM_STORE_ERROR arkium_store_error_quark()
G_DEFINE_QUARK(arkium-store-error-quark, arkium_store_error)

static gboolean
exec_sql(sqlite3 *db, const char *sql, GError **error) {
  char *errmsg = NULL;
  if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
    g_set_error(error, ARKIUM_STORE_ERROR, 0, "%s", errmsg);
    sqlite3_free(errmsg);
    return FALSE;
  }
  return TRUE;
}

ArkiumStore *
arkium_store_open(GError **error) {
  g_autofree char *dir =
      g_build_filename(g_get_user_data_dir(), "arkium", NULL);
  if (g_mkdir_with_parents(dir, 0700) != 0) {
    g_set_error(error, ARKIUM_STORE_ERROR, 0,
                "could not create %s: %s", dir, g_strerror(errno));
    return NULL;
  }

  g_autofree char *db_path = g_build_filename(dir, "arkium.db", NULL);

  sqlite3 *db = NULL;
  if (sqlite3_open(db_path, &db) != SQLITE_OK) {
    g_set_error(error, ARKIUM_STORE_ERROR, 0, "could not open %s: %s",
                db_path, sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }

  static const char *schema =
      "CREATE TABLE IF NOT EXISTS settings ("
      "  key TEXT PRIMARY KEY,"
      "  value TEXT"
      ");"
      "CREATE TABLE IF NOT EXISTS favorites ("
      "  novel_path TEXT PRIMARY KEY,"
      "  favorite INTEGER NOT NULL"
      ");"
      "CREATE TABLE IF NOT EXISTS progress ("
      "  novel_path TEXT PRIMARY KEY,"
      "  chapter_path TEXT NOT NULL,"
      "  scroll_fraction REAL NOT NULL,"
      "  updated_at INTEGER NOT NULL"
      ");";
  if (!exec_sql(db, schema, error)) {
    sqlite3_close(db);
    return NULL;
  }

  ArkiumStore *store = g_new0(ArkiumStore, 1);
  store->db = db;
  return store;
}

void
arkium_store_close(ArkiumStore *store) {
  if (!store) {
    return;
  }
  sqlite3_close(store->db);
  g_free(store);
}

char *
arkium_store_get_library_root(ArkiumStore *store) {
  sqlite3_stmt *stmt;
  const char *sql = "SELECT value FROM settings WHERE key = 'library_root'";
  if (sqlite3_prepare_v2(store->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    return NULL;
  }

  char *result = NULL;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    result = g_strdup((const char *)sqlite3_column_text(stmt, 0));
  }
  sqlite3_finalize(stmt);
  return result;
}

gboolean
arkium_store_set_library_root(ArkiumStore *store, const char *path) {
  sqlite3_stmt *stmt;
  const char *sql =
      "INSERT INTO settings (key, value) VALUES ('library_root', ?1) "
      "ON CONFLICT(key) DO UPDATE SET value = excluded.value";
  if (sqlite3_prepare_v2(store->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    return FALSE;
  }
  sqlite3_bind_text(stmt, 1, path, -1, SQLITE_TRANSIENT);
  gboolean ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

gboolean
arkium_store_get_favorite(ArkiumStore *store, const char *novel_path) {
  sqlite3_stmt *stmt;
  const char *sql = "SELECT favorite FROM favorites WHERE novel_path = ?1";
  if (sqlite3_prepare_v2(store->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    return FALSE;
  }
  sqlite3_bind_text(stmt, 1, novel_path, -1, SQLITE_TRANSIENT);

  gboolean favorite = FALSE;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    favorite = sqlite3_column_int(stmt, 0) != 0;
  }
  sqlite3_finalize(stmt);
  return favorite;
}

gboolean
arkium_store_set_favorite(ArkiumStore *store, const char *novel_path,
                           gboolean favorite) {
  sqlite3_stmt *stmt;
  const char *sql =
      "INSERT INTO favorites (novel_path, favorite) VALUES (?1, ?2) "
      "ON CONFLICT(novel_path) DO UPDATE SET favorite = excluded.favorite";
  if (sqlite3_prepare_v2(store->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    return FALSE;
  }
  sqlite3_bind_text(stmt, 1, novel_path, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, favorite ? 1 : 0);
  gboolean ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

gboolean
arkium_store_set_progress(ArkiumStore *store, const char *novel_path,
                           const char *chapter_path,
                           double scroll_fraction) {
  sqlite3_stmt *stmt;
  const char *sql =
      "INSERT INTO progress (novel_path, chapter_path, scroll_fraction, "
      "updated_at) VALUES (?1, ?2, ?3, ?4) "
      "ON CONFLICT(novel_path) DO UPDATE SET "
      "  chapter_path = excluded.chapter_path,"
      "  scroll_fraction = excluded.scroll_fraction,"
      "  updated_at = excluded.updated_at";
  if (sqlite3_prepare_v2(store->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    return FALSE;
  }
  sqlite3_bind_text(stmt, 1, novel_path, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, chapter_path, -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt, 3, scroll_fraction);
  sqlite3_bind_int64(stmt, 4, (sqlite3_int64)g_get_real_time());
  gboolean ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

gboolean
arkium_store_get_progress(ArkiumStore *store, const char *novel_path,
                           char **out_chapter_path,
                           double *out_scroll_fraction) {
  sqlite3_stmt *stmt;
  const char *sql =
      "SELECT chapter_path, scroll_fraction FROM progress "
      "WHERE novel_path = ?1";
  if (sqlite3_prepare_v2(store->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    return FALSE;
  }
  sqlite3_bind_text(stmt, 1, novel_path, -1, SQLITE_TRANSIENT);

  gboolean found = FALSE;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    if (out_chapter_path) {
      *out_chapter_path =
          g_strdup((const char *)sqlite3_column_text(stmt, 0));
    }
    if (out_scroll_fraction) {
      *out_scroll_fraction = sqlite3_column_double(stmt, 1);
    }
    found = TRUE;
  }
  sqlite3_finalize(stmt);
  return found;
}
