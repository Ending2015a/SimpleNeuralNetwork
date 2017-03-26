#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>

#include <cstdlib>


#include "NetworkParser.h"
#include "NeuralNetwork.h"



void Network_Output_Test() {
	//read network file to stringstream
	std::ifstream fin("Test/network/after.ed");
	std::stringstream ss;
	ss << fin.rdbuf();

	//parse to network
	std::ofstream fout("Test/network/after2.ed");
	std::ofstream fout2("Test/network/layout.txt");
	ednn::NetworkParser parser(ss.str());

	ednn::object o = parser.ParseToObj();
	fout2 << o << std::endl;

	fout << std::setprecision(20);
	ednn::NeuralNetwork nn = parser.Parse();
	fout << nn << std::endl;
}

void Network_Parser_Test() {
	std::ifstream fin("Test/network_parser/network1.ed");
	std::stringstream ss;
	ss << fin.rdbuf();



	ednn::NetworkParser parser(ss.str());

	ednn::object o = parser.ParseToObj();

	std::ofstream fout("Test/network_parser/layout.txt");

	fout << o << std::endl;
}

void Network_Test() {
	std::ofstream fout("stemp.txt");
	fout << std::setprecision(20);

	std::vector<std::vector<double>> datasets({
		{ 2.7810836,2.550537003,       1, 0 },
		{ 1.465489372,2.362125076,     1, 0 },
		{ 3.396561688,4.400293529,     1, 0 },
		{ 1.38807019,1.850220317,      1, 0 },
		{ 3.06407232,3.005305973,      1, 0 },
		{ 7.627531214,2.759262235,     0, 1 },
		{ 5.332441248,2.088626775,     0, 1 },
		{ 6.922596716,1.77106367,      0, 1 },
		{ 8.675418651,-0.242068655,    0, 1 },
		{ 7.673756466,3.508563011,     0, 1 }
	});


	//Create Network
	ednn::NeuralNetwork nn({ 2, 2, 2 });
	nn.CreateNetwork();
	nn.InitNetwork();

	//print out network before training
	fout << "Initial state............" << std::endl;
	fout << nn;
	fout << std::endl << std::endl;

	//Create trainer
	ednn::NeuralNetworkTrainer trainer(nn, 1000, 0.5, false);
	trainer.SetLogStream(fout);
	trainer << datasets;


	//print out network after training
	fout << "After ............" << std::endl;
	fout << nn;
	fout << std::endl << std::endl;


	for (size_t i = 0; i < datasets.size(); i++) {
		std::vector<double> output = nn << datasets[i];
		for (size_t j = 0; j < output.size(); j++) {
			fout << output[j] << " ";
		}
		fout << std::distance(output.begin(), std::max_element(output.begin(), output.end())) << std::endl;
	}
}


void Generate_TestCase(std::vector<std::vector<double>> &testcase) {
	for (size_t i = 0; i < testcase.size(); ++i) {
		testcase[i].resize(4);
		if (testcase[i][2] == 0) {
			testcase[i][2] = 1;
			testcase[i][3] = 0;
		}
		else {
			testcase[i][2] = 0;
			testcase[i][3] = 1;
		}
	}
}

int main(void) {
	


	
	//Network_Parser_Test();
	//Network_Output_Test();
	//Network_Test();

	//system("pause");
	return 0;
}