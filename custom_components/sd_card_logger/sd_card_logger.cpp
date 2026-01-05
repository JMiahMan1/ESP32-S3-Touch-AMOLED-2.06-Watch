#include "sd_card_logger.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include <esp_vfs_fat.h>
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>
#include <sys/stat.h>

namespace esphome {
namespace sd_card_logger {

static const char *const TAG = "sd_card_logger";

void SDCardLogger::setup() {
  printf("DEBUG: SDCardLogger::setup() starting\n");
  ESP_LOGI(TAG, "Setting up SD Card Logger...");

  // Configure SPI bus for SD card
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot = (spi_host_device_t) this->spi_host_;
  host.max_freq_khz = this->freq_khz_;
  
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = (gpio_num_t) this->cs_pin_index_;
  slot_config.host_id = (spi_host_device_t) this->spi_host_;

  // Mount filesystem
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024
  };

  ESP_LOGI(TAG, "Attempting to mount SD card at %s (CS: %d, Host: %d, Freq: %dkHz)", 
           this->mount_point_, this->cs_pin_index_, this->spi_host_, this->freq_khz_);

  esp_err_t ret = esp_vfs_fat_sdspi_mount(this->mount_point_, &host, &slot_config, &mount_config, &this->card_);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount filesystem.");
      ESP_LOGE(TAG, "  -> This usually means the card is detected but not formatted as FAT32.");
      ESP_LOGE(TAG, "  -> If you have a large card (>32GB), try formatting it to FAT32 manually.");
    } else if (ret == ESP_ERR_TIMEOUT) {
      ESP_LOGE(TAG, "SD Card Mount Timeout.");
      ESP_LOGE(TAG, "  -> Check connections and power. Ensure card is fully inserted.");
    } else {
      ESP_LOGE(TAG, "Failed to initialize SD card: %s (0x%x)", esp_err_to_name(ret), ret);
    }
    this->mounted_ = false;
    // Even if mount failed, 'card' structure might be populated if init succeeded but mount failed? 
    // esp_vfs_fat_sdspi_mount likely cleans up on failure, so we can't trust 'this->card_'.
    return;
  }

  this->mounted_ = true;

  // Print card info (if stdout available)
  // sdmmc_card_print_info(stdout, this->card_);
  ESP_LOGI(TAG, "SD Card mounted successfully at %s", this->mount_point_);

  this->total_bytes_ = ((uint64_t) this->card_->csd.capacity) * this->card_->csd.sector_size;
  ESP_LOGI(TAG, "SD Card Size: %lluMB", this->total_bytes_ / (1024 * 1024));

  // Create log file with headers if it doesn't exist
  this->create_log_file_();
}

void SDCardLogger::unmount() {
  if (!this->mounted_) return;
  
  ESP_LOGI(TAG, "Unmounting SD Card...");
  esp_err_t err = esp_vfs_fat_sdcard_unmount(this->mount_point_, this->card_);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "SD Card unmounted successfully.");
    this->mounted_ = false;
    this->card_ = nullptr;
  } else {
    ESP_LOGE(TAG, "Failed to unmount SD Card: %s", esp_err_to_name(err));
  }
}

uint64_t SDCardLogger::get_total_bytes() const {
  if (!this->mounted_) return 0;
  return this->total_bytes_;
}

uint64_t SDCardLogger::get_used_bytes() const {
  if (!this->mounted_) return 0;
  
  uint64_t total = 0, free = 0;
  esp_err_t err = esp_vfs_fat_info(this->mount_point_, &total, &free);
  if (err == ESP_OK) {
    return total - free;
  }
  return 0;
}

void SDCardLogger::dump_config() {
  ESP_LOGCONFIG(TAG, "SD Card Logger:");
  ESP_LOGCONFIG(TAG, "  Mount Point: %s", this->mount_point_);
  ESP_LOGCONFIG(TAG, "  CS Pin: %d", this->cs_pin_index_);
  ESP_LOGCONFIG(TAG, "  SPI Host: %d", this->spi_host_);
  ESP_LOGCONFIG(TAG, "  Status: %s", this->mounted_ ? "Mounted" : "Not Mounted");
  if (this->mounted_ && this->card_) {
    ESP_LOGCONFIG(TAG, "  Card Size: %lluMB", this->total_bytes_ / (1024 * 1024));
  }
}

void SDCardLogger::create_log_file_() {
  if (!this->mounted_)
    return;

  char filepath[64];
  snprintf(filepath, sizeof(filepath), "%s/battery_log.csv", this->mount_point_);

  // Check if file already exists
  struct stat st;
  if (stat(filepath, &st) == 0) {
    ESP_LOGI(TAG, "Log file exists: %s (size: %ld bytes)", filepath, st.st_size);
    return;
  }

  // Create new file with headers
  FILE *f = fopen(filepath, "w");
  if (f == nullptr) {
    ESP_LOGE(TAG, "Failed to create log file: %s", filepath);
    return;
  }

  fprintf(f, "timestamp,battery_voltage,battery_percent,drain_rate\n");
  fclose(f);
  ESP_LOGI(TAG, "Created new log file: %s", filepath);
}

void SDCardLogger::log_data(const std::string &data) {
  if (!this->mounted_) {
    ESP_LOGW(TAG, "Cannot log data: SD card not mounted.");
    return;
  }

  std::string file_path = std::string(this->mount_point_) + "/battery_log.csv";
  FILE *f = fopen(file_path.c_str(), "a");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for appending.");
    return;
  }

  // Get current time
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);

  fprintf(f, "%s, %s\n", time_str, data.c_str());
  fclose(f);
  ESP_LOGI(TAG, "Logged data: %s, %s", time_str, data.c_str());
}


void SDCardLogger::log_battery_data(float voltage, float level) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%.2fV, %.1f%%", voltage, level);
    this->log_data(std::string(buf));
}

}  // namespace sd_card_logger
}  // namespace esphome
