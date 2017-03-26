#pragma once

/********************************************************************
**                                                                 **
**     Neural Network                       -Trial ver-            **
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
#include <vector>
#include <random>
#include <algorithm>
#include <initializer_list>


#include <cmath>
#include <ctime>
#include <cassert>

#include "Object.h"

#define DECLARE_GET_FUNCTION(func_name, var_type, var_name)\
	var_type Get##func_name##(){\
		return this->var_name;\
	}

#define DECLARE_SET_FUNCTION(func_name, var_type, var_name)\
	void Set##func_name##(var_type var_name){\
		this->var_name = var_name;\
	}

#define DECLARE_GET_SET_FUNCTION(func_name, var_type, var_name)\
	DECLARE_GET_FUNCTION(func_name, var_type, var_name)\
	DECLARE_SET_FUNCTION(func_name, var_type, var_name)

#define DUMP(name) #name


namespace ednn {

	static std::mt19937 gen((unsigned int)time(0));
	static std::uniform_real_distribution<double> distrb(0.0, 1.0);
	static double Rate = 0.2;

	static double _default_activate_func(double v) {
		return 1 / (1 + std::exp(-v));
	}

	static double _default_activate_func_D(double o) {
		return o * (1 - o);
	}


	/***********  class Declaration  ***************/

	class Neuron;
	class Layer;
	class NeuralNetwork;
	class NeuralNetworkTrainer;




	/**********************************************
	*                   Neuron                    *
	**********************************************/

	class Neuron {
	public:

		/* --- Constructor --- */
		Neuron(double(*activate_func)(double) = _default_activate_func, double(*activate_func_D)(double) = _default_activate_func_D)
			: actv(activate_func), actvD(_default_activate_func_D) {}

		Neuron(std::vector<double> weights, double bias, double output=0, double delta=0, double(*activate_func)(double) = _default_activate_func
			, double(*activate_func_D)(double) = _default_activate_func_D)
			: actv(activate_func), actvD(_default_activate_func_D), m_wt(weights), m_bs(bias), m_op(output), m_dt(delta) {
			
		}


		/* --- Settings --- */
		DECLARE_GET_SET_FUNCTION(Weight, std::vector<double> &, m_wt)
		DECLARE_GET_SET_FUNCTION(Bias, double, m_bs)
		DECLARE_GET_SET_FUNCTION(Output, double, m_op)

		void SetActFunc(double(*activation_func)(double)) {
			actv = activation_func;
		}


		/* --- Initialize --- */
		void Init() {
			InitWeights();
			InitBias();
		}

		void InitWeights() {
			for (auto &iter : m_wt) {
				iter = distrb(gen);
			}
		}

		void InitBias() {
			m_bs = distrb(gen);
		}

		void _generate_link(size_t sz) {
			m_wt.resize(sz);
		}

		/* --- Other Functions --- */

		/* --- Forward Transfer --- */
		friend Neuron &operator<<(Neuron &n, const std::vector<Neuron> &list) {
			double sum = n.m_bs;
			for (size_t i = 0; i < list.size(); ++i) {
				sum += list[i].m_op * n.m_wt[i];
			}
			n.m_op = n.actv(sum);
			return n;
		}

		friend Neuron &operator<<(Neuron &n, const std::vector<double> &inputs) {
			double sum = n.m_bs;
			for (size_t i = 0; i < inputs.size(); ++i) {
				sum += inputs[i] * n.m_wt[i];
			}
			n.m_op = n.actv(sum);
			return n;
		}

		/* -=-- Back Propagation --- */
		friend std::vector<Neuron> &operator>>(std::vector<Neuron> &from, std::vector<Neuron> &to) {
			for (size_t j = 0; j < to.size(); j++) {
				to[j].m_dt = 0;
				for (size_t l = 0; l < from.size(); l++) {
					to[j].m_dt += from[l].m_dt * from[l].m_wt[j];
				}

				to[j].m_dt *= to[j].actvD(to[j].m_op);
			}

			for (size_t j = 0; j < from.size(); j++) {
				from[j].Update(to);
			}

			return to;
		}

		friend std::vector<Neuron> &operator>>(std::vector<double> &expt, std::vector<Neuron> &tlist) {
			for (size_t j = 0; j < tlist.size(); j++) {
				tlist[j].m_dt = (tlist[j].m_op - expt[j]) * tlist[j].actvD(tlist[j].m_op);
			}
			return tlist;
		}


		//Update Weights
		inline void Update(std::vector<Neuron> &tlist) {
			for (size_t i = 0; i < tlist.size(); i++) {
				m_wt[i] -= Rate *tlist[i].m_op * m_dt;
			}
			m_bs -= Rate *m_dt;
		}


		/* --- Output Stream --- */

		/*
		void printFormat(LayoutPrinter &printer, int cur) {
			printer.newline("Neuron{", cur);
			printer.newline("weight: ", cur + 1);
			for (size_t i = 0; i < this->m_wt.size(); ++i) {
				printer.append(std::string() << m_wt[i]);
				if (i != m_wt.size())printer.append(", ");
			}

			printer.newline(std::string() << "bias: " << m_bs, cur + 1);
			printer.newline(std::string() << "delta: " << m_dt, cur + 1);
			printer.newline(std::string() << "output: " << m_op, cur + 1);

			printer.newline("}", cur);
		}*/

		void printFormat(std::ostream &out, int cur) {
			out << tab(cur) << "Neuron{" << std::endl;
			out << tab(cur + 1) << "weight: ";
			for (size_t i = 0; i < this->m_wt.size(); ++i) {
				out << m_wt[i];
				if (i != m_wt.size())out << ", ";
			}
			out << std::endl;

			out << tab(cur + 1) << "bias: " << m_bs << std::endl;
			out << tab(cur + 1) << "delta: " << m_dt << std::endl;
			out << tab(cur + 1) << "output: " << m_op << std::endl;

			out << tab(cur) << "}" << std::endl;
		}

		friend std::ostream &operator<<(std::ostream &out, Neuron &n) {
			//LayoutPrinter printer;
			n.printFormat(out, 0);
			//out << printer;
			return out;
		}


	private:


		/* --- Variables --- */

		double m_op;  //neuron output
		std::vector<double> m_wt;  //weights list
		double m_bs; //bias
		double m_dt;
		double(*actv)(double) = NULL;  //Activation function
		double(*actvD)(double) = NULL;


	};





	/**********************************************
	*                    Layer                    *
	**********************************************/

	class Layer {
	public:

		/* --- Constructor --- */
		Layer(size_t neuron_num = 0) {
			m_layer.resize(neuron_num);
		}

		Layer(std::vector<Neuron> layer) : m_layer(layer) {}

		Layer(const Layer &l) : m_layer(l.m_layer) {}

		/* --- Settings --- */

		DECLARE_GET_SET_FUNCTION(Layer, std::vector<Neuron> &, m_layer)

		void SetOutput(std::vector<double> &output) {
			assert(output.size() >= m_layer.size());
			for (size_t i = 0; i < m_layer.size(); ++i) {
				m_layer[i].SetOutput(output[i]);
			}
		}
		std::vector<double> GetOutput() {
			std::vector<double> v;
			for (auto &iter : m_layer) {
				v.push_back(iter.GetOutput());
			}
			return v;
		}


		/* --- Initialize --- */
		void _generate_link(size_t sz) {
			for (auto &iter : m_layer) {
				iter._generate_link(sz);
			}
		}

		void Init() {
			for (auto &iter : m_layer) {
				iter.Init();
			}
		}


		/* --- Other Functions --- */
		size_t size() {
			return m_layer.size();
		}



		friend Layer &operator<<(Layer &to, Layer &from) {
			for (auto &iter : to.m_layer) {
				iter << from.m_layer;
			}
			return to;
		}

		friend Layer &operator>>(Layer &from, Layer &to) {
			from.m_layer >> to.m_layer;
			return to;
		}

		friend Layer &operator>>(std::vector<double> &inputs, Layer &to) {
			inputs >> to.m_layer;
			return to;
		}


		/* --- Operator Overloading --- */
		Neuron &operator[](size_t idx) {
			assert(idx < m_layer.size());
			return m_layer[idx];
		}


		/* --- Output Stream --- */
		
		/*void printFormat(LayoutPrinter &printer, int cur) {
			printer.newline("Layer{", cur);
			for (size_t i = 0; i < this->m_layer.size(); ++i) {
				this->m_layer[i].printFormat(printer, cur + 1);
			}
			printer.newline("}", cur);
		}*/

		void printFormat(std::ostream &out, int cur) {
			out << tab(cur) << "Layer{" << std::endl;
			for (size_t i = 0; i < this->m_layer.size(); ++i) {
				this->m_layer[i].printFormat(out, cur + 1);
			}
			out << tab(cur) << "}" << std::endl;
		}

		friend std::ostream &operator<<(std::ostream &out, Layer &l) {
			l.printFormat(out, 0);
			return out;
		}

	private:

		std::vector<Neuron> m_layer;

	};





	/**********************************************
	*                NeuralNetwork                *
	**********************************************/

	class NeuralNetwork {
	public:

		/* --- Constructor --- */
		NeuralNetwork() : m_lynm({ 2, 1, 2 }) {}
		NeuralNetwork(std::vector<int> &layers) : m_lynm(layers) {}
		NeuralNetwork(std::initializer_list<int> layers) : m_lynm(layers) {}
		NeuralNetwork(const NeuralNetwork &nn) : m_net(nn.m_net), m_lynm(nn.m_lynm) {}

		/* --- Initialize --- */
		void CreateNetwork(std::vector<Layer> &layers) {
			m_lynm.resize(layers.size());
			for (size_t i = 0; i < layers.size(); ++i) {
				m_lynm[i] = (int)layers[i].size();
			}
			m_net = layers;
			_generate_link();
		}

		void CreateNetwork() {
			_generate_network();
			_generate_link();
		}

		void InitNetwork() {
			for (auto &iter : m_net) {
				iter.Init();
			}
		}

		/* --- Other Functions --- */
		size_t size() {
			return m_net.size();
		}


		/* --- Operator Overloading --- */
		Layer &operator[](size_t idx) {
			assert(idx < m_net.size());
			return m_net[idx];
		}

		/* --- Output Stream --- */

		/*
		void printFormat(LayoutPrinter &printer, int cur) {
			printer.newline("NeuralNetwork{", cur);
			for (size_t i = 0; i < this->m_net.size(); ++i) {
				this->m_net[i].printFormat(printer, cur+1);
			}
			printer.newline("}", cur);
		}*/

		void printFormat(std::ostream &out, int cur) {
			out << tab(cur) << "NeuralNetwork{" << std::endl;
			for (size_t i = 0; i < this->m_net.size(); ++i) {
				this->m_net[i].printFormat(out, cur + 1);
			}
			out << tab(cur) << "}" << std::endl;
			//printer.newline("}", cur);
		}

		friend std::ostream &operator<<(std::ostream &out, NeuralNetwork &nn) {
			//LayoutPrinter printer;
			nn.printFormat(out, 0);
			//out << printer;
			/*
			out << "NeuralNetwork{" << std::endl;
			for (size_t i = 0; i < nn.m_net.size(); ++i) {
				out << "Layer " << i + 1 << " {" << std::endl;
				out << nn.m_net[i] << std::endl;
				out << "}" << std::endl;
			}
			out << "}";*/
			return out;
		}

		/* --- Forward Transfer --- */
		friend std::vector<double> operator<<(NeuralNetwork &nn, std::vector<double> &input) {
			nn[0].SetOutput(input);
			for (size_t i = 1; i < nn.size(); ++i) {
				nn[i] << nn[i - 1];
			}
			return nn[nn.size() - 1].GetOutput();
		}

		/* --- Back Propagation --- */
		friend void operator>>(std::vector<double> &input, NeuralNetwork &nn) {
			input >> nn[nn.size() - 1];
			for (size_t i = nn.size() - 2; i >= 1; --i) {
				nn[i + 1] >> nn[i];
			}
			for (size_t i = 0; i < nn[1].size(); ++i) {
				nn[1][i].Update(nn[0].GetLayer());
			}
		}

	private:
		friend class NeuralNetworkTrainer;

		std::vector<int>m_lynm;
		std::vector<Layer> m_net;  //neural network

		void _generate_network() {
			m_net.resize(m_lynm.size());
			for (size_t i = 0; i < m_lynm.size(); ++i) {
				m_net[i] = Layer(m_lynm[i]);
			}
		}

		void _generate_link() {
			for (size_t i = 1; i < m_net.size(); ++i) {
				m_net[i]._generate_link(m_net[i - 1].size());
			}
		}
	};

}




