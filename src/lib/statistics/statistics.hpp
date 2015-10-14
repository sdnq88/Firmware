/**
 * Wellford's algorithm to continuously compute variance is used
 * See http://jonisalonen.com/2013/deriving-welfords-method-for-computing-variance/
 */

#pragma once

namespace statistics {
// TODO! [AK] Consider expanding to other data types
class __EXPORT Variance {
private:
	float mean;
	float prev_mean;
	float square_sum;
	uint32_t sample_count;
public:
	Variance() : mean{0.0f}, prev_mean{0.0f}, square_sum{0.0f}, sample_count{0} {}
	~Variance() {}
	// Adds new sample to the data
	void sample(float value) {
		sample_count++;
		prev_mean = mean;
		mean += (value - mean) / sample_count;
		square_sum += (value - mean) * (value - prev_mean);
	}
	// Returns current variance or -1 if variance is not ready yet
	float get_variance() const {
		if (sample_count >= 2) {
			return (square_sum / (sample_count - 1));
		}
		else {
			return -1.0f;
		}
	}
	// Returns current mean value or NAN if mean is not ready yet
	float get_mean() const {
		if (sample_count >= 1) {
			return mean;
		}
		else {
			return NAN;
		}
	}
	// Returns the size of currently sampled data
	uint32_t get_dataset_size() const {
		return sample_count;
	}
	// Resets the dataset
	void reset() {
		mean = 0.0f;
		square_sum = 0.0f;
		sample_count = 0;
	}
};

} // end namespace statistics
