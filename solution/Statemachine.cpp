#include "Statemachine.h"

void Statemachine::Run()
{
	// phase()�̖߂�l��
	// phase�̑J�ڐ�
	Node* nextPhase = phase();

	// �J�ڐ悪�����
	if (nextPhase != nullptr)
	{
		// phase�̒��g��
		// nextPhase�̒��g�ɕύX
		phase = nextPhase->phase;
	}

}