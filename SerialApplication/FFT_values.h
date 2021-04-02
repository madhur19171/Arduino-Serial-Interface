#include <cmath>
const int rep_bits = 13;
const int N = 1 << rep_bits;

static bool weights_filled = false;

static double real_data[N];
static double imag_data[N];


static void fillWeights() {
	double w = 2 * 3.14159265358979323846 / N;
	for (int i = 0; i < N; i++) {
		real_data[i] = cos(w * i);
		imag_data[i] = -sin(w * i);
	}
	weights_filled = true;
}