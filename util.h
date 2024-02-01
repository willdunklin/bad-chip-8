#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include <stdlib.h>

#define UTIL_INSTRUCTION_START 0x200 // where CHIP-8 programs start in memory
#define UTIL_INIT_CAP 256

#define util_da_append(da, item)                                                   \
  do {                                                                             \
    if ((da)->count >= (da)->capacity) {                                           \
        (da)->capacity = (da)->capacity == 0 ? UTIL_INIT_CAP : (da)->capacity*2;   \
        (da)->items = realloc((da)->items, sizeof(*(da)->items) * (da)->capacity); \
    }                                                                              \
    (da)->items[(da)->count++] = (item);                                           \
  } while (0)

#define util_da_free(da) free((da)->items)

#define util_da_append_many(da, items, new_count)                                    \
  do {                                                                               \
    if ((da)->count + (new_count) >= (da)->capacity) {                               \
        if ((da)->capacity == 0) (da)->capacity = UTIL_INIT_CAP;                     \
        while ((da)->count + (new_count) >= (da)->capacity) {                        \
            (da)->capacity *= 2;                                                     \
        }                                                                            \
        (da)->items = realloc((da)->items, sizeof(*(da)->items) * (da)->capacity);   \
    }                                                                                \
    memcopy((da)->items + (da)->count, (items), sizeof(*(da)->items) * (new_count)); \
    (da)->count += (new_count);                                                      \
  } while (0)

typedef struct {
  char* items;
  size_t count;
  size_t capacity;
} String;

typedef struct {
  char** items;
  size_t count;
  size_t capacity;
} CString_List;

bool util_read_file(const char *path, String *out) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) return false;

  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  fseek(file, 0, SEEK_SET);

  size_t new_count = out->count + length;
  while (new_count >= out->capacity) {
    if (out->capacity == 0) out->capacity = UTIL_INIT_CAP;
    else                    out->capacity *= 2;

    out->items = realloc(out->items, out->capacity);
  }

  fread(out->items + out->count, length, 1, file);
  out->count = new_count;

  if (file) fclose(file);
  return true;
}

bool util_write_file(const char *path, void *data, size_t length) {
  FILE* file = fopen(path, "wb");
  if (file == NULL) return false;

  const char* buf = data;
  while (length > 0) {
    size_t written = fwrite(buf, 1, length, file);
    if (written == 0) {
      fclose(file);
      return false;
    }
    length -= written;
    buf += written;
  }

  if (file) fclose(file);
  return true;
}

#endif //CHIP8_UTIL_H
