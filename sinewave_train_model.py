import numpy as np
import math
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow.keras import layers
# Print versions
print(f"TensorFlow version: {tf.__version__}")
print(f"NumPy version: {np.__version__}")
#print(math.__version__) # Removed this line as math module doesn't have a __version__ attribute
print(tf.keras.__version__)
# Define dummy data for demonstration
nsamples = 1000
# Define split ratios
val_ratio = 0.2
test_ratio = 0.2
tflite_model_name = 'Sinewave_model' # Define the tflite_model_name variable
c_model_name = 'Sinewave_model' # Define the c_model_name variable

# Generate some random data
np.random.seed(1234) # Random values between -10 and 10
x_values = np.random.uniform(low=0, high=2*math.pi, size=nsamples)
plt.plot(x_values)
plt.show()
y_values = np.sin(x_values)+(0.1 * np.random.randn(x_values.shape[0]))
plt.plot(x_values, y_values, '.')
plt.show()


# Split the dataset into training, validation, and test sets
val_split = int(val_ratio * nsamples)
test_split = int(val_split + (test_ratio * nsamples))
x_val, x_test, x_train = np.split(x_values, [val_split, test_split])
y_val, y_test, y_train = np.split(y_values, [val_split, test_split])

# Check that our splits add up correctly
assert(x_train.size + x_val.size + x_test.size) == nsamples

# Plot the data in each partition in different colors:
plt.plot(x_train, y_train, 'b.', label="Train")
plt.plot(x_test, y_test, 'r.', label="Test")
plt.plot(x_val, y_val, 'y.', label="Validate")
plt.legend()
plt.show()

# Create a model
model = tf.keras.Sequential()
model.add(layers.Dense(16, activation='relu', input_shape=(1,)))
model.add(layers.Dense(16, activation='relu'))
model.add(layers.Dense(1))
model.summary()

# Compile the model
model.compile(optimizer='rmsprop', loss='mse', metrics=['mae'])
# Train model
history = model.fit(x_train,
                    y_train,
                    epochs=500,
                    batch_size=100,
                    validation_data=(x_val, y_val))
# Plot the training history
loss = history.history['loss']
val_loss = history.history['val_loss']

epochs = range(1, len(loss) + 1)
plt.plot(epochs, loss, 'bo', label='Training loss')
plt.plot(epochs, val_loss, 'b', label='Validation loss')
plt.title('Training and validation loss')
plt.legend()
plt.show()
predictions = model.predict(x_test)
plt.clf()
plt.title('Comparison of predictions to actual values')
plt.plot(x_test, y_test, 'b.', label='Actual values')
plt.plot(x_test, predictions, 'r.', label='Predictions')
plt.legend()
model.predict(np.array([0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0]))
# Convert Keras model to a tflite model
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_SIZE]
tflite_model = converter.convert()

#tflite_model_name = 'model' # Define the tflite_model_name variable
open(tflite_model_name + '.tflite', 'wb').write(tflite_model)
# Function: Convert some hex value into an array for C programming
def hex_to_c_array(hex_data, var_name):

    c_str = ''

    # Create header guard
    c_str += '#ifndef ' + var_name.upper() + '_H\n'
    c_str += '#define ' + var_name.upper() + '_H\n\n'

    # Add array length at top of file
    c_str += '\nunsigned int ' + var_name + '_len = ' + str(len(hex_data)) + ';\n'

    # Declare C variable
    c_str += 'unsigned char ' + var_name + '[] = {'
    hex_array = []
    for i, val in enumerate(hex_data):

        # Construct string from hex
        hex_str = format(val, '#04x')

        # Add formatting so each line stays within 80 characters
        if (i + 1) < len(hex_data):
            hex_str += ','
        if (i + 1) % 12 == 0:
            hex_str += '\n '
        hex_array.append(hex_str)

    # Add closing brace
    c_str += '\n ' + format(' '.join(hex_array)) + '\n};\n\n'

    # Close out header guard
    c_str += '#endif //' + var_name.upper() + '_H'

    return c_str
# Write TFLite model to a C source (or header) file
c_model_name = tflite_model_name # Assign c_model_name for use in the next line
with open(c_model_name + '.h', 'w') as file:
    file.write(hex_to_c_array(tflite_model, c_model_name))