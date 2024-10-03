#include "Transformations.h"

double Transformations::image_brightnes(cv::Mat img){
    cv::Mat gray = img;
    Transformations::is_image(gray);
    cv::Scalar mean_val = cv::mean(gray);
    return mean_val[0];
}

void Transformations::alter_brightnes(std::string path, double brightnes){
    cv::Mat o_img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    Transformations::is_image(o_img,path);
    cv::Mat img;
    cv::resize(o_img,img,cv::Size(500,500));
    cv::Mat new_img = cv::Mat::zeros(img.size(),img.type());
    const double br = -1*(cv::mean(img)[0] - brightnes);
    const double alpha = 1;
    for(int x = 0; x < img.rows; x++){
        for(int y = 0; y < img.cols; y++){
            new_img.at<uchar>(x,y) = cv::saturate_cast<uchar>(alpha*img.at<uchar>(x,y) + br);
        }
    }
    cv::imshow("Original:", img);
    cv::imshow("Altered",new_img);
    cv::waitKey(0);
}

void Transformations::opening(cv::Mat &image,cv::Mat src, char operation, int kernelSize){
    int flag_operation = 0;
    cv::Mat o_img = src;
    Transformations::is_image(o_img);
    switch (operation)
    {
    case 'O':
        flag_operation = cv::MORPH_OPEN;
        break;
    case 'C':
        flag_operation = cv::MORPH_CLOSE;
        break;
    case 'W':
        flag_operation = cv::MORPH_TOPHAT;
        break;
    case 'B':
        flag_operation = cv::MORPH_BLACKHAT;
        break;
    case 'G':
        flag_operation = cv::MORPH_GRADIENT;
        break;
    default:
        throw std::invalid_argument("Invalid argument in Function Transofrmations::opening()");
        break;
    }
    cv::Mat img;
    cv::resize(o_img,img,cv::Size(1000,1000));
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(img,o_img,flag_operation,kernel);
    image = o_img;
}

void Transformations::dots_remove(cv::Mat &img, int threshold_black, int threshold_size){
    cv::Mat binary_image;
    const double brightnes = Transformations::image_brightnes(img);
    cv::threshold(img,binary_image,(brightnes/5*3),255,cv::THRESH_BINARY_INV);
    cv::Mat labels,stats,centroids;
    const int numLab = cv::connectedComponentsWithStats(binary_image,labels,stats,centroids,4,4);
    cv::Mat o_img;  img.copyTo(o_img);//cv::Mat::zeros(binary_image.size(),0);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::Mat kernel_small = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2, 2));
    std::vector<int> x_set;
    for(int x = 1; x < numLab; x++){
        cv::Mat componentMask = (labels == x)*255;
        int area = stats.at<int>(x, cv::CC_STAT_AREA);
        // std::cout<<meanBrightness<<std::endl;
        if(area < 30)
        {    
            x_set.push_back(x);
            cv::Mat mask;
            cv::dilate(componentMask,mask,kernel_small);
            o_img.setTo(brightnes,mask);
            // Transformations::opening(temp,regionToBlur,'G',5);
            //regionToBlur.copyTo(o_img,componentMask);
        }
    }
    std::cout<<"inpaint time\n";
    for(int x = 0; x < x_set.size(); x++){
        std::cout<<x_set[x]<<std::endl;
        cv::Mat componentMask = (labels == x_set[x]);
        cv::Mat mask = componentMask;
        mask.setTo(brightnes,componentMask);
        cv::dilate(componentMask,mask,kernel);
        cv::inpaint(o_img, mask, o_img, 3, cv::INPAINT_TELEA);
        //cv::GaussianBlur(o_img,o_img,cv::Size(5,5),0);
    }
    //Let's do tomorrow gausian blur in places where the areas leave
    //Then it should do the trick
    //cv::GaussianBlur(o_img,o_img,cv::Size(5,5),4);
    cv::imshow("Show",o_img);
    cv::imshow("Binary",binary_image);
}


