#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"

namespace esphome {
namespace sd_card_logger {

class SDCardLogger : public Component, public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                                               spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_1MHZ> {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_mount_point(const char *mount_point) { mount_point_ = mount_point; }
  void set_cs_pin_index(int pin) { cs_pin_index_ = pin; }
  void set_spi_host(int host) { spi_host_ = host; }
  void set_frequency(int freq_khz) { freq_khz_ = freq_khz; }
  
  bool is_mounted() const { return mounted_; }
  void unmount();
  
  void log_data(const std::string &data);
  void log_battery_data(float voltage, float level); // Keep for backward compat or migratercent, float drain_rate);
  
  uint64_t get_total_bytes() const;
  uint64_t get_used_bytes() const;

 protected:
  const char *mount_point_{"/sd"};
  int cs_pin_index_{-1};
  int spi_host_{1}; 
  int freq_khz_{400};
  sdmmc_card_t *card_{nullptr};
  bool mounted_{false};
  uint64_t total_bytes_{0};
  
  void create_log_file_();
};

}  // namespace sd_card_logger
}  // namespace esphome
