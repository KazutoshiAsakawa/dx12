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
	// 敵の行動パターン
	std::function<Node* ()> phase;
};