#pragma once

#include <functional>
#include <deque>
#include <mutex>
#include <condition_variable>

class CMailBox
{
public:
	virtual ~CMailBox() = default;

	typedef std::function<void()> FunctionType;

	void SendCall(const FunctionType&, bool = false, bool = false);
	void SendCall(FunctionType&&);
	void FlushCalls();

	bool IsPending() const;
	bool ReceiveCall();
	void WaitForCall();
	void WaitForCall(unsigned int);
	void SetCanWait(bool);
	void ProcessUntilBreakPoint();
	void Reset();

private:
	struct MESSAGE
	{
		MESSAGE() = default;

		MESSAGE(MESSAGE&&) = default;
		MESSAGE(const MESSAGE&) = delete;

		MESSAGE& operator=(MESSAGE&&) = default;
		MESSAGE& operator=(const MESSAGE&) = delete;

		FunctionType function;
		bool sync;
		bool breakpoint;
	};

	typedef std::deque<MESSAGE> FunctionCallQueue;

	FunctionCallQueue m_calls;
	std::mutex m_callMutex;
	std::condition_variable m_callFinished;
	std::condition_variable m_waitCondition;
	bool m_callDone;
	bool m_canWait = true;
};
