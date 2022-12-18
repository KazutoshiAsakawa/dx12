#include "Statemachine.h"

void Statemachine::Run()
{
	// phase()‚Ì–ß‚è’l‚Í
	// phase‚Ì‘JˆÚæ
	Node* nextPhase = phase();

	// ‘JˆÚæ‚ª‚ ‚ê‚Î
	if (nextPhase != nullptr)
	{
		// phase‚Ì’†g‚ğ
		// nextPhase‚Ì’†g‚É•ÏX
		phase = nextPhase->phase;
	}

}