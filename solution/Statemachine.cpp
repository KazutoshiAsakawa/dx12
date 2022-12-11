#include "Statemachine.h"

void Statemachine::Run()
{
	// phase()の戻り値は
	// phaseの遷移先
	Node* nextPhase = phase();

	// 遷移先があれば
	if (nextPhase != nullptr)
	{
		// phaseの中身を
		// nextPhaseの中身に変更
		phase = nextPhase->phase;
	}

}