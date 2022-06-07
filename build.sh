mkdir -p bin
g++ -std=c++11 ./gen_encoder_layers.cpp -o gen
mv ./gen ./bin
g++ -std=c++11 ./format_json.cpp -o format
mv ./format ./bin
g++ -std=c++11 ./vocab_converter.cpp -o vocab_converter
mv ./vocab_converter ./bin