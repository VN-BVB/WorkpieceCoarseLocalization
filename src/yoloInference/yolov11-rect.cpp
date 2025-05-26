#include "src/yolov11WeldSeamArea/yolov11-rect.h"
#include <QFile>
#include <QImage>
#include <QBuffer>
#include <opencv2/opencv.hpp>
#include <cassert>
#include <cstring>

Yolov11_Rect::Yolov11_Rect(const std::string& engine_file_path) {
    std::cout << "File path: " << engine_file_path << std::endl;
    std::ifstream file(engine_file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << engine_file_path << std::endl;
        return;
    }
    assert(file.good());
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    char* trtModelStream = new char[size];
    if (!trtModelStream) {
        std::cerr << "Memory allocation for the model stream failed!" << std::endl;
        return;
    }
    assert(trtModelStream);
    file.read(trtModelStream, size);
    file.close();
    initLibNvInferPlugins(&this->gLogger, "");
    this->runtime = nvinfer1::createInferRuntime(this->gLogger);
    assert(this->runtime != nullptr);

    this->engine = this->runtime->deserializeCudaEngine(trtModelStream, size);
    assert(this->engine != nullptr);
    delete[] trtModelStream;
    this->context = this->engine->createExecutionContext();

    assert(this->context != nullptr);
    cudaStreamCreate(&this->stream);

#ifdef TRT_10
    this->num_bindings = this->engine->getNbIOTensors();
#else
    this->num_bindings = this->num_bindings = this->engine->getNbBindings();
#endif

    for (int i = 0; i < this->num_bindings; ++i) {
        Binding binding;
        nvinfer1::Dims dims;

#ifdef TRT_10
        std::string name = this->engine->getIOTensorName(i);
        nvinfer1::DataType dtype = this->engine->getTensorDataType(name.c_str());
#else
        nvinfer1::DataType dtype = this->engine->getBindingDataType(i);
        std::string name = this->engine->getBindingName(i);
#endif
        binding.name = name;
        binding.dsize = type_to_size(dtype);
#ifdef TRT_10
        bool IsInput = engine->getTensorIOMode(name.c_str()) == nvinfer1::TensorIOMode::kINPUT;
#else
        bool IsInput = engine->bindingIsInput(i);
#endif
        if (IsInput) {
            this->num_inputs += 1;
#ifdef TRT_10
            dims = this->engine->getProfileShape(name.c_str(), 0, nvinfer1::OptProfileSelector::kMAX);
            // set max opt shape
            this->context->setInputShape(name.c_str(), dims);
#else
            dims = this->engine->getProfileDimensions(i, 0, nvinfer1::OptProfileSelector::kMAX);
            // set max opt shape
            this->context->setBindingDimensions(i, dims);
#endif
            binding.size = get_size_by_dims(dims);
            binding.dims = dims;
            this->input_bindings.push_back(binding);
        } else {
#ifdef TRT_10
            dims = this->context->getTensorShape(name.c_str());
#else
            dims = this->context->getBindingDimensions(i);
#endif
            binding.size = get_size_by_dims(dims);
            binding.dims = dims;
            this->output_bindings.push_back(binding);
            this->num_outputs += 1;
        }
    }
}

Yolov11_Rect::~Yolov11_Rect() {
#ifdef TRT_10
    delete this->context;
    delete this->engine;
    delete this->runtime;
#else
    this->context->destroy();
    this->engine->destroy();
    this->runtime->destroy();
#endif
    cudaStreamDestroy(this->stream);
    for (auto& ptr : this->device_ptrs) {
        CHECK(cudaFree(ptr));
    }

    for (auto& ptr : this->host_ptrs) {
        CHECK(cudaFreeHost(ptr));
    }
}
void Yolov11_Rect::make_pipe(bool warmup) {
    for (auto& bindings : this->input_bindings) {
        void* d_ptr;
        CHECK(cudaMallocAsync(&d_ptr, bindings.size * bindings.dsize, this->stream));
        this->device_ptrs.push_back(d_ptr);

#ifdef TRT_10
        auto name = bindings.name.c_str();
        this->context->setInputShape(name, bindings.dims);
        this->context->setTensorAddress(name, d_ptr);
#endif
    }

    for (auto& bindings : this->output_bindings) {
        void *d_ptr, *h_ptr;
        size_t size = bindings.size * bindings.dsize;
        CHECK(cudaMallocAsync(&d_ptr, size, this->stream));
        CHECK(cudaHostAlloc(&h_ptr, size, 0));
        this->device_ptrs.push_back(d_ptr);
        this->host_ptrs.push_back(h_ptr);

#ifdef TRT_10
        auto name = bindings.name.c_str();
        this->context->setTensorAddress(name, d_ptr);
#endif
    }

    if (warmup) {
        for (int i = 0; i < 10; i++) {
            for (auto& bindings : this->input_bindings) {
                size_t size = bindings.size * bindings.dsize;
                void* h_ptr = malloc(size);
                memset(h_ptr, 0, size);
                CHECK(cudaMemcpyAsync(this->device_ptrs[0], h_ptr, size, cudaMemcpyHostToDevice, this->stream));
                free(h_ptr);
            }
            this->infer();
        }
        // printf("model warmup 10 times\n");
    }
}

