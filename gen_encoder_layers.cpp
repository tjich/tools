#include <cstdio>
#include <vector>
#include <string>
#include <fstream>

#include "json.hpp"
#include "fifo_map.hpp"

template <class K, class V, class dummy_compare, class A>
using WorkaroundFifoMap = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using MyJson = nlohmann::basic_json<WorkaroundFifoMap>;

void GenPush(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "dlcl.push." + std::to_string(i);
  cur["name"] = name;
  cur["type"] = "Mars.DlclPush";
  cur["input"] = MyJson::array();
  std::string input1 = "dlcl.push." + std::to_string(i - 1);
  std::string input2 = "history." + std::to_string(i);
  cur["input"].push_back(input1);
  cur["input"].push_back(input2);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  input.push_back(cur);
  return;
}

void GenPop(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "dlcl.pop." + std::to_string(i);
  cur["name"] = name;
  cur["type"] = "Mars.DlclPop";
  cur["input"] = MyJson::array();
  std::string input1 = "dlcl.push." + std::to_string(i);
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);
  cur["weight"] = "encoder.history.weight";
  cur["index"] = i;

  input.push_back(cur);
  return;
}

void GenLayernorm1(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".self_attn_layer_norm";
  cur["name"] = name;
  cur["type"] = "Mars.LayerNorm";
  cur["input"] = MyJson::array();
  std::string input1 = "dlcl.pop." + std::to_string(i);
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(name);
  cur["weight"] = "encoder.layers." + std::to_string(i) + ".self_attn_layer_norm.weight";
  cur["bias"] = "encoder.layers." + std::to_string(i) + ".self_attn_layer_norm.bias";

  input.push_back(cur);
  return;
}

void GenRelativeMultiHeadAtt(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".multi_head_att";
  cur["name"] = name;
  cur["type"] = "Mars.RelativeMultiHeadAtt";
  cur["input"] = MyJson::array();
  std::string input1 = "layer." + std::to_string(i) + ".self_attn_layer_norm";
  cur["input"].push_back(input1);
  cur["input"].push_back("mask.in");
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(name);
  cur["num_heads"] = 16;
  cur["query.weight"] = "encoder.layers." + std::to_string(i) + ".self_attn.q_proj.weight";
  cur["query.bias"] = "encoder.layers." + std::to_string(i) + ".self_attn.q_proj.bias";
  cur["key.weight"] = "encoder.layers." + std::to_string(i) + ".self_attn.k_proj.weight";
  cur["key.bias"] = "encoder.layers." + std::to_string(i) + ".self_attn.k_proj.bias";
  cur["value.weight"] = "encoder.layers." + std::to_string(i) + ".self_attn.v_proj.weight";
  cur["value.bias"] = "encoder.layers." + std::to_string(i) + ".self_attn.v_proj.bias";
  cur["out.weight"] = "encoder.layers." + std::to_string(i) + ".self_attn.out_proj.weight";
  cur["out.bias"] = "encoder.layers." + std::to_string(i) + ".self_attn.out_proj.bias";
  cur["relative_key.weight"] = "encoder.layers." + std::to_string(i) + ".self_attn.relative_keys_embedding.weight";
  cur["relative_value.weight"] = "encoder.layers." + std::to_string(i) + ".self_attn.relative_values_embedding.weight";
  cur["max_relative_positioin"] = 14;

  input.push_back(cur);
  return;
}

void GenAdd1(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".residual.0";
  cur["name"] = name;
  cur["type"] = "Add";
  cur["input"] = MyJson::array();
  std::string input1 = "dlcl.pop." + std::to_string(i);
  std::string input2 = "layer." + std::to_string(i) + ".multi_head_att";
  cur["input"].push_back(input1);
  cur["input"].push_back(input2);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  input.push_back(cur);
  return;
}

void GenLayernorm2(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".final_layer_norm";
  cur["name"] = name;
  cur["type"] = "Mars.LayerNorm";
  cur["input"] = MyJson::array();
  std::string input1 = "layer." + std::to_string(i) + ".residual.0";
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  cur["weight"] = "encoder.layers." + std::to_string(i) + ".final_layer_norm.weight";
  cur["bias"] = "encoder.layers." + std::to_string(i) + ".final_layer_norm.bias";

  input.push_back(cur);
  return;
}

