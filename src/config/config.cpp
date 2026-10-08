#include "config.hpp"
#include "logger.hpp"

namespace BS_CONFIG {

  YAML::Node make_default_config() {
    YAML::Node node;
    node["server"]["port"] = 9910;
    return node;
  }

  const YAML::Node DEFAULT_CONFIG = make_default_config();

  bs_config ConfigReader::readConfig(fs::path config_path) {
    std::ifstream input_config_file(config_path);
    if (!input_config_file.is_open()) {
      auto& logger = BS_LOGGER::Logger::getInstance();
      logger.error("Cannot find config file {}", config_path.string());
      generateDefaultConfig(config_path);
      return DEFAULT_CONFIG.as<bs_config>();
    }
    try {
      YAML::Node config_node = YAML::Load(input_config_file);
      bs_config config = config_node.as<bs_config>();
      return config;
    } catch (const YAML::Exception& e) {
      auto& logger = BS_LOGGER::Logger::getInstance();
      logger.error("YAML parse error in {}: {}", config_path.string(), e.what());
      generateDefaultConfig(config_path);  // 重新生成
      return DEFAULT_CONFIG.as<bs_config>();
    }
  }

  void ConfigReader::generateDefaultConfig(const fs::path& path) {
    if (!path.parent_path().empty()) {
      fs::create_directories(path.parent_path());
    }
    std::ofstream output_config_file(path);
    if (output_config_file.is_open()) {
      YAML::Emitter out;
      out.SetMapFormat(YAML::Block);
      out.SetSeqFormat(YAML::Block);
      out << DEFAULT_CONFIG;
      output_config_file << out.c_str();
      auto& logger = BS_LOGGER::Logger::getInstance();
      logger.info("Default config generated at: {}", path.string());
    }
  }
}