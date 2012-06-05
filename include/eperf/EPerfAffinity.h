/**
 * Header for ePerF Framework modeling the Affinity
 *
 * @author	Sebastian Dressler
 * @date	2012-06-02
 *
 * \addtogroup ePerF
 * @{
 *
 * */

#ifndef EPERF_AFFINITY_H
#define EPERF_AFFINITY_H

#include <map>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdexcept>

#ifdef __cplusplus

namespace ENHANCE {
class EPerfAffinity {

private:
	// Holds the lower wanted speedup
	double Sl;
	// Holds the upper wanted speedup
	double Su;
	// Holds the slope for the lower speedup
	double sigl;
	// Holds the slope for the upper speedup
	double sigu;
	// Holds the reference timing
	double tRef;
	// Holds the measured timings with respect to the architecture the measurement was made
	std::map<std::string, double> t;

	// Affinity matrix
	std::map<std::string, std::map<int, int> > A;

	// Affinity computation
	double computeAffinity(double S) {
		if (S < Sl) {
			return 0.0;
		} else if ((Sl <= S) && (S < 1.0)) {
			return sigl * (S - 1.0) + 0.5;
		} else if (S == 1.0) {
			return 0.5;
		} else if ((1.0 < S) && (S <= Su)) {
			return sigu * (S - 1.0) + 0.5;
		} else {
			return 1.0;
		}
	}

	void calcSlopes() {
		sigl = std::abs(0.5 / (Sl - 1.0));
		sigu = std::abs(0.5 / (Su - 1.0));
	}

public:
	Affinity(double _Sl = 1.0, double _Su = 1.0) {
		Sl = _Sl;
		Su = _Su;
		calcSlopes();
	}

	Affinity(std::string _p) {
		readMeasurementsFromFile(_p);
		calcSlopes();
	}

	void addReferenceMeasurement(double t) { tRef = t;}
	void addArchMeasurement(std::string a, double _t) { t[a] = _t; }
	
	// Dirty hack
	void addAffinityToMatrix(std::string arch, int N, int a) {
		std::map<int, int> temp = A[arch];
		temp[N] = a;
		A[arch] = temp;
	}
	
	void writeAffinityMatrixToFile(std::string path) {

		std::ofstream f(path.c_str(), std::ios_base::trunc | std::ios_base::binary);

		if (!f.is_open()) {
			throw std::runtime_error("Error: Could not open file to write measurements");
		}

		//Write how many entries
		unsigned int N = A.size();
		f.write(reinterpret_cast<char*>(&N), sizeof(unsigned int));

		// Now step through map
		for (std::map<std::string, std::map<int, int> >::iterator i = A.begin(); i != A.end(); ++i) {
			// First: Length of string, including '\0'
			unsigned int sl = (*i).first.size() + 1;
			f.write(reinterpret_cast<char*>(&sl), sizeof(unsigned int));
			// Write string for arch identification
			f.write((*i).first.c_str(), sl);

			// Write how many entries for second map
			std::map<int, int> sub = (*i).second;
			N = sub.size();
			f.write(reinterpret_cast<char*>(&N), sizeof(unsigned int));

			// Step through sub
			for (std::map<int, int>::iterator j = sub.begin(); j != sub.end(); ++j) {
				// Write N
				int val = (*j).first;
				f.write(reinterpret_cast<char*>(&val), sizeof(int));
				// Write Affinity
				val = (*j).second;
				f.write(reinterpret_cast<char*>(&val), sizeof(int));
			}

		}

		f.close();

	}

	std::map<std::string, double> getSpeedups() {
		std::map<std::string, double> s;
		for (std::map<std::string, double>::iterator i = t.begin(); i != t.end(); ++i) {
			s[(*i).first] = tRef / (*i).second;
		}
		return s;
	}

	std::map<std::string, double> getAffinities() {
		std::map<std::string, double> s = getSpeedups();

		std::map<std::string, double> a;
		for (std::map<std::string, double>::iterator i = s.begin(); i != s.end(); ++i) {
			a[(*i).first] = computeAffinity((*i).second);
		}

		return a;
	}

