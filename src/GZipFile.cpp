#include "esp_err.h"
#include "esp_log.h"

#include "GZipFile.h"

static const char *TAG = "gzip";

GZipFile::GZipFile() : stream({})
{
}

bool GZipFile::open(const char *path)
{
  fp = fopen(path, "rb");
  if (fp == NULL)
  {
    ESP_LOGE(TAG, "Failed to open file");
    return false;
  }
  // read a chunk of data from the file
  in_buffer = (uint8_t *)malloc(1000);
  stream.avail_in = fread(in_buffer, 1, 1000, fp);
  stream.next_in = in_buffer;
  stream.zalloc = NULL;
  stream.zfree = NULL;
  stream.avail_out = 0;
  stream.next_out = NULL;
  int ret = inflateInit2(&stream, MAX_WBITS | 16);
  if (ret != Z_OK)
  {
    ESP_LOGE(TAG, "Failed to init zip file");
  }
  return true;
}

int GZipFile::read(uint8_t *dst, size_t num_bytes)
{
  stream.next_out = dst;
  stream.avail_out = num_bytes;
  while (stream.avail_out > 0)
  {
    int ret = inflate(&stream, Z_NO_FLUSH);
    if (ret == Z_STREAM_END)
    {
      // number of bytes read
      return num_bytes - stream.avail_out;
    }
    // need more data
    if (stream.avail_in == 0)
    {
      stream.avail_in = fread(in_buffer, 1, 1000, fp);
      stream.next_in = in_buffer;
    }
  }
  return num_bytes;
}

void GZipFile::close()
{
  fclose(fp);
  free(in_buffer);
  inflateEnd(&stream);
}
