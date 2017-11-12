#include "SeetaFace.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>

using namespace std;
using namespace seeta;

Detector::Detector(const char* model_name) : seeta::FaceDetection(model_name){
	this->SetMinFaceSize(40);
	this->SetScoreThresh(2.f);
	this->SetImagePyramidScaleFactor(0.8f);
	this->SetWindowStep(4, 4);
}


SeetaFace::SeetaFace(){
    this->detector = new Detector("/home/chenqi/chenqi/c++/Qt/Face/model/seeta_fd_frontal_v1.0.bin");
    this->point_detector = new seeta::FaceAlignment("/home/chenqi/chenqi/c++/Qt/Face/model/seeta_fa_v1.1.bin");
    this->face_recognizer = new seeta::FaceIdentification("/home/chenqi/chenqi/c++/Qt/Face/model/seeta_fr_v1.0.bin");
}


float* SeetaFace::NewFeatureBuffer(){
	return new float[this->face_recognizer->feature_size()];
}


bool SeetaFace::GetFeature(cv::Mat color, float* feat){
	//read pic greyscale
    cv::Mat src_img;
    cv::cvtColor(color, src_img, CV_BGR2GRAY);
	seeta::ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
	src_img_data.data = src_img.data;


	//read pic color
    seeta::ImageData src_img_data_color(color.cols, color.rows, color.channels());
    src_img_data_color.data = color.data;


	std::vector<seeta::FaceInfo> faces = this->detector->Detect(src_img_data);
	int32_t face_num = static_cast<int32_t>(faces.size());
	if (face_num == 0)
	{
		return false;
	}
  // std::vector<seeta::Rect> bbox;
    for (int32_t i = 0; i < face_num; i++)
  {
    bbox.push_back(faces[i].bbox);
  }
	seeta::FacialLandmark points[5];
	this->point_detector->PointDetectLandmarks(src_img_data, faces[0], points);


	this->face_recognizer->ExtractFeatureWithCrop(src_img_data_color, points, feat);


	return true;
};


int SeetaFace::GetFeatureDims(){
	return this->face_recognizer->feature_size();
}


float SeetaFace::FeatureCompare(float* feat1, float* feat2){
	return this->face_recognizer->CalcSimilarity(feat1, feat2);
}


FeatureGroup::FeatureGroup(int feat_dims, SeetaFace* seeta_face){
	this->feat_dims = feat_dims;
	this->seeta_face = seeta_face;
	feats.clear();
}


int FeatureGroup::GetFeatureDims(){
	return this->feat_dims;
}


bool FeatureGroup::AddFeature(float* feat, string filename){
	Feature temp;
	float* new_feat = new float[this->feat_dims];
	memcpy(new_feat, feat, sizeof(new_feat)*this->feat_dims);
	temp.data = new_feat;
	temp.filename = filename;
	this->feats.push_back(temp);
	return true;
}


bool FeatureGroup::SaveModel(string model_file){
	std::ofstream file;
	file.open(model_file);
	file << int(this->feats.size()) << std::endl;
	file << this->feat_dims << std::endl;
	for (int i = 0; i<int(this->feats.size()); i++){
		file << this->feats[i].filename << std::endl;
		for (int j = 0; j<this->feat_dims; j++)
			file << this->feats[i].data[j] << " ";
		file << std::endl;
	}
	file.close();
	return true;
}


FeatureGroup::FeatureGroup(string model_file, SeetaFace* seeta_face){
	std::ifstream file;
	file.open(model_file);
	int size;
	float* new_feat;
	char* buffer = new char[1000];
	file >> size;
	file >> this->feat_dims;
	for (int i = 0; i<size; i++){
		Feature temp;
		file.getline(buffer, 1000);
		while (buffer[0] == '\0' || buffer[0] == ' '){
			file.getline(buffer, 1000);
		}
		temp.filename = buffer;
		new_feat = new float[this->feat_dims];
		for (int j = 0; j<this->feat_dims; j++)
			file >> new_feat[j];
		temp.data = new_feat;
		this->feats.push_back(temp);
	}
	file.close();
	this->seeta_face = seeta_face;
}


FeatureGroup::~FeatureGroup(){
	for (int i = 0; i<int(this->feats.size()); i++)
		delete[](this->feats[i].data);
}


bool FeatureGroup::FindTopK(int k, float* feat, std::vector<Feature>& result){
	//std::cout << "Calculating Similarities..." << std::endl;
	for (int i = 0; i<int(this->feats.size()); i++){
		this->feats[i].similarity_with_goal = this->seeta_face->FeatureCompare(this->feats[i].data, feat);
	}
	//std::cout << "Finding TopK..." << std::endl;
	std::priority_queue<Feature> q;
	for (int i = 0; i<int(this->feats.size()); i++)
		q.push(this->feats[i]);
	for (int i = 0; i<k; i++){
		if (q.empty()) return true;
		result.push_back(q.top());
		q.pop();
	}
	return 0;
}

void saveAllFeature(std::string filepath, std::string savename)
{
    SeetaFace seta;
    DIR *dir;
    struct dirent * ptr;
    char file_list[1000][50];
    int num=0;
    dir = opendir(filepath.c_str());
    while((ptr = readdir(dir)) != NULL) //循环读取目录数据
    {
       if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0)
         continue;
        std::cout<<ptr->d_name<<std::endl;
        strcpy(file_list[num],ptr->d_name ); //存储到数组
        if ( ++num>=1000 ) break;
    }
    closedir(dir);
    ofstream file(savename);
    file << num << std::endl << "2048" << std::endl;
    for(int n=0;n<num;++n)
    {
        cv::Mat color = cv::imread(filepath +"/"+ file_list[n]);
        float gallery_fea[2048];
        seta.GetFeature(color, gallery_fea);

        file << file_list[n] << std::endl;
        for (int i = 0; i < 2048; ++i)
        {
            file << gallery_fea[i] << " ";
        }
        file << std::endl;
        cout << n << "/" << num << endl;
    }
    file.close();
}

