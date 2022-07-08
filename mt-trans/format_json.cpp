#include <cstdio>
#include <iostream>
#include <fstream>

#include "json.hpp"
#include "fifo_map.hpp"

template <class K, class V, class dummy_compare, class A>
using WorkaroundFifoMap = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using MyJson = nlohmann::basic_json<WorkaroundFifoMap>;

MyJson ReadJson(const std::string &json_filename)
{
  MyJson mars_json;
  std::ifstream ifs(json_filename);
  if (!ifs.good())
  {
    printf("[Error] Cannot open file: %s\n", json_filename.c_str());
    return mars_json;
  }
  ifs >> mars_json;
  return mars_json;
}

void FormartJson(int argc, char **argv)
{
  if (argc != 3)
  {
    printf("usage: ./format_json ./input.json ./output.json\n");
    return;
  }

  MyJson my_json = ReadJson(std::string(argv[1]));
  std::string json_str = my_json.dump(4);
  std::string out_filename = std::string(argv[2]);
  std::ofstream ofs(out_filename);
  if (ofs.good())
  {
    ofs << json_str;
  }
  else
  {
    printf("[Error] Cannot open file: %s\n", argv[2]);
  }

  return;
}

int main(int argc, char **argv)
{
  FormartJson(argc, argv);
  return 0;
}