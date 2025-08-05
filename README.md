# LED Block Cipher Cryptanalysis using ML

This research project evaluates the cryptographic security of the LED Block Cipher by analyzing its resistance to machine learning-based cryptanalysis. 
Using neural networks, we attempt to assess whether the different classes of ciphertexts produced by the LED Block Cipher exhibit any distinguishable patterns that could be exploited by adversaries.
 
## The LED Block Cipher

The LED (Lightweight Encryption Device) Block Cipher is a lightweight symmetric-key cryptographic algorithm designed for resource-constrained environments such as RFID tags, sensor networks, and IoT devices.

It operates on 64-bit data blocks, represented as a 4×4 matrix of 16 4-bit 'nibbles', and runs for 32 rounds grouped into 8 'steps'. Each 'round' performs four core operations: AddConstants (adds round-dependent constants to break symmetry), SubCells (applies a PRESENT-like S-Box for non-linearity), ShiftRows (permutes rows to enhance diffusion), and MixColumnsSerial (performs column-wise mixing via matrix multiplication). These operations collectively ensure strong confusion and diffusion with low hardware overhead. The cipher supports 64-bit or 128-bit keys, with key addition applied every four rounds; either directly using a 64-bit key or alternating between halves of a 128-bit key.
 
[Guo, J., Peyrin, T., Poschmann, A., & Robshaw, M. (2012). The LED Block Cipher. IACR ePrint Archive, 2012/600](https://eprint.iacr.org/2012/600.pdf)

## Project Structure

```
├── LED Block Cipher Research Paper.pdf      # The Official LED Block Cipher Research Paper
├── led-bytes.c                              # The Official Reference LED Block Cipher Source Code
├── led-0.c                                  # For creating the Label 0 Dataset
├── led-1.c                                  # For creating the Label 1 Dataset
├── led-2.c                                  # For creating the Label 2 Dataset
├── led_ciphertexts0.csv                     # Label 0 Dataset - 2^17 samples
├── led_ciphertexts1.csv                     # Label 1 Dataset - 2^17 samples
├── led_ciphertexts2.csv                     # Label 2 Dataset - 2^17 samples
├── LED Cipher Analysis.ipynb                # The Main Cryptanalysis Jupyter Notebook file
├── README.md                                # This Documentation file
└── LICENSE                                  # The MIT License file
```

### Datasets

- Three Datasets (`led_ciphertexts0.csv`, `led_ciphertexts1.csv`, `led_ciphertexts2.csv`), each containing columns:
  - Plaintext
  - Ciphertext (as hexadecimal strings)
  - Class/label (0, 1, 2) 

- A fixed 128-bit key, 00112233445566778899AABBCCDDEEFF, is used uniformly across all the classes/categories.
- The main difference in the datasets for the three classes lies in the S-Box used (for the SubCells operation). The label 0 dataset uses the default S-Box. For the label 1 dataset, the outputs of two input bits of the S-Box are swapped; and for the label 2 dataset, the output of one input bit of the S-Box is modified. 

- Dataset Size: Each file contains 2^17 (131,072) samples, for a total of 393,216 samples.

## ML-Cryptanalysis Approach

1. The ciphertexts are converted from hexadecimal to binary arrays (64 bits per ciphertext). All datasets are combined and shuffled to ensure randomness.
2. The data is split into training and validation sets (80/20 split), maintaining class balance.

**Note**: The validation set is intentionally used for both, model validation during training and final testing, in order to provide the most favorable conditions for the ML 'attack'. If the models cannot learn distinguishable patterns even with this advantage, it strengthens the evidence for LED Block Cipher's security.

3. Model Architectures:
   - Artificial Neural Network (ANN):
     - Multiple dense layers with batch normalization and dropout for regularization.
     - Output layer uses softmax activation for 3-class classification.
   - Convolutional Neural Network (CNN):
     - 1D convolutional layers to capture local patterns in the binary ciphertexts.
     - Dense layers for final classification.
4. Both models are trained to predict the class labels from the ciphertexts.
5. Results:
   - Both the ANN and CNN models achieve an accuracy of approximately 34% (in the most favorable conditions), which is close to random guessing for a 3-class problem (i.e., ~33.3% by chance, 1/3 probability for all 3 classes). Moreover, even at this low performance, the models showed signs of overfitting. The validation accuracies consistently remained below 34%, indicating that the models weren't able to generalize at all (even increasing epochs, tuning hyperparameters, or applying additional feature extraction techniques did not help). 
   - Precision, recall, and F1-scores are low and balanced (except for some minor deviations due to training-induced class bias) across all the classes too, indicating no class is inherently easier to learn or distinguish.  
   - Confusion matrices show predictions are distributed nearly uniformly across all the classes (the observed spikes in ANN are again due to the training bias, nothing actually meaningful).       
 
### Interpretation
 
- The models, effectively speaking, haven't been able to learn any meaningful exploitable patterns that distinguish between the ciphertexts of the different classes.
- The achieved accuracies indicate a near-random guessing performance for both the fully-connected and convolutional models, which is a positive security indicator for the LED cipher.
- Even with large datasets and advanced neural model classifiers at play, we observed that the LED cipher successfully obscured the relationship between the plaintexts and ciphertexts across the different dataset variants.
- These results provide confidence for, and support LED Block Cipher's suitability for lightweight security-critical applications, especially against machine learning-based cryptanalysis attacks.

## How to Run

1. Make sure Python 3.8+ installed on your system. 
2. Install the required dependencies:
```python
pip install pandas numpy scikit-learn tensorflow matplotlib seaborn tqdm
```
3. Run the dataset generation C code files (if you wish to generate the datasets locally, otherwise they are given in the repository too) `led-0.c`, `led-1.c`, and `led-2.c`.
4. Now, with the datasets set up, open and run the cells of the `LED Cipher Analysis.ipynb` Jupyter Notebook file.

## Acknowledgements

Special thanks to the Scientific Analysis Group (SAG), Defence Research and Development Organization (DRDO), for supporting this research.

## Contributing

Contributions are welcome!

## License

Distributed under the MIT License. 

LED-Cipher-ML-Cryptanalysis
A machine learning-based cryptanalysis of the LED Block Cipher using neural networks to evaluate its resistance against statistical pattern-recognition attacks.
     