// Free for non-commercial, non-military, and non-critical 
// use unless incorporated in OpenCV. 
// Inherits OpenCV Licence if in OpenCV.


#ifndef COSTVOLUME_HPP
#define COSTVOLUME_HPP


#include <opencv2/gpu/gpu.hpp>
#include <opencv2/gpu/stream_accessor.hpp>
typedef  int FrameID;

class CostVolume
{
public:
    FrameID fid;
    int rows;
    int cols;
    int layers;
    float near;
    float far;
    float depthStep;
    float initialWeight;
    cv::Mat cameraMatrix;//Note! should be in OpenCV format
    cv::Mat R;
    cv::Mat T;
    cv::Mat projection;//projects world coordinates (x,y,z) into (rows,cols,layers)

    cv::gpu::GpuMat baseImage;
    cv::gpu::GpuMat baseImageGray;
    cv::gpu::GpuMat lo;
    cv::gpu::GpuMat hi;
    cv::gpu::GpuMat loInd;

    float * data;
    float * hits;

    cv::gpu::GpuMat dataContainer;
    cv::gpu::GpuMat hitContainer;

    int count;
    cv::gpu::Stream cvStream;

    void updateCost(const cv::gpu::CudaMem& image, const cv::Mat& R, const cv::Mat& T);
    CostVolume(cv::Mat image, FrameID _fid, int _layers, float _near, float _far,
            cv::Mat R, cv::Mat T, cv::Mat _cameraMatrix, float initialCost=3.0, float initialWeight=.001);

    //HACK: remove this function in release
    cv::Mat downloadOldStyle( int layer){
        cv::Mat cost;
        cv::gpu::GpuMat tmp=dataContainer.rowRange(layer,layer+1);
        tmp.download(cost);
        cost=cost.reshape(0,rows);
        return cost;
    }

private:
    void solveProjection(const cv::Mat& R, const cv::Mat& T);
    void checkInputs(const cv::Mat& R, const cv::Mat& T,
            const cv::Mat& _cameraMatrix);
    cudaTextureObject_t simpleTex(const cv::Mat& image,cv::gpu::Stream cvStream=cv::gpu::Stream::Null());

private:
    //temp variables ("static" containers)
    cv::Ptr<char> _cuArray;//cudaArray*
    cv::Ptr<char> _texObj;//cudaTextureObject_t
    
};

#endif // COSTVOLUME_HPP
