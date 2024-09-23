#ifndef CLOVER_SIM_EVENT_ACTION_HH
#define CLOVER_SIM_EVENT_ACTION_HH

#include "LENSLongEventAction.hh"
#include "CloverSimAnalysisManager.hh"

class CloverSimEventAction : public LENSLongEventAction {
public:
	CloverSimEventAction(): LENSLongEventAction() {
		delete m_AnalysisManager;
		m_AnalysisManager = (LENSLongAnalysisManager*)new CloverSimAnalysisManager();
	};
	virtual ~CloverSimEventAction() {};
};

#endif 