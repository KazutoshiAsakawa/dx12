#pragma once
#include <functional>

class Statemachine
{
public:

	struct Node
	{
		std::function<Node* ()> phase;
	};

	void Run();

	inline void SetPhase(std::function<Node* ()> phase) { this->phase = phase; };
protected:
	// �G�̍s���p�^�[��
	std::function<Node* ()> phase;
};