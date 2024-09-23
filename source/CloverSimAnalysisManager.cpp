#include "CloverSimAnalysisManager.hh"
#include "YamlReader.hpp"

CloverSimAnalysisManager::CloverSimAnalysisManager() {
	YamlReader yaml_reader("CloverSimAnalysisManager");
	file_name_ = yaml_reader.GetString("FileName",false,"OpticalPhotonInfo.root");
	option_ = yaml_reader.GetString("FileOption",false,"RECREATE");
	MakeAndOpenOutputFile();
}

void CloverSimAnalysisManager::MakeAndOpenOutputFile()
{
	m_RootOutputFile = new TFile(file_name_.c_str(),option_.c_str());
	std::cout << "[CloverSimAnalysisManager.cpp]: ROOT file opened " << file_name_ << std::endl;
}