// 将图像调整到深度学习需要的尺寸
void Yolov11_Rect::letterbox(const cv::Mat& image, cv::Mat& out, cv::Size& size) {
    const float inp_h = size.height;
    const float inp_w = size.width;
    float height = image.rows;
    float width = image.cols;

    float r = std::min(inp_h / height, inp_w / width);
    int padw = std::round(width * r);
    int padh = std::round(height * r);

    cv::Mat tmp;
    if ((int)width != padw || (int)height != padh) {
        //        std::cout << "Source image size: " << image.rows << "x" << image.cols << std::endl;
        //        std::cout << "Target size: " << padw << "x" << padh << std::endl;
        //        cv::resize(image, tmp, cv::Size(padw, padh));
        cv::resize(image, tmp, cv::Size(padw, padh));
        //        std::cout << "Resized image size: " << tmp.rows << "x" << tmp.cols << std::endl;
    } else {
        tmp = image.clone();
    }

    float dw = inp_w - padw;
    float dh = inp_h - padh;

    dw /= 2.0f;
    dh /= 2.0f;
    int top = int(std::round(dh - 0.1f));
    int bottom = int(std::round(dh + 0.1f));
    int left = int(std::round(dw - 0.1f));
    int right = int(std::round(dw + 0.1f));

    cv::copyMakeBorder(tmp, tmp, top, bottom, left, right, cv::BORDER_CONSTANT, {114, 114, 114});

    out.create({1, 3, (int)inp_h, (int)inp_w}, CV_32F);

    std::vector<cv::Mat> channels;
    cv::split(tmp, channels);

    cv::Mat c0((int)inp_h, (int)inp_w, CV_32F, (float*)out.data);
    cv::Mat c1((int)inp_h, (int)inp_w, CV_32F, (float*)out.data + (int)inp_h * (int)inp_w);
    cv::Mat c2((int)inp_h, (int)inp_w, CV_32F, (float*)out.data + (int)inp_h * (int)inp_w * 2);

    channels[0].convertTo(c2, CV_32F, 1 / 255.f);
    channels[1].convertTo(c1, CV_32F, 1 / 255.f);
    channels[2].convertTo(c0, CV_32F, 1 / 255.f);

    this->pparam.ratio = 1 / r;
    this->pparam.dw = dw;
    this->pparam.dh = dh;
    this->pparam.height = height;
    this->pparam.width = width;
    ;
}

void Yolov11_Rect::copy_from_Mat(const cv::Mat& image) {
    cv::Mat nchw;
    auto& in_binding = this->input_bindings[0];
    int width = in_binding.dims.d[3];
    int height = in_binding.dims.d[2];
    cv::Size size{width, height};
    this->letterbox(image, nchw, size);

    CHECK(cudaMemcpyAsync(this->device_ptrs[0], nchw.ptr<float>(), nchw.total() * nchw.elemSize(), cudaMemcpyHostToDevice,
                          this->stream));

#ifdef TRT_10
    auto name = this->input_bindings[0].name.c_str();
    this->context->setInputShape(name, nvinfer1::Dims{
                                           4, {1, 3, size.height, size.width}
                                       });
    this->context->setTensorAddress(name, this->device_ptrs[0]);
#else
    this->context->setBindingDimensions(0, nvinfer1::Dims{
                                               4, {1, 3, height, width}
                                           });
#endif
}