#include <fstream>
#include <string>

namespace ednn{

	/**********************************************
	*            NeuralNetworkTrainer             *
	**********************************************/


	class NeuralNetworkTrainer {
	public:
		NeuralNetworkTrainer(NeuralNetwork &nn, int epoch = 20, double rate = 0.5, bool shuffle = true) 
			:m_nn(&nn), _epoch(epoch), _shuffle(shuffle){
			Rate = rate;
		}

		void SetLogStream(std::ostream &output_stream) {
			out = &output_stream;
		}


	private:
		NeuralNetwork *m_nn = nullptr;
		int _epoch;
		bool _shuffle;
		std::ostream *out;

		friend void operator<<(NeuralNetworkTrainer &nn, std::vector<std::vector<double>> &inputs) {
			size_t ipsz = (*nn.m_nn)[0].size();
			for (int ep = 1; ep <= nn._epoch; ++ep) {
				if(nn._shuffle)std::random_shuffle(inputs.begin(), inputs.end());
				
				double e_sum = 0;
				for (size_t d = 0; d < inputs.size(); ++d) {
					std::vector<double> op = (*nn.m_nn) << inputs[d];

					for (size_t n = 0; n < op.size(); ++n) {
						e_sum += std::abs(op[n] - inputs[d][ipsz + n]);
						op[n] = inputs[d][ipsz + n];
					}
					
					op >> (*nn.m_nn);

				}
				
				(*nn.out) << "Epoch[" << ep << "]: Rate=" << Rate << ", Error=" << e_sum << std::endl;
			}
		}
	};

	

	//forward transfer
	Neuron &operator<<(Neuron &n, Layer &l) {
		return n << l.GetLayer();
	}

}

