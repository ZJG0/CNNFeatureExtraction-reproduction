CXX=g++
SRC_CPP_FILES     := $(wildcard src/*.cpp)
OBJ_CPP_FILES     := $(wildcard util/*.cpp)
OBJ_FILES    	  := $(patsubst src/%.cpp, src/%.o,$(SRC_CPP_FILES))
OBJ_FILES    	  += $(patsubst util/%.cpp, util/%.o,$(OBJ_CPP_FILES))
HEADER_FILES       = $(wildcard src/*.h)

# FLAGS := -g -O0 -w -std=c++11 -pthread -msse4.1 -maes -msse2 -mpclmul -fpermissive -fpic
FLAGS := -O3 -w -std=c++11 -pthread -msse4.1 -maes -msse2 -mpclmul -fpermissive -fpic
LIBS := -lcrypto -lssl
OBJ_INCLUDES := -I 'lib_eigen/' -I 'util/Miracl/' -I 'util/'
BMR_INCLUDES := $($(OBJ_INCLUDES), -L./)


all: CNNFeatureExtraction.out

CNNFeatureExtraction.out: $(OBJ_FILES)
	g++ $(FLAGS) -o $@ $(OBJ_FILES) $(BMR_INCLUDES) $(LIBS)
%.o: %.cpp $(HEADER_FILES)
	$(CXX) $(FLAGS) -c $< -o $@ $(OBJ_INCLUDES)




clean:
	rm -rf CNNFeatureExtraction.out
	rm -rf src/*.o util/*.o

local: CNNFeatureExtraction.out
	sh local_run

################################################### Test ###################################################
test: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out 3PC 0 files/parties_LAN_3PC files/keyA files/keyAB

################################################### STANDALONE ###################################################
standalone: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out STANDALONE 4 files/parties_localhost files/keyA files/keyAB files/data/mnist_data_8_samples files/data/mnist_labels_8_samples files/data/mnist_data_8_samples files/data/mnist_labels_8_samples
	@echo ""

mnistSA: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out STANDALONE 4 files/parties_localhost files/keyA files/keyAB files/MNIST/mnist_train_data files/MNIST/mnist_train_labels files/MNIST/mnist_train_data files/MNIST/mnist_train_labels

################################################### 4PC ###################################################
# Run all four parties with xterm terminal for A.
four: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out 4PC 3 files/parties_localhost files/keyD files/keyCD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD >/dev/null &
	./CNNFeatureExtraction.out 4PC 2 files/parties_localhost files/keyC files/keyCD files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC >/dev/null &
	./CNNFeatureExtraction.out 4PC 1 files/parties_localhost files/keyB files/keyAB files/data/mnist_data_8_BD files/data/mnist_labels_8_BD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD >/dev/null &
	xterm -ls -xrm 'XTerm*selectToClipboard:true' -hold -e "./CNNFeatureExtraction.out 4PC 0 files/parties_localhost files/keyA files/keyAB files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC " &

mnist4PC: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out 4PC 3 files/parties_localhost files/keyD files/keyCD files/MNIST/mnist_train_data_13 files/MNIST/mnist_train_labels_13 files/MNIST/mnist_train_data_13 files/MNIST/mnist_train_labels_13 >/dev/null &
	./CNNFeatureExtraction.out 4PC 2 files/parties_localhost files/keyC files/keyCD files/MNIST/mnist_train_data_02 files/MNIST/mnist_train_labels_02 files/MNIST/mnist_train_data_02 files/MNIST/mnist_train_labels_02 >/dev/null &
	./CNNFeatureExtraction.out 4PC 1 files/parties_localhost files/keyB files/keyAB files/MNIST/mnist_train_data_13 files/MNIST/mnist_train_labels_13 files/MNIST/mnist_train_data_13 files/MNIST/mnist_train_labels_13 >/dev/null &
	xterm -ls -xrm 'XTerm*selectToClipboard:true' -hold -e "./CNNFeatureExtraction.out 4PC 0 files/parties_localhost files/keyA files/keyAB files/MNIST/mnist_train_data_02 files/MNIST/mnist_train_labels_02 files/MNIST/mnist_train_data_02 files/MNIST/mnist_train_labels_02" &


################################################### 3PC ###################################################
# Run all three parties with xterm terminal for A.
three: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out 3PC 2 files/parties_localhost files/keyC files/keyCD files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC >/dev/null &
	./CNNFeatureExtraction.out 3PC 1 files/parties_localhost files/keyB files/keyAB files/data/mnist_data_8_BD files/data/mnist_labels_8_BD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD >/dev/null &
	xterm -ls -xrm 'XTerm*selectToClipboard:true' -hold -e "./CNNFeatureExtraction.out 3PC 0 files/parties_localhost files/keyA files/keyAB files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC " &
################################################### Remote runs ###################################################
abcFile: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out 3PC 2 files/parties_localhost files/keyC files/keyCD files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC >/dev/null &
	./CNNFeatureExtraction.out 3PC 1 files/parties_localhost files/keyB files/keyAB files/data/mnist_data_8_BD files/data/mnist_labels_8_BD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD >/dev/null &
	./CNNFeatureExtraction.out 3PC 0 files/parties_localhost files/keyA files/keyAB files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC >3PC.txt 
	@echo "Execution completed"

defFile: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out 4PC 3 files/parties_localhost files/keyD files/keyCD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD >/dev/null &
	./CNNFeatureExtraction.out 4PC 2 files/parties_localhost files/keyC files/keyCD files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC >/dev/null &
	./CNNFeatureExtraction.out 4PC 1 files/parties_localhost files/keyB files/keyAB files/data/mnist_data_8_BD files/data/mnist_labels_8_BD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD >/dev/null &
	./CNNFeatureExtraction.out 4PC 0 files/parties_localhost files/keyA files/keyAB files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC >4PC.txt 
	@echo "Execution completed"

abcTerminal: CNNFeatureExtraction.out
	./CNNFeatureExtraction.out 3PC 2 files/parties_localhost files/keyC files/keyCD files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC >/dev/null &
	./CNNFeatureExtraction.out 3PC 1 files/parties_localhost files/keyB files/keyAB files/data/mnist_data_8_BD files/data/mnist_labels_8_BD files/data/mnist_data_8_BD files/data/mnist_labels_8_BD >/dev/null &
	./CNNFeatureExtraction.out 3PC 0 files/parties_localhost files/keyA files/keyAB files/data/mnist_data_8_AC files/data/mnist_labels_8_AC files/data/mnist_data_8_AC files/data/mnist_labels_8_AC 
	@echo "Execution completed"