void Yolov11_Rect::copy_from_Mat(const cv::Mat& image, cv::Size& size) {
    cv::Mat nchw;
    this->letterbox(image, nchw, size);
    cudaError_t err = cudaMalloc(&this->device_ptrs[0], nchw.total() * nchw.elemSize());

    CHECK(cudaMemcpyAsync(this->device_ptrs[0], nchw.ptr<float>(), nchw.total() * nchw.elemSize(), cudaMemcpyHostToDevice,
                          this->stream));
    // cudaError_t error = cudaGetLastError();
    // printf("CUDA error: %s\n", cudaGetErrorString(error));

#ifdef TRT_10
    auto name = this->input_bindings[0].name.c_str();
    this->context->setInputShape(name, nvinfer1::Dims{
                                           4, {1, 3, size.height, size.width}
                                       });
    this->context->setTensorAddress(name, this->device_ptrs[0]);
#else
    this->context->setBindingDimensions(0, nvinfer1::Dims{
                                               4, {1, 3, size.height, size.width}
                                           });
#endif
}

void Yolov11_Rect::infer() {
#ifdef TRT_10
    this->context->enqueueV3(this->stream);
#else
    this->context->enqueueV2(this->device_ptrs.data(), this->stream, nullptr);
#endif

     // 将推理结果从 GPU 内存复制到主机内存
    for (int i = 0; i < this->num_outputs; i++) {
        size_t osize = this->output_bindings[i].size * this->output_bindings[i].dsize;
        CHECK(cudaMemcpyAsync(this->host_ptrs[i], this->device_ptrs[i + this->num_inputs], osize, cudaMemcpyDeviceToHost,
                              this->stream));
    }
    // 等待所有的 CUDA 流操作完成
    cudaStreamSynchronize(this->stream);
    // int num_classes = 0;
    // //std::cout << "num_outputs: " << this->num_outputs << std::endl;
    // // 现在处理推理结果
    // for (int i = 0; i < this->num_outputs; i++) {
    //     // 假设输出数据存储在 host_ptrs[i] 中，形状为 num_boxes x (5 + num_classes)
    //     float* output_data = static_cast<float*>(this->host_ptrs[i]);

    //     // 计算检测到的框的数量
    //     // qDebug() << "Output size: " << this->output_bindings[0].dims.d[1];
    //     int num_boxes = this->output_bindings[0].dims.d[1] / (5 + num_classes);
    //     // qDebug()<<"num_boxes"<<num_boxes;
    // }
}


