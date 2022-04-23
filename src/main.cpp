#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <arpa/inet.h>
#include <vector>

#include "ILDAFile.h"
#include "SPIRenderer.h"

static const char *TAG = "main";

extern "C"
{
  void app_main(void);
}

static const char *files[] = {
    "/spiffs/Xwing.ild.gz",
    "/spiffs/LemmTumble.ild.gz",
    "/spiffs/Vader.ild.gz",
    "/spiffs/Enterprise2.ild.gz"};
static const int num_files = 4;

void app_main()
{
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false};

  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    }
    else
    {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return;
  }
  // read all the files in
  std::vector<ILDAFile *> ilda_files;
  for (int i = 0; i < num_files; i++)
  {
    ILDAFile *ilda = new ILDAFile();
    ilda->read(files[i]);
    ilda_files.push_back(ilda);
    // feed the watchdog so we don't get a timeout
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  esp_vfs_spiffs_unregister(NULL);

  SPIRenderer *renderer = new SPIRenderer(ilda_files);
  renderer->start();
  // run forever
  while (true)
  {
    vTaskDelay(600000 / portTICK_PERIOD_MS);
  }
}