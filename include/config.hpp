#include <iostream>
#include "yaml-cpp/yaml.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace BS_CONFIG {
  YAML::Node make_default_config();

  extern const YAML::Node DEFAULT_CONFIG;

  struct bs_config {
    int server_port;
  };

  class ConfigReader {
  public:
    static bs_config readConfig(std::filesystem::path config_path = "./configs/config.yaml");
  private:
    static void generateDefaultConfig(const std::filesystem::path& path);
  };
}

namespace YAML {
  template<>
  struct convert<BS_CONFIG::bs_config> {
    static Node encode(const BS_CONFIG::bs_config& config) {
      Node node;
      node["server"]["port"] = config.server_port;
      return node;
    }

    static bool decode(const Node& node, BS_CONFIG::bs_config& config) {
      if(!node.IsMap()) {
        return false;
      }
      config.server_port = node["server"]["port"].as<int>();
      return true;
    }
  };
}