void Yolov11_Rect::postprocess(std::vector<det::Object>& objs, float score_thres, float iou_thres, int topk, int num_labels) {
    objs.clear();
    int num_channels = this->output_bindings[0].dims.d[1];
    //d[1]：通常代表通道数，即每个输出的通道数量。
    //对于 YOLO 等目标检测模型，num_channels 可能代表每个锚点的所有输出类别的数量，
    //qDebug()<<"num_channels"<<num_channels;
    int num_anchors = this->output_bindings[0].dims.d[2];
    //qDebug()<<"num_anchors"<<num_anchors;
    //d[2]：通常代表锚点的数量。在 YOLOv4、YOLOv5 等模型中，
    //网络会为每个锚点预测多个框，每个框对应一个锚点（Anchor）。num_anchors 表示模型输出的锚点数量。

    auto& dw = this->pparam.dw;
    auto& dh = this->pparam.dh;
    auto& width = this->pparam.width;
    auto& height = this->pparam.height;
    auto& ratio = this->pparam.ratio;
    std::vector<cv::RotatedRect> rotated_bboxes;
    std::vector<cv::Rect> bboxes;
    std::vector<float> scores;
    std::vector<int> labels;
    std::vector<int> indices;

    cv::Mat output = cv::Mat(num_channels, num_anchors, CV_32F, static_cast<float*>(this->host_ptrs[0]));
    output = output.t();
    //AABB：5列锚点数行
    //OBB：6列锚点数行
    for (int i = 0; i < num_anchors; i++) {
        auto row_ptr = output.row(i).ptr<float>();
        // //打印这一行的数据
        // std::cout << "Row " << i << ": ";
        // for (int j = 0; j < num_channels; j++) {
        //     std::cout << row_ptr[j] << " ";  // 打印每一个元素
        // }
        // std::cout << std::endl;  // 每一行数据打印完成后换行
        auto bboxes_ptr = row_ptr;
        auto scores_ptr = row_ptr + 4;//AABB和OBB区别可打印出来看看
        auto max_s_ptr = std::max_element(scores_ptr, scores_ptr + num_labels);
        float x0 , y0 , x1 , y1 ;
        //AABB第五个存放类别置信度
        //OBB
        float score = *max_s_ptr;
        //std::cout<<"score"<<score;
        if (score >  score_thres ) {
            cv::Rect_<float> bbox;
            float x = *bboxes_ptr++ - dw;
            float y = *bboxes_ptr++ - dh;
            float w = *bboxes_ptr++;
#ifdef obb
            float h = *bboxes_ptr++;
            float* angle_ptr = bboxes_ptr + num_labels; // 将angle_ptr指向bboxes_ptr后移num_labels的位置
            float angle = *angle_ptr * 180.0 / CV_PI;  // 从该位置读取angle并转换为度
            //float angle = -1.45571* 180.0 / CV_PI;
            //std::cout<<"angle"<<angle<<std::endl;
#else
            float h = *bboxes_ptr;
            float angle = 0.0;
#endif

            // //计算左上角坐标 (x0, y0)
            x0 = clamp((x - 0.5f * w) * ratio, 0.f, width);
            y0 = clamp((y - 0.5f * h) * ratio, 0.f, height);
            // 计算右下角坐标 (x1, y1)
            x1 = clamp((x + 0.5f * w) * ratio, 0.f, width);
            y1 = clamp((y + 0.5f * h) * ratio, 0.f, height);
            //std::cout<<"AABB"<<x0<<" "<<y0<<" "<<x1<<" "<<y1<<" "<<std::endl;
            // rotate_bbox( x, y, w, h, 0.00, ratio, dw, dh, width, height, x0, y0, x1, y1);
            // std::cout<<"OBB"<<x0<<" "<<y0<<" "<<x1<<" "<<y1<<" "<<std::endl;
            cv::RotatedRect rotated_bbox(cv::Point2f(x* ratio, y* ratio), cv::Size2f(w* ratio, h* ratio), angle);
            rotated_bboxes.push_back(rotated_bbox);
            bbox.x = x0;
            bbox.y = y0;
            bbox.width = x1 - x0;
            bbox.height = y1 - y0;
            bboxes.push_back(bbox);
            int label = max_s_ptr - scores_ptr;//计算标签类别
            labels.push_back(label);
            scores.push_back(score);//懒得改结构体了
        }
    }
// iou_thres：交并比（IoU）的阈值，用于判断两个框的重叠程度。
// 如果两个框的 IoU 大于这个值，则认为它们是重复的，保留得分较高的框。
// indices：输出的框索引，筛选后保留的框的索引将存储在这里。
#ifdef BATCHED_NMS
    cv::dnn::NMSBoxesBatched(bboxes, scores, labels, score_thres, iou_thres, indices);
#else
    cv::dnn::NMSBoxes(bboxes, scores, score_thres, iou_thres, indices);
    // std::cout << "Indices: ";
    // for (size_t i = 0; i < indices.size(); ++i) {
    //     std::cout << indices[i] << " ";  // 打印每个索引值
    // }
    // std::cout << std::endl;  // 打印完所有索引后换行
    // std::cout << "OpenCV version: " << CV_VERSION << std::endl;
    // qDebug()<<"obb";
#endif

    int cnt = 0;
    for (auto& i : indices) {//遍历
        if (cnt >= topk) {//限制数量
            break;
        }
        Object obj;
        obj.rotated_rect = rotated_bboxes[i];
        obj.rect = bboxes[i];  // 将框的坐标赋给目标对象
        obj.prob = scores[i];  // 将框的置信度赋给目标对象
        obj.label = labels[i];  // 将框的类别标签赋给目标对象
        objs.push_back(obj);
        cnt += 1;
        // // 打印每个目标对象的信息
        // std::cout << "Object " << i + 1 << " details:" << std::endl;

        // std::cout << "Rotated Rect (center, size, angle): ";
        // std::cout << "Center: (" << obj.rotated_rect.center.x << ", " << obj.rotated_rect.center.y << "), ";
        // std::cout << "Size: (" << obj.rotated_rect.size.width << ", " << obj.rotated_rect.size.height << "), ";
        // std::cout << "Angle: " << obj.rotated_rect.angle << std::endl;
        // // double angle_deg = obj.rotated_rect.angle; // 角度制
        // // // 转换为弧度
        // // double angle_rad = angle_deg * M_PI / 180.0;
        // // // 减去 π/2
        // // angle_rad -= M_PI / 2;
        // // // 再转换为角度
        // // double angle_deg_after = angle_rad ;
        // // std::cout << "Angle after subtraction of π/2: " << angle_deg_after << " degrees" << std::endl;

        // std::cout << "Rect (x, y, width, height): ";
        // std::cout << "x: " << obj.rect.x << ", y: " << obj.rect.y << ", ";
        // std::cout << "Width: " << obj.rect.width << ", Height: " << obj.rect.height << std::endl;

        // std::cout << "Confidence (prob): " << obj.prob << std::endl;

        // std::cout << "Label: " << obj.label << std::endl;

        // std::cout << "---------------------------" << std::endl;

    }
}

