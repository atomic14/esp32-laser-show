#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "zlib.h"

class GZipFile
{
private:
  FILE *fp;
  z_stream stream;
  uint8_t *in_buffer;

public:
  GZipFile();
  bool open(const char *path);
  int read(uint8_t *dst, size_t num_bytes);
  void close();
};