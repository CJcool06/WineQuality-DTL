# WineQuality-DTL
 Decision Tree Learning implementation to predict wine quality given unique training and testing datasets

 An example dataset for learning and testing can be found in `/input`.

 A larger dataset can be found in `/data`.

 Compiling is as simple as `make`.

 Run the program by executing the resulting binary with parameters:
 `<training file> <testing file> <minleaf>`

 Lower the minleaf value for more accurate results.

 An example can be seen below:
 `./winequality.bin data/train data/test-sample 30`

 This algorithm predicts wine quality with greater than 95% accuracy.