void GenLinear1(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".fc1";
  cur["name"] = name;
  cur["type"] = "Mars.Linear";
  cur["input"] = MyJson::array();
  std::string input1 = "layer." + std::to_string(i) + ".final_layer_norm";
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  cur["transB"] = 1;

  cur["weight"] = "encoder.layers." + std::to_string(i) + ".fc1.weight";
  cur["bias"] = "encoder.layers." + std::to_string(i) + ".fc1.bias";

  input.push_back(cur);
  return;
}

void GenRelu(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".fc1.relu";
  cur["name"] = name;
  cur["type"] = "Relu";
  cur["input"] = MyJson::array();
  std::string input1 = "layer." + std::to_string(i) + ".fc1";
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  input.push_back(cur);
  return;
}

void GenLinear2(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".fc2";
  cur["name"] = name;
  cur["type"] = "Mars.Linear";
  cur["input"] = MyJson::array();
  std::string input1 = "layer." + std::to_string(i) + ".fc1.relu";
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  cur["transB"] = 1;

  cur["weight"] = "encoder.layers." + std::to_string(i) + ".fc2.weight";
  cur["bias"] = "encoder.layers." + std::to_string(i) + ".fc2.bias";

  input.push_back(cur);
  return;
}

void GenAdd2(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "layer." + std::to_string(i) + ".residual.1";
  cur["name"] = name;
  cur["type"] = "Add";
  cur["input"] = MyJson::array();
  std::string input1 = "layer." + std::to_string(i) + ".residual.0";
  std::string input2 = "layer." + std::to_string(i) + ".fc2";
  cur["input"].push_back(input1);
  cur["input"].push_back(input2);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  input.push_back(cur);
  return;
}

void GenLayernorm3(MyJson &input, int i)
{
  MyJson cur;
  std::string name = "history." + std::to_string(i + 1);
  cur["name"] = name;
  cur["type"] = "Mars.LayerNorm";
  cur["input"] = MyJson::array();
  std::string input1 = "layer." + std::to_string(i) + ".residual.1";
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(output1);

  cur["weight"] = "encoder.history.layer_norms." + std::to_string(i + 1) + ".weight";
  cur["bias"] = "encoder.history.layer_norms." + std::to_string(i + 1) + ".bias";

  input.push_back(cur);
  return;
}

void GenFinalEncoderOut(MyJson &input, int i) {
  MyJson cur;
  std::string name = "encoder.out";
  cur["name"] = name;
  cur["type"] = "Mars.LayerNorm";
  cur["input"] = MyJson::array();
  std::string input1 = "dlcl.pop." + std::to_string(i);
  cur["input"].push_back(input1);
  cur["output"] = MyJson::array();
  std::string output1 = name;
  cur["output"].push_back(name);
  cur["weight"] = "encoder.layer_norm.weight";
  cur["bias"] = "encoder.layer_norm.bias";

  input.push_back(cur);
  return;
}

void GenerateLayers(MyJson &input, int i, bool is_final)
{
  GenPush(input, i);
  GenPop(input, i);
  if (is_final) {
    GenFinalEncoderOut(input, i);
    return;
  }
  GenLayernorm1(input, i);
  GenRelativeMultiHeadAtt(input, i);
  GenAdd1(input, i);
  GenLayernorm2(input, i);
  GenLinear1(input, i);
  GenRelu(input, i);
  GenLinear2(input, i);
  GenAdd2(input, i);
  GenLayernorm3(input, i);
  return;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("usage: ./gen num\n");
  }
  std::string argv1(argv[1]);
  int layers = stoi(argv1);
  printf("layers: %d\n", layers);

  MyJson result = MyJson::array();
  for (int i = 1; i <= layers; i++)
  {
    GenerateLayers(result, i, i == layers);
  }

  std::ofstream ofs("output.json");
  std::string json_str = result.dump(4);
  ofs << json_str;

  return 0;
}