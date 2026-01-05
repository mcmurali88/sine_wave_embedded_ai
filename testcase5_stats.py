# Statistical analysis script for Test Case 5 logs
# Usage: Paste your log data into the LOG_DATA variable and run the script.
import re
import numpy as np

# Paste your log data here as a multi-line string
LOG_DATA = '''
TestCase5: 0.964185 | Output: 0.832860 | Duration: 76
TestCase5: 1.094219 | Output: 0.908852 | Duration: 76
TestCase5: 0.925957 | Output: 0.810751 | Duration: 76
... (truncated for brevity, paste full log here) ...
'''

# Extract input, output, and duration using regex
pattern = re.compile(r"TestCase5: ([\d.]+) \| Output: ([\d.]+) \| Duration: (\d+)")
inputs, outputs, durations = [], [], []
for match in pattern.finditer(LOG_DATA):
    inputs.append(float(match.group(1)))
    outputs.append(float(match.group(2)))
    durations.append(int(match.group(3)))

inputs = np.array(inputs)
outputs = np.array(outputs)
durations = np.array(durations)

# Compute statistics
print("--- Statistical Analysis of Test Case 5 Logs ---")
print(f"Total samples: {len(inputs)}")
print(f"Input:    min={inputs.min():.5f}, max={inputs.max():.5f}, mean={inputs.mean():.5f}, std={inputs.std():.5f}")
print(f"Output:   min={outputs.min():.5f}, max={outputs.max():.5f}, mean={outputs.mean():.5f}, std={outputs.std():.5f}")
print(f"Duration: min={durations.min()}, max={durations.max()}, mean={durations.mean():.2f}, std={durations.std():.2f}")

# Output/Input ratio
ratios = outputs / inputs
print(f"Output/Input Ratio: min={ratios.min():.5f}, max={ratios.max():.5f}, mean={ratios.mean():.5f}, std={ratios.std():.5f}")

# Detect outlier durations
outlier_indices = np.where(durations > durations.mean() + 3*durations.std())[0]
if len(outlier_indices) > 0:
    print(f"\nOutlier durations detected at indices: {outlier_indices.tolist()}")
    for idx in outlier_indices:
        print(f"  Input={inputs[idx]:.5f}, Output={outputs[idx]:.5f}, Duration={durations[idx]}")
else:
    print("\nNo outlier durations detected.")

# Optional: plot (requires matplotlib)
try:
    import matplotlib.pyplot as plt
    plt.figure(figsize=(10,5))
    plt.subplot(2,1,1)
    plt.plot(inputs, label='Input')
    plt.plot(outputs, label='Output')
    plt.legend()
    plt.title('Input and Output Values')
    plt.subplot(2,1,2)
    plt.plot(ratios, label='Output/Input Ratio')
    plt.legend()
    plt.title('Output/Input Ratio')
    plt.tight_layout()
    plt.show()
except ImportError:
    pass
