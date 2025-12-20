// ==========================================================
// >> 2nd ORDER BUTTERWORTH FILTER CLASS <<
// ==========================================================

class ButterworthFilter {
  private:
  float b0, b1, b2, a1_norm, a2_norm; // Coefficients
  float x_n_1, x_n_2, y_n_1, y_n_2; // Delay Line

  public:
  // Constructor Methods
  ButterworthFilter(float fc, float fs) {
    float T = 1.0 / fs;
    float w0 = 2.0 * PI * fc;
    float w0T = w0 * T;
    float w0T_sq = w0T * w0T;
    float sqrt2_w0T = 1.41421356 * w0T;

    // Denominator Factor
    float a0 = w0T_sq + sqrt2_w0T + 4.0;

    // Numerator coefficients
    b0 = w0T_sq / a0;
    b1 = 2.0 * w0T_sq / a0;
    b2 = w0T_sq / a0;

    // Normalized denominator coefficients
    a1_norm = (2.0 * w0T_sq - 8.0) / a0;
    a2_norm = (w0T_sq - sqrt2_w0T + 4.0) / a0;

    reset(0.0);
  }

  // Resets the filter memory to a specific value to prevent startup spikes.
  void reset(float initialValue = 0.0) {
    x_n_1 = x_n_2 = y_n_1 = y_n_2 = initialValue;
  }

  float update(float input) {

    // Update with difference equation
    float y_n = b0 * input + b1 * x_n_1 + b2 * x_n_2 - a1_norm * y_n_1 - a2_norm * y_n_2;

    // Shift memory for next iteration
    x_n_2 = x_n_1;
    x_n_1 = input;
    y_n_2 = y_n_1;
    y_n_1 = y_n;

    return y_n;
  }
};

// ==========================================================
// >> 2nd ORDER BUTTERWORTH FILTER CLASS <<
// ==========================================================


// ==========================================================
// >> 1st ORDER LOW PASS FILTER CLASS <<
// ==========================================================

class LowPassFilter {
  private:
    float alpha, beta;   // Coefficients derived from Tustin method
    float x_n_1, y_n_1;  // Delay Line (1st order only needs 1 sample memory)

  public:
    // Constructor using derivation from Source 
    LowPassFilter(float fc, float fs) {
      float T = 1.0 / fs;
      float w = 2.0 * PI * fc; // Cut-off in rad/s [cite: 4]
      
      float denominator = 2.0 + (w * T);
      
      // alpha = (w*T) / (2 + w*T) 
      alpha = (w * T) / denominator;
      
      // beta = (w*T - 2) / (2 + w*T) 
      beta = (w * T - 2.0) / denominator;

      reset(0.0);
    }

    void reset(float initialValue = 0.0) {
      x_n_1 = y_n_1 = initialValue;
    }

    float update(float input) {
      // Difference Equation: y[n] = alpha*x[n] + alpha*x[n-1] - beta*y[n-1] 
      float y_n = (alpha * input) + (alpha * x_n_1) - (beta * y_n_1);

      // Shift memory
      x_n_1 = input;
      y_n_1 = y_n;

      return y_n;
    }
};

// ==========================================================
// >> 1st ORDER LOW PASS FILTER CLASS <<
// ==========================================================


#define PI 3.1415926535897932384626433832795

// User Configure
const unsigned long DELAY_US = 10000; // Microseconds -> 10ms
const float SAMPLING_FREQ = 1000000.0 / (float)DELAY_US;
const float CUT_OFF_FREQ = 5; // Common Cutoff Frequency


ButterworthFilter signalFilter(CUT_OFF_FREQ, SAMPLING_FREQ); // Butterworth filter object
LowPassFilter basicFilter(CUT_OFF_FREQ, SAMPLING_FREQ); // Low Pass Filter Object


unsigned long previous_micros = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long current_micros = micros();

  if((current_micros - previous_micros) >= DELAY_US)
  {
    previous_micros += DELAY_US;

    // Generate signal: 2Hz Sine (Clean) + 10Hz Sine (Noise)
    float t = micros() / 1.0e6;
    float raw_signal = 20*sin(2*PI*2*t) + 3*sin(2*PI*20*t);
    
    // Filter Signal
    float filtered_signal_1stLP = basicFilter.update(raw_signal);
    float filtered_signal_butter = signalFilter.update(raw_signal);

    // Output for Serial Plotter
    Serial.print("Raw Signal:");
    Serial.print(raw_signal);
    Serial.print(",");
    Serial.print("1st order LPF:");
    Serial.print(filtered_signal_1stLP);
    Serial.print(",");
    Serial.print("2nd Order BW:");
    Serial.println(filtered_signal_butter);

  }

}