	int getIntNormAffinityForArch(int min, int max, std::string arch) {
		
		double a = computeAffinity(tRef / t[arch]);

		if (a == 0.0) {
			return 0;
		} else if (a >= 1.0) {
			return max;
		} else {
			return (int)(a * (double)(max - min) + min);
		}

	}

	void writeNormalizedAffinitiesToFile(int min, int max, std::string path) {

		std::ofstream f(path.c_str(), std::ios_base::trunc | std::ios_base::binary);

		if (!f.is_open()) {
			throw std::runtime_error("Error: Could not open file to write measurements");
		}

		//Write how many entries
		unsigned int N = t.size();
		f.write(reinterpret_cast<char*>(&N), sizeof(unsigned int));

		// Now step through map
		for (std::map<std::string, double>::iterator i = t.begin(); i != t.end(); ++i) {
			// First: Length of string, including '\0'
			unsigned int sl = (*i).first.size() + 1;
			f.write(reinterpret_cast<char*>(&sl), sizeof(unsigned int));
			// Write string for arch identification
			f.write((*i).first.c_str(), sl);

			// Write the normalized affinity
			int a = getIntNormAffinityForArch(min, max, (*i).first);
			f.write(reinterpret_cast<char*>(&a), sizeof(int));
		}

		f.close();

	}

	void writeMeasurementsToFile(std::string path) {

		std::ofstream f(path.c_str(), std::ios_base::trunc | std::ios_base::binary);

		if (!f.is_open()) {
			throw std::runtime_error("Error: Could not open file to write measurements");
		}

		// Write file content (first write -> reference measurement)
		f.write(reinterpret_cast<char*>(&tRef), sizeof(double));
		// Now min / max speedups
		f.write(reinterpret_cast<char*>(&Sl), sizeof(double));
		f.write(reinterpret_cast<char*>(&Su), sizeof(double));

		//Write how many entries
		unsigned int N = t.size();
		f.write(reinterpret_cast<char*>(&N), sizeof(unsigned int));

		// Now step through map
		for (std::map<std::string, double>::iterator i = t.begin(); i != t.end(); ++i) {
			// First: Length of string, including '\0'
			unsigned int sl = (*i).first.size() + 1;
			f.write(reinterpret_cast<char*>(&sl), sizeof(unsigned int));
			// Write string for arch identification
			f.write((*i).first.c_str(), sl);
			// Write measured time
			f.write(reinterpret_cast<char*>(&(*i).second), sizeof(double));
		}

		f.close();
		
	}

	void readMeasurementsFromFile(std::string path) {
		std::ifstream f(path.c_str(), std::ios_base::binary);

		if (!f.is_open()) {
			throw std::runtime_error("Error: Could not open file to read measurements");
		}

		// First: Read reference measurement
		f.read(reinterpret_cast<char*>(&tRef), sizeof(double));
		// Min / Max speedups
		f.read(reinterpret_cast<char*>(&Sl), sizeof(double));
		f.read(reinterpret_cast<char*>(&Su), sizeof(double));

		// Read how many entries
		unsigned int N;
		f.read(reinterpret_cast<char*>(&N), sizeof(unsigned int));

		// Read remaining file content
		for (unsigned int i = 0; i < N; i++) {
			// Read the length of the string
			unsigned int sl;
			f.read(reinterpret_cast<char*>(&sl), sizeof(unsigned int));

			// Now we can read in the string
			char *s = new char[sl];
			f.read(s, sl);

			// Read in the value
			double value;
			f.read(reinterpret_cast<char*>(&value), sizeof(double));
			
			// Save value to map
			t[std::string(s)] = value;

			delete[] s;
		}

		f.close();

	}

};
}
#else
typedef struct Affinity Affinity;
#endif

#endif /* EPERF_AFFINITY_H */

/** @} */
