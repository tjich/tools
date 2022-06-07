#include <cstdio>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string ParseLine(const std::string& line) {
  std::string space(" ");
  size_t space_pos = line.find(space);
  if (space_pos != std::string::npos) {
    std::string result = line.substr(0, space_pos);
    return result;
  } else {
    printf("[Warn] [VocabConverter] no whitespace in this line [ %s ]\n", line.c_str());
    return line;
  }
}

void ConverterFile(const std::string& input, const std::string& output) {
  std::ifstream ifs(input);
  if (!ifs.good()) {
    printf("[Error] [VocabConverter] Open file error, cannot open %s\n", input.c_str());
    return;
  }
  std::ofstream ofs(output);
  if (!ofs.good()) {
    printf("[Error] [VocabConverter] Open file error, cannot open %s\n", output.c_str());
    return;
  }

  ofs << "<BOS>\n";
  ofs << "<PAD>\n";
  ofs << "<EOS>\n";
  ofs << "UNK\n";
  std::string line;
  while (std::getline(ifs, line)) {
    std::string result = ParseLine(line);
    ofs << result << "\n";
  }
  return;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("usage: ./vocab_converter ./input_vocab ./output_vocab\n");
    return 0;
  }
  std::string input(argv[1]);
  std::string output(argv[2]);

  ConverterFile(input, output);

  return 0;
}