#pragma once

/********************************************************************
**                                                                 **
**     Object Container                      -Trial ver-           **
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
#include <map>
#include <deque>
#include <string>

#include <cassert>

namespace ednn {

	/************  class Declaration  *************/


	class object;
	class ObjectParser;
	class ObjectPrinter;


	/************  Other Functions  *************/

	std::string tab(int n) {
		std::string s;
		for (int i = 0; i < n; i++)s += "\t";
		return s;
	}


	/**********************************************
	*                ObjectParser                 *
	**********************************************/

	class ObjectParser {
	public:
	private:
	};



	/**********************************************
	*                LayoutPrinter                *
	**********************************************/
	/*
	class LayoutPrinter {
	public:

		

		void newline(std::string line, int shift) {
			m_str.push_back(line);
			m_sft.push_back(shift);
		}

		void append(std::string line) {
			m_str.back() += line;
		}

		

		friend std::ostream &operator<<(std::ostream &out, LayoutPrinter &p) {
			for (size_t i = 0; i < p.m_str.size(); i++) {
				for (size_t sh = 0; sh < p.m_sft[i]; sh++) {
					out << '\t';
				}
				out << p.m_str[i] << std::endl;
			}
			return out;
		}

	private:
		std::vector<std::string> m_str;
		std::vector<int>m_sft;
	};*/

	


	/**********************************************
	*                   object                    *
	**********************************************/

	class object {
	public:
		std::map<std::string, std::deque<object>> obj_list;
		std::string name;
		object *_p;

		class Printer {
		public:
			static int shift;
		};

		std::deque<object> &child(std::string Key) {
			assert(contain(Key));
			return obj_list[Key];
		}

		object &parent() {
			return *_p;
		}


		object(std::string str, object &parent) : _p(&parent) { name = str; }
		object(std::string str = "") : _p(this){ name = str;  }

		std::deque<object> &operator[](std::string Key) {
			assert(contain(Key));
			return obj_list[Key];
		}

		int Int() {
			return std::stoi(this->name);
		}

		double Double() {
			return std::stod(this->name);
		}

		static int Int(object &obj) {
			return std::stoi(obj.name);
		}

		static double Double(object &obj) {
			return std::stod(obj.name);
		}

		bool contain(std::string Key) {
			return obj_list.find(Key) != obj_list.end();
		}

		std::deque<object> find() {

		}


		size_t size() {
			return obj_list.size();
		}

		void append(std::string Key, object &value) {
			if (!contain(Key)) {
				obj_list[Key] = std::deque<object>({ value });
			}
			else {
				obj_list[Key].push_back(value);
			}
		}

		void erase(std::string Key) {
			auto iter = obj_list.find(Key);
			obj_list.erase(iter);
		}

		void erase(std::string Key, size_t index) {
			auto iter = obj_list.find(Key);
			iter->second.erase(iter->second.begin() + index);
		}

		void clear() {
			obj_list.clear();
			name.clear();
		}
		/*
		void printFormat(LayoutPrinter &printer, int cur) {
			printer.newline("obj{", cur);
			printer.newline(std::string("Name: ") + this->name, cur+1);
			printer.newline(std::string("Parent: ") + this->_p->name, cur+1);


			for (auto iter = this->obj_list.begin(); iter != this->obj_list.end(); ++iter) {
				printer.newline(std::string("Map: ") + iter->first + std::string("{"), cur + 1);

				for (size_t i = 0; i < iter->second.size(); ++i) {
					iter->second[i].printFormat(printer, cur + 2);
				}
				printer.newline("}", cur + 1);
			}

			printer.newline("}", cur);
		}*/

		void printFormat(std::ostream &out, int cur) {
			out << tab(cur) << "obj{" << std::endl;
			out << tab(cur + 1) << "Name: " << this->name << std::endl;
			out << tab(cur + 1) << "Parent: " << this->_p->name << std::endl;

			for (auto iter = this->obj_list.begin(); iter != this->obj_list.end(); ++iter) {
				out << tab(cur + 1) << "Map: " << iter->first << "{" << std::endl;
				//printer.newline(std::string("Map: ") + iter->first + std::string("{"), cur + 1);

				for (size_t i = 0; i < iter->second.size(); ++i) {
					iter->second[i].printFormat(out, cur + 2);
				}
				out << tab(cur + 1) << "}" << std::endl;
				//printer.newline("}", cur + 1);
			}
			out << tab(cur) << "}" << std::endl;
			//printer.newline("}", cur);
		}

		friend std::ostream &operator<<(std::ostream &out, object &obj) {
			auto pre = out.precision();
			//LayoutPrinter printer;
			obj.printFormat(out, 0);
			//out << printer << std::endl;

			/*
			ss << "obj{" << std::endl;
			ss << "name: " << obj.name << std::endl;
			ss << "parent: " << obj._p->name << std::endl;
			for (auto iter = obj.obj_list.begin(); iter != obj.obj_list.end(); iter++) {
				ss << "map:" << iter->first << "{" << std::endl;
				for (size_t i = 0; i < iter->second.size(); ++i) {
					ss << iter->second[i] << std::endl;
				}
				ss << "}" << std::endl;
			}
			ss << "}" << std::endl;*/
			return out;
		}
	};


	

	/*
	template<typename T>
	std::string& operator<<(std::string &str, T value) {
		std::stringstream ss;
		ss << value;
		str = str + ss.str();
		return str;
	}*/



}


