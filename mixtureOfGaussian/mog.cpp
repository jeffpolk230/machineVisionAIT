
#include <opencv2/opencv.hpp>
#include <numeric>

namespace
{
        const double Epsilon = 1.0e-08;
        
        void observe_probs(const cv::Mat& probs)
        {
                std::vector<double> t(probs.cols, 0.0);
                for ( int n = 0; n < probs.rows; ++n ) {
                        const double* gamma_n = probs.ptr<double>(n);
                        double s = 0.0;
                        for ( int k = 0; k < probs.cols; ++k ) {
                                s += gamma_n[k]; // \gamma_{n,k}
                                t[k] += gamma_n[k];
                        }
                        
                }
                double total = std::accumulate(t.begin(), t.end(), 0.0);
                
        }
        
        void observe_weights(const cv::Mat& weights)
        {
                cv::MatConstIterator_<double> first = weights.begin<double>();
                cv::MatConstIterator_<double> last = weights.end<double>();
                double s = 0.0;
                while ( first != last ) { // loop over k
                        s += *first; // *first means \pi_{k}
                        ++first;
                }
                
        }
        
        void observe_labels_and_means(const cv::Mat& labels, const cv::Mat& means, int height, int width)
        {
                const int dimension = 3;
                cv::Mat rgb_image(height, width, CV_8UC3);
                cv::MatIterator_<cv::Vec3b> rgb_first = rgb_image.begin<cv::Vec3b>();
                cv::MatIterator_<cv::Vec3b> rgb_last = rgb_image.end<cv::Vec3b>();
                cv::MatConstIterator_<int> label_first = labels.begin<int>();
                
                cv::Mat means_u8;
                means.convertTo(means_u8, CV_8UC1, 255.0);
                cv::Mat means_u8c3 = means_u8.reshape(dimension);
                
                while ( rgb_first != rgb_last ) {
                        const cv::Vec3b& rgb = means_u8c3.ptr<cv::Vec3b>(*label_first)[0];
                        *rgb_first = rgb;
                        ++rgb_first;
                        ++label_first;
                }
                cv::imshow("tmp", rgb_image);
                cv::imwrite("./MOGresult.jpg", rgb_image);
                cv::waitKey();
        }
}

int main(int argc, const char * argv[])
{
        cv::Mat image = cv::imread(argv[1]);
        const int image_rows = image.rows;
        const int image_cols = image.cols;
        
        const int dimension = 3;
        
        cv::Mat reshaped_image = image.reshape(1, image_rows * image_cols);
        
        // create an input for the EM Algorithm
        cv::Mat samples;
        reshaped_image.convertTo(samples, CV_64FC1, 1.0 / 255.0);
        
        const int cluster_num = 3;
        cv::EM model = cv::EM(cluster_num);
        
        // prepare outputs
        cv::Mat labels;
        cv::Mat probs;
        cv::Mat log_likelihoods;
        
        // execute EM Algorithm
        model.train(samples, log_likelihoods, labels, probs);
        
        observe_probs(probs);
        
        const cv::Mat& means = model.get<cv::Mat>("means");
        observe_labels_and_means(labels, means, image_rows, image_cols);
        
        const cv::Mat& weights = model.get<cv::Mat>("weights");
        observe_weights(weights);
        
        const std::vector<cv::Mat>& covs = model.get<std::vector<cv::Mat> >("covs");
        
        return 0;
}