#ifndef CLOVER_SIM_ANALYSIS_MANAGER_HH
#define CLOVER_SIM_ANALYSIS_MANAGER_HH

#include "LENSLongAnalysisManager.hh"
#include <iostream>

class CloverSimAnalysisManager : public LENSLongAnalysisManager {
public:
	CloverSimAnalysisManager();
	virtual ~CloverSimAnalysisManager() {}

	void MakeAndOpenOutputFile() override;

protected:
	std::string file_name_;
	std::string option_;
};
#endif // !CLOVER_SIM_ANALYSIS_MANAGER_HH