void Yolov11_Rect::draw_objects(const cv::Mat& image, cv::Mat& res, const std::vector<det::Object>& objs,
                          const std::vector<std::string>& CLASS_NAMES, const std::vector<std::vector<unsigned int>>& COLORS) {
    res = image.clone();
    for (auto& obj : objs) {
        cv::Scalar color = cv::Scalar(COLORS[obj.label][0], COLORS[obj.label][1], COLORS[obj.label][2]);//RGB
        // 计算文本内容
        char text[256];
        sprintf(text, "%s %.1f%%", CLASS_NAMES[obj.label].c_str(), obj.prob * 100);
#ifdef obb
        cv::Point2f rect_points[4];
        obj.rotated_rect.points(rect_points);
        // 绘制旋转矩形
        for (int j = 0; j < 4; j++) {
            cv::line(res, rect_points[j], rect_points[(j + 1) % 4], color, 2);
        }
        // for (int i = 0; i < 4; i++) {
        //     std::cout << "Point " << i << ": ("
        //               << rect_points[i].x << ", "
        //               << rect_points[i].y << ")" << std::endl;
        // }
        //旋转框的文本绘制功能存在缺陷，因为图像多次迭代，文本多了图像会模糊，可修改。
        cv::Point2f pt1 = rect_points[1];
        cv::Point2f pt2 = rect_points[2];


        // 计算上边的方向向量
        cv::Point2f direction = pt2 - pt1;
        float angle = std::atan2(direction.y, direction.x); // 计算旋转角度

        // 获取文本的大小和基线
        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 1, 1, &baseLine);

        // 计算文本的中心位置，使文本基线与矩形的上边对齐
        cv::Point2f center = (pt1 + pt2) * 0.5; // 上边的中心点
        cv::Point2f text_pos = center - cv::Point2f(label_size.width / 2, label_size.height / 2); // 确保文本居中

        // 旋转矩阵
        cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle * 180.0 / CV_PI, 1.0); // 旋转矩阵
        cv::Mat inverse_rotation_matrix = cv::getRotationMatrix2D(center, -angle * 180.0 / CV_PI, 1.0);

        // 复制原图像
        cv::Mat temp = res.clone();

        // 旋转图像
        cv::Mat temp2 = res.clone();
        cv::warpAffine(temp, temp2, rotation_matrix, temp.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);

        // 绘制文本
        cv::putText(temp2, text, text_pos, cv::FONT_HERSHEY_SIMPLEX, 1, {0, 0, 255}, 2);
        cv::warpAffine(temp2, res, inverse_rotation_matrix, res.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
        // 反向旋转
        // cv::Mat temp3;
        // cv::warpAffine(temp2, temp3, inverse_rotation_matrix, res.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
        // temp3.copyTo(res);
        //写字
        //cv::putText(res, text, cv::Point(text_pos.x, text_pos.y ), cv::FONT_HERSHEY_SIMPLEX, 1, {0, 0, 255}, 2);
        // 获取文本框区域
        //cv::Rect roi(text_pos.x, text_pos.y, 200, 200);

        // 只复制该区域到目标图像
        //temp3(roi).copyTo(res(roi));


#else
        cv::rectangle(res, obj.rect, color, 2); \
            // for (const auto& obj : objs) { \
            //     std::cout << "Object Rect: " \
            //               << "x: " << obj.rect.x << ", " \
            //               << "y: " << obj.rect.y << ", " \
            //               << "width: " << obj.rect.width +obj.rect.x<< ", " \
            //               << "height: " << obj.rect.height+obj.rect.y << std::endl; \
            // } \
            char text[256]; \
            sprintf(text, "%s %.1f%%", CLASS_NAMES[obj.label].c_str(), obj.prob * 100);//字写入内存

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 1, 1, &baseLine);//计算文本大小，文本，字体，缩放，粗细
        int x = (int)obj.rect.x;
        int y = (int)obj.rect.y ;
        if (y > res.rows) {
            y = res.rows;
        }
        //填充
        cv::rectangle(res, cv::Rect(x, y-label_size.height - baseLine, label_size.width, label_size.height + baseLine), {0, 0, 255}, -1);
        //写字
        cv::putText(res, text, cv::Point(x, y-baseLine ), cv::FONT_HERSHEY_SIMPLEX, 1, {255, 255, 255}, 2);
#endif
    }

}
