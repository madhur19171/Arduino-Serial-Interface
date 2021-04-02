//Best Solution1 32 Point:
#include "FFT_1024.h"
#include <vector>
using namespace std;

void butterfly(data_t xr0, data_t xi0, data_t xr1, data_t xi1, data_t wr, data_t wi,
	data_t* yr0, data_t* yi0, data_t* yr1, data_t* yi1) {
	data_t temp_r, temp_i;

	temp_r = xr1 * wr - xi1 * wi;
	temp_i = xr1 * wi + xi1 * wr;

	*yr0 = xr0 + temp_r;
	*yr1 = xr0 - temp_r;
	*yi0 = xi0 + temp_i;
	*yi1 = xi0 - temp_i;

	/*printf("xr0: %f\t xi0: %f\t xr1: %f\t xi1: %f\t wr: %f\t wi: %f\t yr0: %f\t yi0: %f\t yr1: %f\t yi1: %f\n",
			(float)xr0, (float)xi0, (float)xr1, (float)xi1, (float)wr, (float)wi, (float)*yr0, (float)*yi0, (float)*yr1, (float)*yi1);*/
}

//Done
void FFT_stage(int stage, data_t real_in[N], data_t imag_in[N], data_t real_out[N], data_t imag_out[N], data_t real_w[N], data_t imag_w[N]) {
	const int group_size = stage;
	int ind = 0, j = 0, a = 0, b = 0;
	//printf("Stage %d: \n", stage);
GROUP_LOOP:
	for (int i = 0; i < (N >> 1); i++) {
		j = i % (1 << (group_size - 1));
		ind = j == 0 ? i << 1 : ind;
		a = ind + j;
		b = a + (1 << (group_size - 1));
		butterfly(real_in[a], imag_in[a], real_in[b], imag_in[b], real_w[j << (rep_bits - group_size)], imag_w[j << (rep_bits - group_size)],
			&real_out[a], &imag_out[a], &real_out[b], &imag_out[b]);
	}
}


void FFT_bit_reversal(data_t real[N], data_t imag[N], data_t out_real[N], data_t out_imag[N]) {
	for (int i = 0; i < N; i++) {
		int rev = 0, i_temp = i;
		for (int j = 0; j < rep_bits; j++) {
			rev |= (i_temp & ((data_rep)1)) << (rep_bits - j - 1);
			i_temp >>= 1;
		}
		out_real[i] = real[rev];
		out_imag[i] = imag[rev];
	}
}

//void init_in_memory(vector<short> data_in_arg, data_t data_in_real[N], data_t data_in_imag[N]) {
//	for (int i = 0; i < N; i++) {
//		data_in_imag[i] = (data_t)0;
//		data_in_real[i] = (data_t)data_in_arg[i];
//	}
//}

void init_in_memory(short data_in_arg[N], data_t data_in_real[N], data_t data_in_imag[N]) {
	for (int i = 0; i < N; i++) {
		data_in_imag[i] = (data_t)0;
		data_in_real[i] = (data_t)data_in_arg[i];
	}
}

//void init_out_memory(data_t_sw data_out_real[N], data_t_sw data_out_imag[N], vector<data_t_sw> data_out_real_arg, vector<data_t_sw> data_out_imag_arg) {
//IN_DATA_LOOP:
//	for (int i = 0; i < N; i++) {
//		data_out_real_arg.push_back(data_out_real[i]);
//		data_out_imag_arg.push_back(data_out_imag[i]);
//	}
//}

void init_out_memory(data_t_sw data_out_real[N], data_t_sw data_out_imag[N], data_t_sw data_out_real_arg[N], data_t_sw data_out_imag_arg[N]) {
IN_DATA_LOOP:
	for (int i = 0; i < N; i++) {
		data_out_real_arg[i] = data_out_real[i];
		data_out_imag_arg[i] = data_out_imag[i];
	}
}

void swapArrays(data_t source[N], data_t destination[N]) {
	for (int i = 0; i < N; i++)
		destination[i] = source[i];
}

//void FFT_1024(vector<short> data_in_arg, vector<float> data_out_real_arg, vector<float> data_out_imag_arg)
void FFT_1024(short data_in_arg[N], data_t_sw data_out_real_arg[N], data_t_sw data_out_imag_arg[N]) {

	data_t* real_w = real_data, * imag_w = imag_data;
	//populate_ROM(real_w, imag_w);


	data_t data_in_real[N];

	data_t data_in_imag[N];

	init_in_memory(data_in_arg, data_in_real, data_in_imag);


	//To avoid Single Producer-Single Consumer Violation in dataflow
	//They act as Buffers between two stages
	data_t out_stage_1_real[N], out_stage_1_imag[N];
	data_t out_stage_2_real[N], out_stage_2_imag[N];


	FFT_bit_reversal(data_in_real, data_in_imag, out_stage_1_real, out_stage_1_imag);

	for (int i = 1; i <= 10; i++) {
		FFT_stage(i, out_stage_1_real, out_stage_1_imag, out_stage_2_real, out_stage_2_imag, real_w, imag_w);
		swapArrays(out_stage_2_imag, out_stage_1_imag);
		swapArrays(out_stage_2_real, out_stage_1_real);
	}

	init_out_memory(out_stage_2_real, out_stage_2_imag, data_out_real_arg, data_out_imag_arg);
}
