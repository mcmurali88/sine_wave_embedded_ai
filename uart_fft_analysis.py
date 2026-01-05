import numpy as np
import matplotlib.pyplot as plt
import re
import serial

# --- Configuration ---
COM_PORT = 'COM13'         # Change to your UART COM port
BAUD_RATE = 115200         # Set to your STM32 UART baud rate
SAMPLE_RATE = 1000         # Hz (should match your STM32 test case)
NUM_SAMPLES = 1000         # Number of samples to read

# --- Read UART data from COM port ---
input_vals = []
output_vals = []
pattern = re.compile(r"TestCase7: ([\d.-]+) \| Output: ([\d.-]+)")

print(f"Reading {NUM_SAMPLES} samples from {COM_PORT}...")
with serial.Serial(COM_PORT, BAUD_RATE, timeout=2) as ser:
    while len(input_vals) < NUM_SAMPLES:
        line = ser.readline().decode(errors='ignore')
        match = pattern.search(line)
        if match:
            input_vals.append(float(match.group(1)))
            output_vals.append(float(match.group(2)))

input_vals = np.array(input_vals)
output_vals = np.array(output_vals)
print(f"Read {len(input_vals)} samples.")

# --- FFT Analysis ---
def plot_fft(signal, sample_rate, title):
    N = len(signal)
    fft_vals = np.fft.fft(signal)
    fft_freqs = np.fft.fftfreq(N, 1/sample_rate)
    fft_magnitude = np.abs(fft_vals)
    # Only plot positive frequencies
    pos_mask = fft_freqs > 0
    plt.figure(figsize=(8,4))
    plt.plot(fft_freqs[pos_mask], fft_magnitude[pos_mask])
    plt.title(f'FFT of {title}')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.grid(True)
    plt.show()
    # Print peak frequency
    peak_freq = fft_freqs[pos_mask][np.argmax(fft_magnitude[pos_mask])]
    print(f'Peak frequency in {title}: {peak_freq:.2f} Hz')

# --- Plot FFT for input and output ---
plot_fft(input_vals, SAMPLE_RATE, 'Input Signal')
plot_fft(output_vals, SAMPLE_RATE, 'Output Signal')
