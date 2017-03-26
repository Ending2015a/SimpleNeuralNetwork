#pragma once

/********************************************************************
**                                                                 **
**       Network Parser                       -Trial ver-          **
**                                                                 **
**        Created by Ending2015a (TsuChing Hsiao) on 2017/3/25     **
**                                                                 **
**        Copyright (c) 2015 Ending2015a. All rights reserved.     **
**                                                                 **
*********************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Permission is hereby granted, free of charge, to any person obtaining a copy   *
* of this software and associated documentation files (the "Software"), to deal  *
* in the Software without restriction, including without limitation the rights   *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
* copies of the Software, and to permit persons to whom the Software is          *
* furnished to do so, subject to the following conditions:                       *
*                                                                                *
* The above copyright notice and this permission notice shall be included in     *
* all copies or substantial portions of the Software.                            *
*                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN      *
* THE SOFTWARE.                                                                  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <locale>
#include <map>
#include <algorithm>

#include <cassert>

#include "Object.h"
#include "NeuralNetwork.h"


namespace ednn {



	/**********************************************
	*                NetworkParser                *
	**********************************************/


	//Parse network to object class
	class NetworkParser {
	public:

		/* --- Constructor --- */
		NetworkParser(std::string &conf) {
			buffer.str(conf);
		}

		
		/* --- Parse Function --- */
		object &ParseToObj() {
			pt = &obj; //set pointer to the first layer
			std::string line; //store new line
			std::string pline; //store remain line
			auto iswp = [](char c) {return std::isspace<char>(c, std::locale::classic()); };
			while (std::getline(buffer, line, '\n')) {
				
				line.erase(std::remove_if(line.begin(), line.end(), iswp), line.end());
				line = line.substr(0, line.find("//"));
				pline = pline + line;
				
				_parse_command(pline);
			}
			return obj;
		}

		NeuralNetwork Parse() {
			object o = ParseToObj();
			return NetworkParser::Parse(o);
		}

		static NeuralNetwork Parse(object &obj) {
			assert(obj.contain("NeuralNetwork"));
			object &p = obj["NeuralNetwork"][0];
			assert(p.contain("Layer"));
			std::vector<Layer> layer;
			for (size_t n = 0; n < p["Layer"].size(); ++n) {
				std::vector<Neuron> neurons;
				object &pl = p["Layer"][n];
				
				for (size_t m = 0; m < pl["Neuron"].size(); ++m) {
					object &pn = pl["Neuron"][m];
					std::vector<double> wt;
					double bs=0, op=0, dt=0;
					if (pn.contain("weight")) {
						for (size_t o = 0; o < pn["weight"].size(); ++o) {
							wt.push_back(pn["weight"][o].Double());
						}
					}
					if (pn.contain("bias")) {
						bs = pn["bias"][0].Double();
					}

					if (pn.contain("output")) {
						op = pn["output"][0].Double();
					}

					if (pn.contain("delta")) {
						dt = pn["delta"][0].Double();
					}
					neurons.push_back(Neuron(wt, bs, op, dt));
				}
				layer.push_back(Layer(neurons));
			}
			NeuralNetwork nn;
			nn.CreateNetwork(layer);
			return nn;
		}

	private:
		/* --- Variable --- */
		std::stringstream buffer;  //string buffer
		std::vector<std::string> Sstack;  //object stack
		object obj; //final product
		object *pt;  //current stack pointer

		void _parse_command(std::string &line) {
			size_t np = 0;
			while (1) {
				if (line[np] == '{') {
					std::string Key(line.substr(0, np));
					Sstack.push_back(Key);
					pt->append(Key, object(Key, *pt));
					pt = &(*pt)[Key].back();
					line = line.substr(np + 1, std::string::npos);
					np = 0;
				}
				else if (line[np] == '}') {
					line = line.substr(np + 1, std::string::npos);
					pt = &pt->parent();
					Sstack.pop_back();
					np = 0;
				}
				else if (line[np] == ':') {
					std::string key = line.substr(0, np);
					std::string values = line.substr(np + 1, std::string::npos);
					std::string tok;
					std::stringstream ss(values);
					while (std::getline(ss, tok, ',')) {
						pt->append(key, object(tok, *pt));
					}
					np = 0;
					line = "";
				}
				else ++np;
				
				if (np >= line.size())break;
			}
		}
	};
};
